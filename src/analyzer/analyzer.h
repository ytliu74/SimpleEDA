/**
 * @file analyzer.h
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

#include "../parser/parser.h"
#include "../utils/utils.h"
#include "analyzer_type.h"
#include "qcustomplot.h"

int FindNode(std::vector<NodeName> node_vec, NodeName name);

void DcPlot(DcResult result, PrintVariable print_variable);
void AcPlot(AcResult result, PrintVariable print_variable);
void TranPlot(TranResult result, PrintVariable print_variable);
void Plot(QVector<double> x, QVector<double> y, QString x_label, QString y_label,
          bool x_log, bool y_log);

void AddExpTerm(const std::vector<ExpTerm> exp_term_vec, const arma::vec result,
                arma::mat& mat);

class Analyzer {
  public:
    Analyzer() {}
    Analyzer(Parser parser);
    ~Analyzer() {}

    std::vector<AnalysisMatrix> GetAnalysisResults() { return analysis_matrix_vec; }

    void PrintMatrix(arma::cx_mat mat, std::vector<NodeName> nodes);
    void PrintRHS(arma::cx_mat rhs, std::vector<NodeName> nodes);

  private:
    Circuit circuit;
    std::vector<NodeName> modified_node_vec;

    std::vector<AnalysisMatrix> analysis_matrix_vec;

    TranResult tran_result;
    DcResult dc_result;
    AcResult ac_result;

    void DoDcAnalysis(const DcAnalysis dc_analysis);
    void DoAcAnalysis(const AcAnalysis ac_analysis);
    void DoTranAnalysis(const TranAnalysis tran_analysis);

    AnalysisMatrix GetAnalysisMatrix(const double frequency);
};

#endif  // ANALYZER_H
