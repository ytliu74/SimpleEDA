/**
 * @file analyzer_type.h
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-26
 */

#ifndef ANALYZER_TYPE_H
#define ANALYZER_TYPE_H

#include <armadillo>
#include <iostream>
#include <vector>

#include "../parser/parser.h"

struct AnalysisMatrix {
    arma::cx_mat analysis_mat;
    std::vector<NodeName> node_vec;
    arma::cx_mat rhs;
};

struct TranResult {
    std::vector<arma::vec> tran_result_vec;
    std::vector<double> time_point_vec;
    std::vector<NodeName> node_vec;
};

struct TranAnalysisMat {
    arma::mat MNA;
    std::vector<NodeName> node_vec;
    arma::mat RHS_gen;
};

struct DcResult {
    std::vector<arma::vec> dc_result_vec;
    std::vector<double> dc_value_vec;
    std::vector<NodeName> node_vec;
};

struct AcResult {
    std::vector<arma::cx_vec> ac_result_vec;
    std::vector<double> ac_value_vec;
    std::vector<NodeName> node_vec;
};

#endif  // ANALYZER_TYPE_H