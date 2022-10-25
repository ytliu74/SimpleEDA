/**
 * @file tran_analyzer.cpp
 * @author Yaotian Liu
 * @brief Implementation of tran analysis
 * @date 2022-10-23
 */

#include "analyzer.h"

using arma::mat;
using arma::vec;

TranAnalysisMat BackEuler(Circuit circuit, double h);
TranAnalysisMat TrapezoidalRule(Circuit circuit, double h);

// TODO: only support RCL.
void Analyzer::DoTranAnalysis(const TranAnalysis tran_analysis) {
    double t_start = tran_analysis.t_start;
    double t_stop = tran_analysis.t_stop;
    double t_step = tran_analysis.t_step;
    int scan_num = (t_stop - t_start) / t_step;

    std::vector<vec> tran_result_vec;
    std::vector<double> time_point_vec;

    TranAnalysisMat tran_analysis_mat = BackEuler(circuit, t_step);

    std::cout << "MNA: " << std::endl << tran_analysis_mat.MNA << std::endl;
    std::cout << "RHS: " << std::endl << tran_analysis_mat.RHS_gen << std::endl;

    // for ()
}

TranAnalysisMat BackEuler(Circuit circuit, double h) {
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

TranAnalysisMat TrapezoidalRule(Circuit circuit, double h) {}
