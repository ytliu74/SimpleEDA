/**
 * @file Analyzer.cpp
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-15
 */

#include "Analyzer.h"

Analyzer::Analyzer(Parser parser) {
    Vsrc_vec = parser.getVsrc();
    Res_vec = parser.getResistor();
    Cap_vec = parser.getCapacitor();
    Ind_vec = parser.getInductor();
    Node_vec = parser.getNode();

    updateMatrix();
}

void Analyzer::updateMatrix() {
    createNA();
    createMNA();
}

int Analyzer::findNode(NodeName name) {
    for (std::size_t i = 0; i < Node_vec.size(); i++) {
        if (Node_vec[i] == name)
            return i;
    }
    return -1;
}

// TODO: implement other devices
void Analyzer::createNA() {
    int nodeNum = Node_vec.size();
    NA_mat = arma::mat(nodeNum, nodeNum, arma::fill::zeros);

    // Add resistor stamps
    for (Res res : Res_vec) {
        int node_1_index = findNode(res.node_1);
        int node_2_index = findNode(res.node_2);
        double conductance = 1 / res.value;
        NA_mat(node_1_index, node_1_index) += conductance;
        NA_mat(node_1_index, node_2_index) += (-1 * conductance);
        NA_mat(node_2_index, node_1_index) += (-1 * conductance);
        NA_mat(node_2_index, node_2_index) += conductance;
    }
}

void Analyzer::createMNA() {}
