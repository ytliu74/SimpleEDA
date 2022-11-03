/**
 * @file acdc_analyzer.cpp
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-15
 */

#include "analyzer.h"

using arma::cx_mat;
using arma::cx_vec;
using arma::mat;
using arma::span;
using arma::vec;
using std::complex;
using std::cout;
using std::endl;
using std::setw;
using std::vector;

void Analyzer::DoDcAnalysis(const DcAnalysis dc_analysis) {
    double frequency = 0;
    AnalysisMatrix analysis_matrix = GetAnalysisMatrix(frequency);

    double start = dc_analysis.start;
    double end = dc_analysis.end;
    double step = dc_analysis.step;

    int node_num = analysis_matrix.node_vec.size();

    // `reduced` means remove the 0(gnd) node.
    mat reduced_mat = GetReal(analysis_matrix.linear_analysis_mat(span(1, node_num - 1),
                                                                  span(1, node_num - 1)));

    std::vector<NodeName> reduced_node_vec = analysis_matrix.node_vec;
    std::vector<NodeName>::iterator h = reduced_node_vec.begin();
    reduced_node_vec.erase(h);

    mat reduced_rhs = GetReal(analysis_matrix.rhs(span(1, node_num - 1), 0));

    std::vector<vec> dc_result_vec;
    std::vector<double> dc_value_vec;

    int scan_vsrc_index = FindNode(reduced_node_vec, "i_" + dc_analysis.Vsrc_name);
    for (double v = start; v <= end + 1e-4; v += step) {
        mat scan_rhs = reduced_rhs;
        scan_rhs(scan_vsrc_index, 0) = v;

        vec dc_result = arma::solve(reduced_mat, scan_rhs);

        // cout << "result: " << endl << dc_result << endl;

        dc_result_vec.push_back(dc_result);
        dc_value_vec.push_back(v);
    }

    dc_result = DcResult{dc_result_vec, dc_value_vec, reduced_node_vec};
}

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

    vector<arma::cx_vec> ac_result_vec;

    for (auto f : scan_freq_vec) {
        AnalysisMatrix analysis_matrix = GetAnalysisMatrix(f);
        int node_num = analysis_matrix.node_vec.size();

        // `reduced` means remove the 0(gnd) node.
        cx_mat reduced_mat = analysis_matrix.linear_analysis_mat(span(1, node_num - 1),
                                                                 span(1, node_num - 1));

        std::vector<NodeName> reduced_node_vec = analysis_matrix.node_vec;
        std::vector<NodeName>::iterator h = reduced_node_vec.begin();
        reduced_node_vec.erase(h);

        cx_mat reduced_rhs = analysis_matrix.rhs(span(1, node_num - 1), 0);

        cx_vec ac_result = arma::solve(reduced_mat, reduced_rhs);

        ac_result_vec.push_back(ac_result);
    }

    AnalysisMatrix analysis_matrix = GetAnalysisMatrix(0);
    std::vector<NodeName> reduced_node_vec = analysis_matrix.node_vec;
    std::vector<NodeName>::iterator h = reduced_node_vec.begin();
    reduced_node_vec.erase(h);

    ac_result = {ac_result_vec, scan_freq_vec, reduced_node_vec};
}

AnalysisMatrix Analyzer::GetAnalysisMatrix(const double frequency) {
    const double w = M_2_PI * frequency;  // w = 2 pi f

    // ----- Generate NA metrix -----
    int node_num = circuit.node_vec.size();
    cx_mat NA_mat(node_num, node_num, arma::fill::zeros);

    cx_mat RHS(node_num, 1, arma::fill::zeros);

    // Add resistor stamps
    for (Res res : circuit.res_vec) {
        int node_1_index = FindNode(circuit.node_vec, res.node_1);
        int node_2_index = FindNode(circuit.node_vec, res.node_2);
        double conductance = 1 / res.value;
        NA_mat(node_1_index, node_1_index) += complex<double>(conductance, 0);
        NA_mat(node_1_index, node_2_index) += complex<double>(-1 * conductance, 0);
        NA_mat(node_2_index, node_1_index) += complex<double>(-1 * conductance, 0);
        NA_mat(node_2_index, node_2_index) += complex<double>(conductance, 0);
    }

    // Add capacitor stamps
    for (Cap cap : circuit.cap_vec) {
        int node_1_index = FindNode(circuit.node_vec, cap.node_1);
        int node_2_index = FindNode(circuit.node_vec, cap.node_2);
        double value = cap.value * w;
        NA_mat(node_1_index, node_1_index) += complex<double>(0, value);
        NA_mat(node_1_index, node_2_index) += complex<double>(0, -1 * value);
        NA_mat(node_2_index, node_1_index) += complex<double>(0, -1 * value);
        NA_mat(node_2_index, node_2_index) += complex<double>(0, value);
    }

    // Add Current Source
    for (Isrc isrc : circuit.isrc_vec) {
        int node_1_index = FindNode(circuit.node_vec, isrc.node_1);
        int node_2_index = FindNode(circuit.node_vec, isrc.node_2);
        double value = isrc.value;
        // The current run from node_1 to node_2,
        // thus on the LHS, LHS(node_1) = -Ik => RHS(node_1) = +Ik.
        // Same for node_2.
        RHS(node_1_index, 0) += complex<double>(value, 0);
        RHS(node_2_index, 0) += complex<double>(-value, 0);
    }

    // Add VCCS
    for (VCCS vccs : circuit.vccs_vec) {
        int node_1_index = FindNode(circuit.node_vec, vccs.node_1);
        int node_2_index = FindNode(circuit.node_vec, vccs.node_2);
        int ctrl_node_1_index = FindNode(circuit.node_vec, vccs.ctrl_node_1);
        int ctrl_node_2_index = FindNode(circuit.node_vec, vccs.ctrl_node_2);
        double value = vccs.value;
        NA_mat(node_1_index, ctrl_node_1_index) += complex<double>(value, 0);
        NA_mat(node_1_index, ctrl_node_2_index) += complex<double>(-1 * value, 0);
        NA_mat(node_2_index, ctrl_node_1_index) += complex<double>(-1 * value, 0);
        NA_mat(node_2_index, ctrl_node_2_index) += complex<double>(value, 0);
    }

    // ----- Generate MNA metrix -----
    modified_node_vec = circuit.node_vec;

    // Every inducter contributes to one more branch node
    for (Ind ind : circuit.ind_vec)
        modified_node_vec.push_back("i_" + ind.name);

    // Every voltage source contributes to one more branch node
    for (Vsrc vsrc : circuit.vsrc_vec)
        modified_node_vec.push_back("i_" + vsrc.name);

    // Every VCVS contributes to one more branch node
    for (VCVS vcvs : circuit.vcvs_vec)
        modified_node_vec.push_back("i_" + vcvs.name);

    // Initialize MNA metrix
    int modified_node_num = modified_node_vec.size();
    cx_mat MNA_mat = NA_mat;
    MNA_mat.resize(modified_node_num, modified_node_num);

    // Update RHS
    RHS.resize(modified_node_num, 1);

    // Add inductor stamps
    for (Ind ind : circuit.ind_vec) {
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
    for (Vsrc vsrc : circuit.vsrc_vec) {
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
    for (VCVS vcvs : circuit.vcvs_vec) {
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

    AnalysisMatrix result_mat(MNA_mat, modified_node_vec, RHS);
    return result_mat;
}
