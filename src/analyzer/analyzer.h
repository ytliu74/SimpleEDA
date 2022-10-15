/**
 * @file Analyzer.h
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-15
 */

#if !defined(ANALYZER_H)
#define ANALYZER_H

#include <armadillo>
#include <iostream>
#include <vector>

#include "parser/parser.h"
#include "utils/utils.h"

class Analyzer {
  public:
    Analyzer(Parser parser);
    ~Analyzer() {}

    arma::mat GetNA() { return NA_mat; }
    arma::mat GetMNA() { return MNA_mat; }

  private:
    std::vector<Vsrc> vsrc_vec;
    std::vector<Res> res_vec;
    std::vector<Cap> cap_vec;
    std::vector<Ind> ind_vec;
    std::vector<NodeName> node_vec;

    arma::mat NA_mat;
    arma::mat MNA_mat;

    void UpdateMatrix();

    void CreateNA();
    void CreateMNA();

    int FindNode(NodeName name);
};

#endif  // ANALYZER_H
