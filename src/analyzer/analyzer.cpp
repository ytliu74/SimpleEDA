/**
 * @file Analyzer.cpp
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-15
 */

#include "analyzer.h"

using arma::cx_mat;
using std::complex;
using std::cout;
using std::endl;
using std::setw;
using std::vector;

Analyzer::Analyzer(Parser parser) {
    vsrc_vec = parser.GetVsrc();
    isrc_vec = parser.GetIsrc();
    vccs_vec = parser.GetVCCS();
    vcvs_vec = parser.GetVCVS();
    res_vec = parser.GetResistor();
    cap_vec = parser.GetCapacitor();
    ind_vec = parser.GetInductor();
    node_vec = parser.GetNode();

    auto analysis_type = parser.GetAnalysisType();
    auto dc_analysis = parser.GetDcAnalysis();
    auto ac_analysis = parser.GetAcAnalysis();

    switch (analysis_type) {
        case DC: {
            cout << "Running DC analysis" << endl;
            DoDcAnalysis(dc_analysis);
            break;
        }
        case AC: {
            cout << "Running AC analysis" << endl;
            DoAcAnalysis(ac_analysis);
            break;
        }
        default: break;
    }
}

void Analyzer::DoDcAnalysis(const DcAnalysis dc_analysis) {
    double frequency = 0;
    AnalysisMatrix analysis_matrix = GetAnalysisMatrix(frequency);
    cx_mat rhs = analysis_matrix.rhs;

    double start = dc_analysis.start;
    double end = dc_analysis.end;
    double step = dc_analysis.step;

    int scan_vsrc_index = FindNode(modified_node_vec, "i_" + dc_analysis.Vsrc_name);
    for (double v = start; v <= end + 1e-4; v += step) {
        cx_mat scan_rhs = rhs;
        scan_rhs(scan_vsrc_index, 0) = v;
        AnalysisMatrix scan_analysis_mat = {analysis_matrix.node_metrix,
                                            analysis_matrix.node_vec, scan_rhs};
        analysis_matrix_vec.push_back(scan_analysis_mat);
    }
}

// TODO
void Analyzer::DoAcAnalysis(const AcAnalysis ac_analysis) {
    vector<double> scan_freq_vec;

    double f_start = ac_analysis.f_start;
    double f_end = ac_analysis.f_end;
    double point_num = ac_analysis.point_num;

    // Generate scan frequency
    switch (ac_analysis.variation_type) {
        case DEC: {
            double dec_num = log(f_end / f_start) / log(10);

            for (int d = 0; d < dec_num; d++) {
                for (int i = 0; i < point_num; i++) {
                    double f = exp10(d + i / point_num) * f_start;
                    if (f > f_end)
                        break;
                    scan_freq_vec.push_back(f);
                }
            }
            scan_freq_vec.push_back(f_end);
            break;
        }
        case OCT: {
            double oct_num = log(f_end / f_start) / log(2);

            for (int o = 0; o < oct_num; o++) {
                for (int i = 0; i < point_num; i++) {
                    double f = exp2(o + i / point_num) * f_start;
                    if (f > f_end)
                        break;
                    scan_freq_vec.push_back(f);
                }
            }
            scan_freq_vec.push_back(f_end);
            break;
        }
        case LIN: {
            for (int i = 0; i < point_num; i++) {
                double f = (f_end - f_start) * (i / point_num) + f_start;
                scan_freq_vec.push_back(f);
            }
            scan_freq_vec.push_back(f_end);
            break;
        }
        default: break;
    }

    for (auto f : scan_freq_vec) {
        analysis_matrix_vec.push_back(GetAnalysisMatrix(f));
    }
}

AnalysisMatrix Analyzer::GetAnalysisMatrix(const double frequency) {
    const double w = M_2_PI * frequency;  // w = 2 pi f

    // ----- Generate NA metrix -----
    int node_num = node_vec.size();
    cx_mat NA_mat(node_num, node_num, arma::fill::zeros);

    cx_mat RHS(node_num, 1, arma::fill::zeros);

    // Add resistor stamps
    for (Res res : res_vec) {
        int node_1_index = FindNode(node_vec, res.node_1);
        int node_2_index = FindNode(node_vec, res.node_2);
        double conductance = 1 / res.value;
        NA_mat(node_1_index, node_1_index) += complex<double>(conductance, 0);
        NA_mat(node_1_index, node_2_index) += complex<double>(-1 * conductance, 0);
        NA_mat(node_2_index, node_1_index) += complex<double>(-1 * conductance, 0);
        NA_mat(node_2_index, node_2_index) += complex<double>(conductance, 0);
    }

    // Add capacitor stamps
    for (Cap cap : cap_vec) {
        int node_1_index = FindNode(node_vec, cap.node_1);
        int node_2_index = FindNode(node_vec, cap.node_2);
        double value = cap.value * w;
        NA_mat(node_1_index, node_1_index) += complex<double>(0, value);
        NA_mat(node_1_index, node_2_index) += complex<double>(0, -1 * value);
        NA_mat(node_2_index, node_1_index) += complex<double>(0, -1 * value);
        NA_mat(node_2_index, node_2_index) += complex<double>(0, value);
    }

    // Add Current Source
    for (Isrc isrc : isrc_vec) {
        int node_1_index = FindNode(node_vec, isrc.node_1);
        int node_2_index = FindNode(node_vec, isrc.node_2);
        double value = isrc.value;
        // The current run from node_1 to node_2,
        // thus on the LHS, LHS(node_1) = -Ik => RHS(node_1) = +Ik.
        // Same for node_2.
        RHS(node_1_index, 0) += complex<double>(value, 0);
        RHS(node_2_index, 0) += complex<double>(-value, 0);
    }

    // Add VCCS
    for (VCCS vccs : vccs_vec) {
        int node_1_index = FindNode(node_vec, vccs.node_1);
        int node_2_index = FindNode(node_vec, vccs.node_2);
        int ctrl_node_1_index = FindNode(node_vec, vccs.ctrl_node_1);
        int ctrl_node_2_index = FindNode(node_vec, vccs.ctrl_node_2);
        double value = vccs.value;
        NA_mat(node_1_index, ctrl_node_1_index) += complex<double>(value, 0);
        NA_mat(node_1_index, ctrl_node_2_index) += complex<double>(-1 * value, 0);
        NA_mat(node_2_index, ctrl_node_1_index) += complex<double>(-1 * value, 0);
        NA_mat(node_2_index, ctrl_node_2_index) += complex<double>(value, 0);
    }

    // ----- Generate MNA metrix -----
    modified_node_vec = node_vec;

    // Every inducter contributes to one more branch node
    for (Ind ind : ind_vec)
        modified_node_vec.push_back("i_" + ind.name);

    // Every voltage source contributes to one more branch node
    for (Vsrc vsrc : vsrc_vec)
        modified_node_vec.push_back("i_" + vsrc.name);

    // Every VCVS contributes to one more branch node
    for (VCVS vcvs : vcvs_vec)
        modified_node_vec.push_back("i_" + vcvs.name);

    // Initialize MNA metrix
    int modified_node_num = modified_node_vec.size();
    cx_mat MNA_mat = NA_mat;
    MNA_mat.resize(modified_node_num, modified_node_num);

    // Update RHS
    RHS.resize(modified_node_num, 1);

    // Add inductor stamps
    for (Ind ind : ind_vec) {
        int node_1_index = FindNode(modified_node_vec, ind.node_1);
        int node_2_index = FindNode(modified_node_vec, ind.node_2);
        double value = ind.value * w;
        int branch_index = FindNode(modified_node_vec, "i_" + ind.name);
        MNA_mat(branch_index, node_1_index) += complex<double>(1, 0);
        MNA_mat(branch_index, node_2_index) += complex<double>(-1, 0);
        MNA_mat(branch_index, branch_index) += complex<double>(0, -1 * value);
        MNA_mat(node_1_index, branch_index) += complex<double>(1, 0);
        MNA_mat(node_2_index, branch_index) += complex<double>(-1, 0);
    }

    // Add voltage source stamps
    for (Vsrc vsrc : vsrc_vec) {
        int node_1_index = FindNode(modified_node_vec, vsrc.node_1);
        int node_2_index = FindNode(modified_node_vec, vsrc.node_2);
        double value = vsrc.value;
        int branch_index = FindNode(modified_node_vec, "i_" + vsrc.name);
        MNA_mat(branch_index, node_1_index) += complex<double>(1, 0);
        MNA_mat(branch_index, node_2_index) += complex<double>(-1, 0);
        MNA_mat(branch_index, branch_index) += 0;
        MNA_mat(node_1_index, branch_index) += complex<double>(1, 0);
        MNA_mat(node_2_index, branch_index) += complex<double>(-1, 0);
        RHS(branch_index, 0) += complex<double>(value, 0);
    }

    // Add VCVS
    for (VCVS vcvs : vcvs_vec) {
        int node_1_index = FindNode(modified_node_vec, vcvs.node_1);
        int node_2_index = FindNode(modified_node_vec, vcvs.node_2);
        int ctrl_node_1_index = FindNode(modified_node_vec, vcvs.ctrl_node_1);
        int ctrl_node_2_index = FindNode(modified_node_vec, vcvs.ctrl_node_2);
        double value = vcvs.value;
        int branch_index = FindNode(modified_node_vec, "i_" + vcvs.name);
        MNA_mat(branch_index, node_1_index) += complex<double>(1, 0);
        MNA_mat(branch_index, node_2_index) += complex<double>(-1, 0);
        MNA_mat(branch_index, ctrl_node_1_index) += complex<double>(-1 * value, 0);
        MNA_mat(branch_index, ctrl_node_2_index) += complex<double>(value, 0);
        MNA_mat(node_1_index, branch_index) += complex<double>(1, 0);
        MNA_mat(node_2_index, branch_index) += complex<double>(-1, 0);
    }

    AnalysisMatrix result_mat = {MNA_mat, modified_node_vec, RHS};
    return result_mat;
}

int Analyzer::FindNode(vector<NodeName> node_vec, NodeName name) {
    for (std::size_t i = 0; i < node_vec.size(); i++) {
        if (node_vec[i] == name) {
            return i;
        }
    }
    cout << "Not found: " << name << endl;
    return -1;
}

void Analyzer::PrintMatrix(cx_mat mat, vector<NodeName> nodes) {
    cout << "Matrix: " << endl << ' ';
    for (auto node : nodes) {
        cout << setw(18) << node << "\t";
    }
    cout << endl;
    for (uint r = 0; r < mat.n_rows; r++) {
        cout << setw(4) << nodes[r] << "\t";
        if (r == 0)
            cout << "╭";
        else if (r == mat.n_rows - 1)
            cout << "╰";
        else
            cout << "│";
        for (uint c = 0; c < mat.n_cols; c++) {
            cout << " " << setw(7) << std::real(mat(r, c)) << " + "
                 << "j"
                 << "(" << setw(7) << std::imag(mat(r, c)) << ")"
                 << "\t";
        }
        if (r == 0)
            cout << "╮";
        else if (r == mat.n_rows - 1)
            cout << "╯";
        else
            cout << "│";
        cout << endl;
    }
    cout << endl;
}

void Analyzer::PrintRHS(cx_mat rhs, vector<NodeName> nodes) {
    cout << "RHS: " << endl;
    // cout << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    for (uint i = 0; i < rhs.n_rows; i++) {
        cout << setw(4) << nodes[i] << "\t";
        if (i == 0)
            cout << "╭";
        else if (i == rhs.n_rows - 1)
            cout << "╰";
        else
            cout << "│";

        cout << setw(8) << std::real(rhs(i, 0)) << " ";

        if (i == 0)
            cout << "╮" << endl;
        else if (i == rhs.n_rows - 1)
            cout << "╯" << endl;
        else
            cout << "│" << endl;
    }
}