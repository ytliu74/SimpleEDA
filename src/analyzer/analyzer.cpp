/**
 * @file Analyzer.cpp
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-15
 */

#include "analyzer.h"

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

    UpdateMatrix();
}

void Analyzer::UpdateMatrix() {
    CreateNA();
    CreateMNA();
}

int Analyzer::FindNode(vector<NodeName> node_vec, NodeName name) {
    for (std::size_t i = 0; i < node_vec.size(); i++) {
        if (node_vec[i] == name)
            return i;
    }
    cout << "Not found: " << name << endl;
    return -1;
}

void Analyzer::PrintMatrix(arma::cx_mat mat, vector<NodeName> nodes) {
    cout << std::setiosflags(std::ios::fixed) << std::setprecision(4);
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
                 << "s"
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
}

void Analyzer::PrintRHS() {
    cout << "RHS: " << endl;
    // cout << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    for (uint i = 0; i < RHS.n_rows; i++) {
        cout << setw(4) << modified_node_vec[i] << "\t";
        if (i == 0)
            cout << "╭";
        else if (i == RHS.n_rows - 1)
            cout << "╰";
        else
            cout << "│";

        cout << setw(10) << std::real(RHS(i, 0)) << " ";

        if (i == 0)
            cout << "╮" << endl;
        else if (i == RHS.n_rows - 1)
            cout << "╯" << endl;
        else
            cout << "│" << endl;
    }
}

// TODO: implement other devices
void Analyzer::CreateNA() {
    int node_num = node_vec.size();
    NA_mat = arma::cx_mat(node_num, node_num, arma::fill::zeros);

    RHS = arma::cx_mat(node_num, 1, arma::fill::zeros);

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
        double value = cap.value;
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

    // TODO: Add VCVS
}

void Analyzer::CreateMNA() {
    int extra_node_num = 0;
    modified_node_vec = node_vec;  // Copy the original node vec first.

    // Every inducter contributes to one more branch node
    extra_node_num += ind_vec.size();
    for (Ind ind : ind_vec) {
        NodeName ind_i = "i_" + ind.name;
        modified_node_vec.push_back(ind_i);
    }

    // Every voltage source contributes to one more branch node
    extra_node_num += vsrc_vec.size();
    for (Vsrc vsrc : vsrc_vec)
        modified_node_vec.push_back("i_" + vsrc.name);

    // Initialize MNA metrix
    int modified_node_num = modified_node_vec.size();
    MNA_mat = NA_mat;
    MNA_mat.resize(modified_node_num, modified_node_num);

    // Update RHS
    RHS.resize(modified_node_num, 1);

    // Add inductor stamps
    for (Ind ind : ind_vec) {
        int node_1_index = FindNode(modified_node_vec, ind.node_1);
        int node_2_index = FindNode(modified_node_vec, ind.node_2);
        double value = ind.value;
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
}
