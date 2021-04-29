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

    for (auto pgraph : m_graphs) {
        if (m_params.size() < i)
            break;
        auto ls = m_params.at(i)->getLineStyle();
        auto sc = m_params.at(i)->getScatterStyle();
        auto color = m_params.at(i)->getColor();
        pgraph->setLineStyle(ls);
        pgraph->setScatterStyle(QCPScatterStyle(sc));

        if (m_params.at(i)->isCustomColor()) {
            setGraphColor(pgraph, color);
            i++;
            continue;
        }

        graphWithGenColor.insert(i, pgraph);
        i++;
    }

    // Set begin color for first graph
    QColor color = QColor::fromHsv(220, 71, 100);
    double goldenRatioConjugate = 0.618033988749895;
    double hue = color.hsvHueF();

    for (auto k : graphWithGenColor.keys()) {
        hue += goldenRatioConjugate;
        hue = fmod(hue, 1.);
        setGraphColor(graphWithGenColor.value(k), hsvToRgb(hue, 0.5, 0.95));        // s=0.99, v=0.99; s=0.25, h=0.8; s=0.3, v=0.99
    }

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
    pen.setWidthF(3);
    g->setPen(pen);
}

QColor CustomPlot::hsvToRgb(float h, float s, float v)
{
    int h_i = int(h * 6);
    float f = h * 6 - h_i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    float r = 0;
    float g = 0;
    float b = 0;

    switch (h_i) {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
        case 1:
            r = q;
            g = v;
            b = p;
            break;
        case 2:
            r = p;
            g = v;
            b = t;
            break;
        case 3:
            r = p;
            g = q;
            b = v;
            break;
        case 4:
            r = t;
            g = p;
            b = v;
            break;
        case 5:
            r = v;
            g = p;
            b = q;
            break;
    }
    return QColor(int(r * 256), int(g * 256), int(b * 256));
}

QColor CustomPlot::generateColor(QColor color)
{
//    int red = QRandomGenerator::global()->bounded(256);
//    int green = QRandomGenerator::global()->bounded(256);
//    int blue = QRandomGenerator::global()->bounded(256);
    int red = 0;
    int green = 128;
    int blue = 255;

    // mix the color
    if (color.isValid()) {
        red = (red + color.red()) / 2;
        green = (green + color.green()) / 2;
        blue = (blue + color.blue()) / 2;
    }

    return QColor(red, green, blue);
}
