/**
 * @file analyzer_plot.cpp
 * @author Yaotian Liu
 * @brief
 * @date 2022-10-28
 */

#include "analyzer.h"

using std::complex;

void DcPlot(DcResult result, std::vector<PrintVariable> print_variable_vec) {
    std::vector<QVector<double>> x_vec, y_vec;
    std::vector<NodeName> name_vec;

    for (auto print_variable : print_variable_vec) {
        NodeName node = print_variable.node;
        int node_index = FindNode(result.node_vec, node);
        QVector<double> x;
        for (auto dc_value : result.dc_value_vec)
            x.push_back(dc_value);

        QVector<double> y;
        for (auto dc_result : result.dc_result_vec)
            y.push_back(dc_result(node_index));

        x_vec.push_back(x);
        y_vec.push_back(y);
        name_vec.push_back(node);
    }

    Plot(x_vec, y_vec, name_vec, QString("Vsrc"), QString("Value"), false, false);
}

void AcPlot(AcResult result, std::vector<PrintVariable> print_variable_vec) {
    std::vector<QVector<double>> x_vec, y_vec;
    std::vector<NodeName> name_vec;

    for (auto print_variable : print_variable_vec) {
        NodeName node = print_variable.node;
        int node_index = FindNode(result.node_vec, node);

        QVector<double> freq;
        for (auto f : result.freq_vec)
            freq.push_back(f);

        QVector<double> y;
        for (auto ac_result : result.ac_result_vec) {
            complex<double> r = ac_result(node_index);
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
        x_vec.push_back(freq);
        y_vec.push_back(y);
        name_vec.push_back(node);
    }

    Plot(x_vec, y_vec, name_vec, QString("Frequency"), QString("Value"), true, false);
}

void TranPlot(TranResult result, std::vector<PrintVariable> print_variable_vec) {
    std::vector<QVector<double>> x_vec, y_vec;
    std::vector<NodeName> name_vec;

    for (auto print_variable : print_variable_vec) {
        NodeName node = print_variable.node;
        int node_index = FindNode(result.node_vec, node);

        QVector<double> t;
        for (auto t_p : result.time_point_vec)
            t.push_back(t_p);

        QVector<double> y;
        for (int i = 0; i < result.tran_result_mat.n_cols; i++)
            y.push_back(result.tran_result_mat(node_index, i));

        x_vec.push_back(t);
        y_vec.push_back(y);
        name_vec.push_back(node);
    }

    Plot(x_vec, y_vec, name_vec, QString("Time"), QString("Value"), false, false);
}

// Plot with x and y
void Plot(std::vector<QVector<double>> x_vec, std::vector<QVector<double>> y_vec,
          std::vector<NodeName> name_vec, QString x_label, QString y_label, bool x_log,
          bool y_log) {
    QCustomPlot* plot = new QCustomPlot();

    int plot_num = x_vec.size();

    std::vector<QPen> pens = {QPen(Qt::blue), QPen(Qt::red), QPen(Qt::darkYellow)};

    for (int i = 0; i < plot_num; i++) {
        plot->addGraph(plot->xAxis, plot->yAxis);
        plot->graph(i)->setPen(pens[i]);
        plot->graph(i)->setLineStyle(QCPGraph::lsLine);
        plot->graph(i)->setData(x_vec[i], y_vec[i]);
        plot->graph(i)->setName(name_vec[i]);
    }

    plot->legend->setVisible(true);

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