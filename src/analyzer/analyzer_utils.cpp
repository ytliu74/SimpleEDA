/**
 * @file analyzer_utils.cpp
 * @author Yaotian Liu
 * @brief utils for analyzer
 * @date 2022-10-23
 */

#include "analyzer.h"

using arma::cx_mat;
using std::complex;
using std::cout;
using std::endl;
using std::setw;
using std::vector;

void DcPlot(DcResult result, PrintVariable print_variable);
void AcPlot(AcResult result, PrintVariable print_variable);
void TranPlot(TranResult result, PrintVariable print_variable);
void Plot(QVector<double> x, QVector<double> y, QString x_label, QString y_label,
          bool x_log, bool y_log);

Analyzer::Analyzer(Parser parser) {
    circuit = parser.GetCircuit();

    auto analysis_type = parser.GetAnalysisType();
    auto dc_analysis = parser.GetDcAnalysis();
    auto ac_analysis = parser.GetAcAnalysis();
    auto tran_analysis = parser.GetTranAnalysis();
    auto print_variable = parser.GetPrintVariable();

    switch (analysis_type) {
        case DC: {
            cout << "Running DC analysis" << endl;
            DoDcAnalysis(dc_analysis);
            if (print_variable.print_type)
                DcPlot(dc_result, print_variable);
            break;
        }
        case AC: {
            cout << "Running AC analysis" << endl;
            DoAcAnalysis(ac_analysis);
            if (print_variable.print_type)
                AcPlot(ac_result, print_variable);
            break;
        }
        case TRAN: {
            cout << "Running TRAN analysis" << endl;
            DoTranAnalysis(tran_analysis);
            if (print_variable.print_type)
                TranPlot(tran_result, print_variable);
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

void DcPlot(DcResult result, PrintVariable print_variable) {
    NodeName node = print_variable.node;
    int node_index = FindNode(result.node_vec, node);
    QVector<double> x;
    for (auto dc_value : result.dc_value_vec)
        x.push_back(dc_value);

    QVector<double> y;
    for (auto dc_result : result.dc_result_vec)
        y.push_back(dc_result(node_index));

    Plot(x, y, QString("Vsrc"), QString("Value"), false, false);
}

void AcPlot(AcResult result, PrintVariable print_variable) {
    NodeName node = print_variable.node;
    int node_index = FindNode(result.node_vec, node);

    QVector<double> freq;
    for (auto f : result.freq_vec)
        freq.push_back(f);

    QVector<double> y;
    for (auto ac_result : result.ac_result_vec) {
        complex r = ac_result(node_index);
        switch (print_variable.analysis_variable_type) {
            case MAG: {
                y.push_back(sqrt(pow(r.real(), 2) + pow(r.imag(), 2)));
                break;
            }
            case REAL: {
                y.push_back(r.real());
                break;
            }
            case IMAGINE: {
                y.push_back(r.imag());
                break;
            }
            case PHASE: {
                y.push_back(atan(r.imag() / r.real()));
                break;
            }
            case DB: {
                // TODO
                break;
            }
        }
    }

    Plot(freq, y, QString("Frequency"), QString("Value"), true, false);
}

void TranPlot(TranResult result, PrintVariable print_variable) {
    NodeName node = print_variable.node;
    int node_index = FindNode(result.node_vec, node);

    QVector<double> t;
    for (auto t_p : result.time_point_vec)
        t.push_back(t_p);

    QVector<double> y;
    for (int i = 0; i < result.tran_result_mat.n_cols; i++)
        y.push_back(result.tran_result_mat(node_index, i));

    Plot(t, y, QString("Time"), QString("Value"), false, false);
}

// Plot with x and y
void Plot(QVector<double> x, QVector<double> y, QString x_label, QString y_label,
          bool x_log, bool y_log) {
    QCustomPlot* plot = new QCustomPlot();
    plot->addGraph(plot->xAxis, plot->yAxis);
    plot->graph()->setPen(QPen(Qt::blue));
    plot->graph()->setLineStyle(QCPGraph::lsLine);
    plot->graph()->setData(x, y);
    if (x_log) {
        QSharedPointer<QCPAxisTickerLog> logTickerx(new QCPAxisTickerLog);
        plot->xAxis->setTicker(logTickerx);
        plot->xAxis->setScaleType(QCPAxis::stLogarithmic);
    }
    if (y_log) {
        QSharedPointer<QCPAxisTickerLog> logTickery(new QCPAxisTickerLog);
        plot->yAxis->setTicker(logTickery);
        plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    }

    plot->graph()->rescaleAxes();

    plot->xAxis->setLabel(x_label);
    plot->yAxis->setLabel(y_label);

    plot->setMinimumSize(450, 300);
    plot->show();
}