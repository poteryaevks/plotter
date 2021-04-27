#include "customplot.h"
#include <QScreen>

extern "C" Library_EXPORT CustomPlotBuilderPtr Instance()
{
    return CustomPlot::CreateInstance();
}

CustomPlot::CustomPlot()
    : m_plot(new QCustomPlot)
{
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    qDebug() << "Сreate QCustomPlot";
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
    if (m_plot != nullptr) {
        QRect screenGeometry = QGuiApplication::screens()[0]->geometry();

        // resize widget
        m_plot->resize(screenGeometry.width() / 2, screenGeometry.height() / 2);
        // set center display
        int x = (screenGeometry.width() - m_plot->width()) / 2;
        int y = (screenGeometry.height() - m_plot->height()) / 2;
        m_plot->move(x, y);

        m_plot->show();
    }
}

void CustomPlot::clearViewer()
{
    foreach (PlotParams* params, m_params) {
        if (params != nullptr)
            delete params;
    }

    m_configPlot = {};
    m_graphsValues.clear();
    m_graphs.clear();
    m_params.clear();
    m_plot->clearGraphs();
    m_plot->replot();
}

void CustomPlot::addValues(const QVector<QPair<double, double>> &values)
{
    m_graphsValues.append(values);
}

void CustomPlot::addValues(const std::vector<std::pair<double, double>> &values)
{
    GraphValues graphValues;

    for (auto v : values)
        graphValues.append(qMakePair(v.first, v.second));

    m_graphsValues.append(graphValues);
}

void CustomPlot::addGraph(const QVector<QPair<double, double>> &values, const QString &nameGraph)
{
    PlotParams *params = new PlotParams(defaultParams);
    params->setPlotName(nameGraph);

    QCPGraph* graph = createGraph(params);

    if (graph != nullptr) {
        m_graphsValues.append(values);

        QVector<double> xValues;
        QVector<double> yValues;

        for (auto pointValue : values) {
            xValues << pointValue.first;
            yValues << pointValue.second;
        }

        graph->setData(xValues, yValues);
        m_graphs << graph;
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

void CustomPlot::draw()
{
    for (auto graphValues : m_graphsValues) {
        PlotParams *params = new PlotParams(defaultParams);
        QCPGraph* graph = createGraph(params);

        if (graph != nullptr) {
            QVector<double> xValues;
            QVector<double> yValues;

            for (auto pointValue : graphValues) {
                xValues << pointValue.first;
                yValues << pointValue.second;
            }
            graph->setData(xValues, yValues);
            m_graphs << graph;
            m_params << params;
        }
    }
    updateAxis();
    repaintPlot();
}

void CustomPlot::applyParams()
{
    repaintPlot();
}

QCPGraph* CustomPlot::createGraph(PlotParams *params)
{
    QCPGraph *pgraph = m_plot->addGraph();

    if (pgraph != nullptr && params != nullptr) {
//        setGraphColor(pgraph, QColor(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256)));
        pgraph->setLineStyle(params->getLineStyle());
        pgraph->setScatterStyle(QCPScatterStyle(params->getScatterStyle()));
        if (!params->getPlotName().isEmpty())
            pgraph->setName(params->getPlotName());

        return pgraph;
    }
    return nullptr;
}

void CustomPlot::updateAxis()
{
    double xMax = 0;
    double yMax = 0;
    double xMin = 0;
    double yMin = 0;

    for (int i = 0; i < m_graphsValues.size(); i++) {
        QVector<QPair<double, double>>::const_iterator it(m_graphsValues.at(i).begin());

        while (it != m_graphsValues.at(i).end()) {
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
    int i = 0;
    QMap<int, QCPGraph*> graphWithGenColor;

    QList<QColor> tempColor = {QColor(0, 128, 255),
                               QColor(255, 128, 0),
                               QColor(128, 0, 255)};

    for (auto pgraph : m_graphs) {
        if (m_params.size() < i)
            break;
        auto ls = m_params.at(i)->getLineStyle();
        auto sc = m_params.at(i)->getScatterStyle();
        auto color = m_params.at(i)->getColor();
        pgraph->setLineStyle(ls);
        pgraph->setScatterStyle(QCPScatterStyle(sc));

        // temp
        setGraphColor(pgraph, tempColor[i]);
//        if (m_params.at(i)->isCustomColor()) {
//            setGraphColor(pgraph, color);
//            i++;
//            continue;
//        }

        graphWithGenColor.insert(i, pgraph);
        i++;
    }

//    QColor colorGen(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256));
//    QColor colorGen(0, 128, 255);

//    int redColor = colorGen.red();

//    for (auto k : graphWithGenColor.keys()) {
//        if (redColor > 255)
//            redColor = redColor - 255;
//        colorGen.setRed(redColor);
//        m_params.at(k)->setColor(colorGen);
//        setGraphColor(graphWithGenColor.value(k), colorGen);
//        redColor = redColor + (360 / graphWithGenColor.size());
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
    pen.setWidthF(1.5);
    g->setPen(pen);
}
