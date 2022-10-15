/**
 * @file Analyzer.cpp
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-15
 */

#include "analyzer.h"

Analyzer::Analyzer(Parser parser) {
    vsrc_vec = parser.GetVsrc();
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

int Analyzer::FindNode(NodeName name) {
    for (std::size_t i = 0; i < node_vec.size(); i++) {
        if (node_vec[i] == name)
            return i;
    }
    return -1;
}

// TODO: implement other devices
void Analyzer::CreateNA() {
    int node_num = node_vec.size();
    NA_mat = arma::mat(node_num, node_num, arma::fill::zeros);

    // Add resistor stamps
    for (Res res : res_vec) {
        int node_1_index = FindNode(res.node_1);
        int node_2_index = FindNode(res.node_2);
        double conductance = 1 / res.value;
        NA_mat(node_1_index, node_1_index) += conductance;
        NA_mat(node_1_index, node_2_index) += (-1 * conductance);
        NA_mat(node_2_index, node_1_index) += (-1 * conductance);
        NA_mat(node_2_index, node_2_index) += conductance;
    }
}

void Analyzer::CreateMNA() {}
