/**
 * @file Analyzer.h
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-15
 */

#if !defined(ANALYZER_H)
#define ANALYZER_H

#include <armadillo>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

#include "parser/parser.h"
#include "utils/utils.h"

struct AnalysisMatrix {
    arma::cx_mat node_metrix;
    std::vector<NodeName> node_vec;
    arma::cx_mat rhs;
};

class Analyzer {
  public:
    Analyzer() {}
    Analyzer(Parser parser);
    ~Analyzer() {}

    std::vector<AnalysisMatrix> GetAnalysisResults() { return analysis_matrix_vec; }

    void PrintMatrix(arma::cx_mat mat, std::vector<NodeName> nodes);
    void PrintRHS(arma::cx_mat rhs, std::vector<NodeName> nodes);

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

    std::vector<AnalysisMatrix> analysis_matrix_vec;

    void DoDcAnalysis(const DcAnalysis dc_analysis);
    void DoAcAnalysis(const AcAnalysis ac_analysis);

    AnalysisMatrix GetAnalysisMatrix(const double frequency);

    int FindNode(std::vector<NodeName> node_vec, NodeName name);
};

#endif  // ANALYZER_H
