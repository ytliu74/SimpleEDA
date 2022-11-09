/**
 * @file analyzer_utils.cpp
 * @author Yaotian Liu
 * @brief utils for analyzer
 * @date 2022-10-23
 */

#include "analyzer.h"

using arma::cx_mat;
using std::cout;
using std::endl;
using std::setw;
using std::vector;

Analyzer::Analyzer(Parser parser) {
    circuit = parser.GetCircuit();

    auto analysis_type = parser.GetAnalysisType();
    auto dc_analysis = parser.GetDcAnalysis();
    auto ac_analysis = parser.GetAcAnalysis();
    auto tran_analysis = parser.GetTranAnalysis();
    auto print_variable_vec = parser.GetPrintVariables();

    switch (analysis_type) {
        case DC: {
            cout << "Running DC analysis" << endl;
            DoDcAnalysis(dc_analysis);
            if (!print_variable_vec.empty())
                DcPlot(dc_result, print_variable_vec);
            break;
        }
        case AC: {
            cout << "Running AC analysis" << endl;
            DoAcAnalysis(ac_analysis);
            if (!print_variable_vec.empty())
                AcPlot(ac_result, print_variable_vec);
            break;
        }
        case TRAN: {
            cout << "Running TRAN analysis" << endl;
            DoTranAnalysis(tran_analysis);
            if (!print_variable_vec.empty())
                TranPlot(tran_result, print_variable_vec);
            break;
        }
        default: break;
    }
}

void Analyzer::PrintMatrix(cx_mat mat, vector<NodeName> nodes) {
    cout << "Matrix: " << endl << ' ';
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
                 << "j"
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
    cout << endl;
}

void Analyzer::PrintRHS(cx_mat rhs, vector<NodeName> nodes) {
    cout << "RHS: " << endl;
    // cout << std::setiosflags(std::ios::fixed) << std::setprecision(4);
    for (uint i = 0; i < rhs.n_rows; i++) {
        cout << setw(4) << nodes[i] << "\t";
        if (i == 0)
            cout << "╭";
        else if (i == rhs.n_rows - 1)
            cout << "╰";
        else
            cout << "│";

        cout << setw(8) << std::real(rhs(i, 0)) << " ";

        if (i == 0)
            cout << "╮" << endl;
        else if (i == rhs.n_rows - 1)
            cout << "╯" << endl;
        else
            cout << "│" << endl;
    }
}

int FindNode(vector<NodeName> node_vec, NodeName name) {
    for (std::size_t i = 0; i < node_vec.size(); i++) {
        if (node_vec[i] == name) {
            return i;
        }
    }
    cout << "Not found: " << name << endl;
    return -1;
}

void AddExpTerm(const std::vector<ExpTerm> exp_term_vec, const arma::vec result,
                arma::mat &mat) {
    for (ExpTerm exp_analysis : exp_term_vec) {
        int node_1_index = exp_analysis.node_1_index;
        int node_2_index = exp_analysis.node_2_index;
        int row_index = exp_analysis.row_index;
        int col_index = exp_analysis.col_index;
        ExpCoeff zero_order = exp_analysis.zero_order;
        ExpCoeff first_order = exp_analysis.first_order;

        double value;
        // Both the value related node is not GND
        if (node_1_index >= 0 && node_2_index >= 0) {
            value = result(node_1_index) - result(node_2_index);
        }
        // Node_2 is GND
        else if (node_1_index >= 0) {
            value = result(node_1_index);
        }
        // Node_1 is GND
        else {
            value = -1 * result(node_2_index);
        }

        // If the stamp point is still in the reduced matrix
        if (row_index >= 0 && col_index >= 0) {
            mat(row_index, col_index) +=
                zero_order.constant +
                zero_order.exp.real() * exp(zero_order.exp.imag() * value) +
                (first_order.exp.real() * exp(first_order.exp.imag() * value) +
                 first_order.constant) *
                    value;
        }
    }
}

double VecDifference(arma::vec vec_old, arma::vec vec_new) {
    arma::vec diff = vec_old - vec_new;
    int size = vec_old.size();

    for (int i = 0; i < size; i++) {
        double delta = fabs(diff(i));
        double epsilon = std::min(EPSILON_ABS, EPSILON_REL * fabs(vec_old(i)));
        if (delta > epsilon)
            return false;
    }

    return true;
}