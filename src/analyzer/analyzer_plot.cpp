/**
 * @file analyzer_plot.cpp
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-28
 */

#include "analyzer.h"

using std::complex;

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
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plot->xAxis->setLabel(x_label);
    plot->yAxis->setLabel(y_label);

    plot->setMinimumSize(450, 300);
    plot->show();
}