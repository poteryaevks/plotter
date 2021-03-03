#include "customplot.h"

CustomPlot::CustomPlot()
    : m_plot(new QCustomPlot)
{
    qDebug() << "create QCustomPlot";
}

CustomPlot::~CustomPlot()
{

}

QWidget* CustomPlot::widget()
{
    return m_plot;
}

void CustomPlot::showViewer()
{
    if (m_plot != nullptr)
        m_plot->show();
}

void CustomPlot::clearViewer()
{
    foreach (PlotParams* params, m_params) {
        if (params != nullptr)
            delete params;
    }

    m_graphs.clear();
    m_params.clear();
    m_plot->clearGraphs();
    m_plot->replot();
}

void CustomPlot::addValues(const QVector<QPair<double, double>> &values)
{
    m_values.append(values);
}

void CustomPlot::addGraph(const QVector<QPair<double, double>> &values, const QString &nameGraph)
{
    QCPGraph *pgraph = m_plot->addGraph();
    PlotParams *params = new PlotParams(defaultParams);
    params->setPlotName(nameGraph);

    if (pgraph != nullptr) {
        m_values.append(values);
        pgraph->setLineStyle(params->getLineStyle());
        pgraph->setScatterStyle(QCPScatterStyle(params->getScatterStyle()));
        setGraphColor(pgraph, QColor(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256)));

        QVector<QPair<double, double>>::const_iterator it(values.begin());
        QVector<double> xValues;
        QVector<double> yValues;

        while (it != values.end()) {
            xValues << (*it).first;
            yValues << (*it).second;
            ++it;
        }

        pgraph->setData(xValues, yValues);
        m_graphs << pgraph;
        m_params << params;
        updateAxis();
        repaintPlot();
    }
}

void CustomPlot::xAxisMinChanged(double value)
{
    m_configPlot.xAxisMin = value;
    repaintPlot();
}

void CustomPlot::xAxisMaxChanged(double value)
{
    m_configPlot.xAxisMax = value;
    repaintPlot();
}

void CustomPlot::yAxisMinChanged(double value)
{
    m_configPlot.yAxisMin = value;
    repaintPlot();
}

void CustomPlot::yAxisMaxChanged(double value)
{
    m_configPlot.yAxisMax = value;
    repaintPlot();
}

void CustomPlot::showHideLegend()
{
    m_plot->legend->setVisible(!m_plot->legend->visible());
    m_plot->replot();
}

void CustomPlot::updateAxis()
{
    double xMax = 0;
    double yMax = 0;
    double xMin = 0;
    double yMin = 0;

    for (int i = 0; i < m_values.size(); i++) {
        QVector<QPair<double, double>>::const_iterator it(m_values.at(i).begin());

        while (it != m_values.at(i).end()) {
            xMax = qMax(xMax, (*it).first);
            yMax = qMax(yMax, (*it).second);

            if (xMin == 0 || yMin == 0) {
                xMin = xMax;
                yMin = yMax;
            }
            xMin = qMin(xMin, (*it).first);
            yMin = qMin(yMin, (*it).second);
            ++it;
        }
    }
    m_configPlot.xAxisMax = xMax;
    m_configPlot.xAxisMin = xMin;
    m_configPlot.yAxisMax = yMax;
    m_configPlot.yAxisMin = yMin;
}

void CustomPlot::repaintPlot()
{
//    foreach(QCPGraph* pgraph, graphs) {

//        auto ls = m_params[i]->getLineStyle();
//        auto sc = m_params[i]->getScatterStyle();
//        auto color = m_params[i++]->getColor();
//        pgraph->setLineStyle(ls);
//        setGraphColor(pgraph, color);
//        pgraph->setScatterStyle(QCPScatterStyle(sc));
//    }

    m_plot->xAxis->setRange(m_configPlot.xAxisMin, m_configPlot.xAxisMax);
    m_plot->yAxis->setRange(m_configPlot.yAxisMin, m_configPlot.yAxisMax);
    m_plot->replot();
}

void CustomPlot::setGraphColor(QCPGraph* g, QColor color)
{
    if (g == nullptr)
        return;

    auto pen = g->pen();
    pen.setColor(color);
    g->setPen(pen);
}
