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

struct ExpCoeff {
    std::complex<double> exp;
    double constant;

    ExpCoeff() {}
    ExpCoeff(double a, double b) : exp(a, b), constant(0) {}
    ExpCoeff(double a, double b, double c) : exp(a, b), constant(c) {}
};

struct ExpTerm {
    // The position of the ExpTerm
    int row_index;
    int col_index;
    // x = V(node_1) - V(node_2)
    int node_1_index;
    int node_2_index;
    ExpCoeff zero_order;   // a*e^{bx}+c -> (a, b, c)
    ExpCoeff first_order;  // (a*e^{bx}+c)*x -> (a, b, c)

    ExpTerm() {}

    // For analysis mat
    ExpTerm(int row_index, int col_index, int node_1_index, int node_2_index,
            ExpCoeff zero_order, ExpCoeff first_order)
        : row_index(row_index - 1),
          col_index(col_index - 1),
          node_1_index(node_1_index - 1),
          node_2_index(node_2_index - 1),
          zero_order(zero_order),
          first_order(first_order) {}

    ExpTerm(int row_index, int col_index, int node_1_index, int node_2_index,
            ExpCoeff zero_order)
        : row_index(row_index - 1),
          col_index(col_index - 1),
          node_1_index(node_1_index - 1),
          node_2_index(node_2_index - 1),
          zero_order(zero_order) {}

    // For RHS
    ExpTerm(int row_index, int node_1_index, int node_2_index, ExpCoeff zero_order,
            ExpCoeff first_order)
        : row_index(row_index - 1),
          col_index(0),
          node_1_index(node_1_index - 1),
          node_2_index(node_2_index - 1),
          zero_order(zero_order),
          first_order(first_order) {}

    ExpTerm(int row_index, int node_1_index, int node_2_index, ExpCoeff zero_order)
        : row_index(row_index - 1),
          col_index(0),
          node_1_index(node_1_index - 1),
          node_2_index(node_2_index - 1),
          zero_order(zero_order) {}
};

struct AnalysisMatrix {
    arma::cx_mat linear_analysis_mat;
    std::vector<ExpTerm> exp_analysis_vec;
    std::vector<NodeName> node_vec;
    arma::cx_mat rhs;
    std::vector<ExpTerm> exp_rhs_vec;

    AnalysisMatrix() {}
    AnalysisMatrix(arma::cx_mat linear_analysis_mat, std::vector<NodeName> node_vec,
                   arma::cx_mat rhs)
        : linear_analysis_mat(linear_analysis_mat), node_vec(node_vec), rhs(rhs) {}

    AnalysisMatrix(arma::cx_mat linear_analysis_mat,
                   std::vector<ExpTerm> exp_analysis_vec, std::vector<NodeName> node_vec,
                   arma::cx_mat rhs, std::vector<ExpTerm> exp_rhs_vec)
        : linear_analysis_mat(linear_analysis_mat),
          exp_analysis_vec(exp_analysis_vec),
          node_vec(node_vec),
          rhs(rhs),
          exp_rhs_vec(exp_rhs_vec) {}
};

struct TranResult {
    arma::mat tran_result_mat;
    std::vector<double> time_point_vec;
    std::vector<NodeName> node_vec;
};

struct TranAnalysisMat {
    arma::mat MNA;
    std::vector<ExpTerm> exp_analysis_vec;
    std::vector<NodeName> node_vec;
    arma::mat RHS_gen;
    std::vector<ExpTerm> exp_rhs_vec;

    TranAnalysisMat() {}
    TranAnalysisMat(arma::mat MNA, std::vector<ExpTerm> exp_analysis_vec,
                    std::vector<NodeName> node_vec, arma::mat RHS_gen,
                    std::vector<ExpTerm> exp_rhs_vec)
        : MNA(MNA),
          exp_analysis_vec(exp_analysis_vec),
          node_vec(node_vec),
          RHS_gen(RHS_gen),
          exp_rhs_vec(exp_rhs_vec) {}
};

struct DcResult {
    std::vector<arma::vec> dc_result_vec;
    std::vector<double> dc_value_vec;
    std::vector<NodeName> node_vec;
};

struct AcResult {
    std::vector<arma::cx_vec> ac_result_vec;
    std::vector<double> freq_vec;
    std::vector<NodeName> node_vec;
};

#endif  // ANALYZER_TYPE_H