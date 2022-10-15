/**
 * @file Analyzer.h
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-15
 */

#if !defined(ANALYZER_H)
#define ANALYZER_H

#include <Parser/Parser.h>

#include <armadillo>
#include <iostream>
#include <vector>

class Analyzer {
  public:
    Analyzer(Parser parser);
    ~Analyzer() {}

    arma::mat getNA() { return NA_mat; }
    arma::mat getMNA() { return MNA_mat; }

  private:
    std::vector<Vsrc> Vsrc_vec;
    std::vector<Res> Res_vec;
    std::vector<Cap> Cap_vec;
    std::vector<Ind> Ind_vec;
    std::vector<NodeName> Node_vec;

    arma::mat NA_mat;
    arma::mat MNA_mat;

    void updateMatrix();

    void createNA();
    void createMNA();
};

#endif  // ANALYZER_H
