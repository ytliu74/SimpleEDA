/**
 * @file tran_analyzer.cpp
 * @author Yaotian Liu
 * @brief Implementation of tran analysis
 * @date 2022-10-23
 */

#include "analyzer.h"

using arma::mat;
using arma::span;
using arma::vec;
using std::cout;
using std::endl;

TranAnalysisMat BackEuler(const Circuit circuit, const double h);
TranAnalysisMat TrapezoidalRule(const Circuit circuit, const double h);

double GetVsrcValue(const Vsrc vsrc, double t);
double GetPulseValue(const Pulse pulse, double t);
double GetSinValue(const Sin sin, double t);

// TODO: only support RCL.
void Analyzer::DoTranAnalysis(const TranAnalysis tran_analysis) {
    double t_start = tran_analysis.t_start;
    double t_stop = tran_analysis.t_stop;
    double t_step = tran_analysis.t_step;
    int scan_num = (t_stop - t_start) / t_step;

    TranAnalysisMat tran_analysis_mat = BackEuler(circuit, t_step);

    int total_node_num = tran_analysis_mat.node_vec.size();

    // Remove the ground node
    mat MNA =
        tran_analysis_mat.MNA(span(1, total_node_num - 1), span(1, total_node_num - 1));

    std::vector<NodeName> MNA_node_vec = tran_analysis_mat.node_vec;
    std::vector<NodeName>::iterator h = MNA_node_vec.begin();
    MNA_node_vec.erase(h);

    mat RHS_gen = tran_analysis_mat.RHS_gen(span(1, total_node_num - 1),
                                            span(1, total_node_num - 1));

    mat tran_result_mat(total_node_num - 1, scan_num + 1, arma::fill::zeros);
    std::vector<double> time_point_vec;

    time_point_vec.push_back(t_start);

    // cout << "MNA: " << endl << MNA << endl;
    // cout << "RHS_gen: " << endl << RHS_gen << endl;

    for (int i = 0; i < scan_num; i++) {
        time_point_vec.push_back(t_start + (i + 1) * t_step);

        mat RHS_t_h = RHS_gen * tran_result_mat.col(i);

        // voltage source up
        for (auto vsrc : circuit.vsrc_vec) {
            int index = FindNode(MNA_node_vec, "i_" + vsrc.name);
            double value = GetVsrcValue(vsrc, t_start + (i + 1) * t_step);
            RHS_t_h(index, 0) = value;
        }

        // Source source up
        for (auto isrc : circuit.isrc_vec) {
            int node_1_index = FindNode(MNA_node_vec, isrc.node_1);
            int node_2_index = FindNode(MNA_node_vec, isrc.node_2);
            if (node_1_index >= 0)
                RHS_t_h(node_1_index, 0) += -1 * isrc.tran_const_value;
            if (node_2_index >= 0)
                RHS_t_h(node_2_index, 0) += 1 * isrc.tran_const_value;
        }

        vec tran_result;

        if (!circuit.diode_vec.empty()) {
            // Nonlinear
            vec result_n(total_node_num - 1, arma::fill::zeros);
            vec result_n_plus_1(total_node_num - 1, arma::fill::zeros);

            while (true) {
                // Update the analysis matrix
                mat mna = AddExpTerm(tran_analysis_mat.exp_analysis_vec, result_n, MNA);
                // Update RHS
                mat rhs = AddExpTerm(tran_analysis_mat.exp_rhs_vec, result_n, RHS_t_h);
                // cout << "reduced_mat:" << endl << MNA << endl;
                // cout << "scan_rhs" << endl << RHS_t_h << endl;

                result_n_plus_1 = arma::solve(mna, rhs, arma::solve_opts::allow_ugly);
                if (VecDifference(result_n, result_n_plus_1))
                    break;
                result_n = result_n_plus_1;
            }
            tran_result = result_n_plus_1;
        }
        // Linear
        else {
            tran_result = arma::solve(MNA, RHS_t_h);
        }

        tran_result_mat.col(i + 1) = tran_result;
    }
    tran_result = TranResult{tran_result_mat, time_point_vec, MNA_node_vec};
}

TranAnalysisMat BackEuler(const Circuit circuit, const double h) {
    // ----- Generate NA metrix -----
    int node_num = circuit.node_vec.size();
    mat NA(node_num, node_num, arma::fill::zeros);

    mat RHS_gen(node_num, node_num, arma::fill::zeros);

    for (Res res : circuit.res_vec) {
        int node_1_index = FindNode(circuit.node_vec, res.node_1);
        int node_2_index = FindNode(circuit.node_vec, res.node_2);
        double conductance = 1 / res.value;
        NA(node_1_index, node_1_index) += conductance;
        NA(node_1_index, node_2_index) += -1 * conductance;
        NA(node_2_index, node_1_index) += -1 * conductance;
        NA(node_2_index, node_2_index) += conductance;
    }

    // ----- Generate MNA metrix -----
    std::vector<NodeName> modified_node_vec = circuit.node_vec;

    // Every inducter contributes to one more branch node
    for (Ind ind : circuit.ind_vec)
        modified_node_vec.push_back("i_" + ind.name);

    // Every capatitor contributes to one more branch node
    for (Cap cap : circuit.cap_vec)
        modified_node_vec.push_back("i_" + cap.name);

    // Every voltage source contributes to one more branch node
    for (Vsrc vsrc : circuit.vsrc_vec)
        modified_node_vec.push_back("i_" + vsrc.name);

    // Initialize MNA metrix
    int modified_node_num = modified_node_vec.size();
    mat MNA = NA;
    MNA.resize(modified_node_num, modified_node_num);

    // Update RHS
    RHS_gen.resize(modified_node_num, modified_node_num);

    // Add inductor stamps
    for (Ind ind : circuit.ind_vec) {
        int node_1_index = FindNode(modified_node_vec, ind.node_1);
        int node_2_index = FindNode(modified_node_vec, ind.node_2);
        double value = ind.value;
        int branch_index = FindNode(modified_node_vec, "i_" + ind.name);
        MNA(branch_index, node_1_index) += 1;
        MNA(branch_index, node_2_index) += -1;
        MNA(branch_index, branch_index) += -1 * value / h;
        MNA(node_1_index, branch_index) += 1;
        MNA(node_2_index, branch_index) += -1;
        RHS_gen(branch_index, branch_index) += -1 * value / h;
    }

    // Add capacitor stamps
    for (Cap cap : circuit.cap_vec) {
        int node_1_index = FindNode(modified_node_vec, cap.node_1);
        int node_2_index = FindNode(modified_node_vec, cap.node_2);
        double value = cap.value;
        int branch_index = FindNode(modified_node_vec, "i_" + cap.name);
        MNA(branch_index, node_1_index) += value / h;
        MNA(branch_index, node_2_index) += -1 * value / h;
        MNA(branch_index, branch_index) += -1;
        MNA(node_1_index, branch_index) += 1;
        MNA(node_2_index, branch_index) += -1;
        RHS_gen(branch_index, node_1_index) += value / h;
        RHS_gen(branch_index, node_2_index) += -1 * value / h;
    }

    // Add voltage source stamps
    for (Vsrc vsrc : circuit.vsrc_vec) {
        int node_1_index = FindNode(modified_node_vec, vsrc.node_1);
        int node_2_index = FindNode(modified_node_vec, vsrc.node_2);
        // double value = vsrc.value;
        int branch_index = FindNode(modified_node_vec, "i_" + vsrc.name);
        MNA(branch_index, node_1_index) += 1;
        MNA(branch_index, node_2_index) += -1;
        MNA(node_1_index, branch_index) += 1;
        MNA(node_2_index, branch_index) += -1;
        // RHS_gen(branch_index, node_1_index) += 1;
        // RHS_gen(branch_index, node_2_index) += -1;
    }

    // Add Diode stamps
    std::vector<ExpTerm> exp_analysis_vec;
    std::vector<ExpTerm> exp_rhs_vec;
    for (Diode diode : circuit.diode_vec) {
        int node_1_index = FindNode(circuit.node_vec, diode.node_1);
        int node_2_index = FindNode(circuit.node_vec, diode.node_2);
        exp_analysis_vec.push_back(ExpTerm(node_1_index, node_1_index, node_1_index,
                                           node_2_index, ExpCoeff(40, 40)));
        exp_analysis_vec.push_back(ExpTerm(node_1_index, node_2_index, node_1_index,
                                           node_2_index, ExpCoeff(-40, 40)));
        exp_analysis_vec.push_back(ExpTerm(node_2_index, node_1_index, node_1_index,
                                           node_2_index, ExpCoeff(-40, 40)));
        exp_analysis_vec.push_back(ExpTerm(node_2_index, node_2_index, node_1_index,
                                           node_2_index, ExpCoeff(40, 40)));

        exp_rhs_vec.push_back(ExpTerm(node_1_index, node_1_index, node_2_index,
                                      ExpCoeff(-1, 40, 1), ExpCoeff(40, 40)));
        exp_rhs_vec.push_back(ExpTerm(node_2_index, node_1_index, node_2_index,
                                      ExpCoeff(1, 40, -1), ExpCoeff(-40, 40)));
    }

    TranAnalysisMat tran_analysis_mat(MNA, exp_analysis_vec, modified_node_vec, RHS_gen,
                                      exp_rhs_vec);

    return tran_analysis_mat;
}

// TODO
// TranAnalysisMat TrapezoidalRule(Circuit circuit, double h) {}

double GetVsrcValue(const Vsrc vsrc, double t) {
    if (vsrc.pulse.chosen)
        return GetPulseValue(vsrc.pulse, t);
    else if (vsrc.sin.chosen)
        return GetSinValue(vsrc.sin, t);
    else
        return vsrc.value;
}

double GetPulseValue(const Pulse pulse, double t) {
    double value;

    double k_r = (pulse.v2 - pulse.v1) / pulse.tr;
    double k_f = (pulse.v1 - pulse.v2) / pulse.tf;

    if (t <= pulse.td)
        value = pulse.v1;
    else {
        t = (t - pulse.td) - floor((t - pulse.td) / pulse.per) * pulse.per;
        // 0 < t <= PER
        if (t < pulse.tr)
            value = pulse.v1 + k_r * t;
        else if (t < pulse.tr + pulse.pw)
            value = pulse.v2;
        else if (t < pulse.tr + pulse.pw + pulse.tf)
            value = pulse.v2 + k_f * (t - pulse.tr - pulse.pw);
        else
            value = pulse.v1;
    }

    return value;
}

double GetSinValue(const Sin sin, double t) {
    double value;

    if (t <= sin.td)
        value = sin.v0;
    else
        value = sin.v0 + sin.va * exp(-1 * (t - sin.td) * sin.theta) *
                             std::sin(2 * M_PI * sin.freq * (t - sin.td));

    return value;
}