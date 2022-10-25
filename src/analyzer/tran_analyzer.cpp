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

    cout << "MNA: " << endl << MNA << endl;
    cout << "RHS_gen: " << endl << RHS_gen << endl;

    for (int i = 0; i < scan_num; i++) {
        time_point_vec.push_back(t_start + (i + 1) * t_step);

        // voltage source up
        for (auto vsrc : circuit.vsrc_vec) {
            int node_1_index = FindNode(MNA_node_vec, vsrc.node_1);
            double value = vsrc.value;
            tran_result_mat(node_1_index, i) = value;
        }

        cout << "set voltage source up" << endl;

        mat RHS_t_h = RHS_gen * tran_result_mat.col(i);
        cout << "RHS_t_h: " << endl << RHS_t_h << endl;
        vec tran_result = arma::solve(MNA, RHS_t_h);
        tran_result_mat.col(i + 1) = tran_result;
        cout << tran_result << endl;
    }
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
        RHS_gen(branch_index, node_1_index) += 1;
        RHS_gen(branch_index, node_2_index) += -1;
    }

    TranAnalysisMat tran_analysis_mat = {MNA, modified_node_vec, RHS_gen};
    return tran_analysis_mat;
}

// TODO
TranAnalysisMat TrapezoidalRule(Circuit circuit, double h) {}
