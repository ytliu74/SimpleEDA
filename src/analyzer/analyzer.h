/**
 * @file Analyzer.h
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-15
 */

#if !defined(ANALYZER_H)
#define ANALYZER_H

#include <armadillo>
#include <iomanip>
#include <iostream>
#include <vector>

#include "parser/parser.h"
#include "utils/utils.h"

class Analyzer {
  public:
    Analyzer() {}
    Analyzer(Parser parser);
    ~Analyzer() {}

    arma::cx_mat GetNA() { return NA_mat; }
    arma::cx_mat GetMNA() { return MNA_mat; }

    std::vector<NodeName> GetNodes() { return node_vec; }
    std::vector<NodeName> GetModifiedNodes() { return modified_node_vec; }

    void PrintMatrix(arma::cx_mat mat, std::vector<NodeName> nodes);
    void PrintRHS();

  private:
    std::vector<Vsrc> vsrc_vec;
    std::vector<Isrc> isrc_vec;
    std::vector<VCCS> vccs_vec;
    std::vector<VCVS> vcvs_vec;
    std::vector<Res> res_vec;
    std::vector<Cap> cap_vec;
    std::vector<Ind> ind_vec;
    std::vector<NodeName> node_vec;
    std::vector<NodeName> modified_node_vec;

    arma::cx_mat NA_mat;
    arma::cx_mat MNA_mat;

    arma::cx_mat RHS;

    void UpdateMatrix();

    void CreateNA();
    void CreateMNA();

    int FindNode(std::vector<NodeName> node_vec, NodeName name);
};

#endif  // ANALYZER_H
