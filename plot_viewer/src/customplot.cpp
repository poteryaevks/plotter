#include "customplot.h"
#include "qcustomplot.h"
#include "common_types.h"

#include <QScreen>

using namespace plot_viewer;

extern "C" Library_EXPORT CustomPlotBuilderPtr Instance()
{
    return PvPlot::CreateInstance();
}

PvPlot::PvPlot()
    : plot_(std::make_unique<QCustomPlot>())
{
    plot_->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    qDebug() << "Сreate QCustomPlot";
}

PvPlot::~PvPlot()
{
    clearViewer();
}

QWidget* PvPlot::widget()
{
    return plot_.get();
}

QString PvPlot::title() const
{
    if (plot_->plotLayout()->rowCount() > 1) {
        QCPTextElement *titleElement = qobject_cast<QCPTextElement*>(plot_->plotLayout()->element(0, 0));
        if (titleElement != nullptr)
            return titleElement->text();
    }

    return {};
}

void PvPlot::setTitle(const QString &title)
{
    if (plot_->plotLayout()->rowCount() == 1) {
        plot_->plotLayout()->insertRow(0);
        plot_->plotLayout()->addElement(0, 0, new QCPTextElement(plot_.get(), title, QFont("sans", 12, QFont::Bold)));
    }
    else {
        QCPTextElement *titleElement = qobject_cast<QCPTextElement*>(plot_->plotLayout()->element(0, 0));
        if (titleElement != nullptr)
            titleElement->setText(title);
    }

    repaint();
}

void PvPlot::showViewer()
{
    if (plot_ != nullptr) {
        QRect screenGeometry = QGuiApplication::screens()[0]->geometry();

        // resize widget
        plot_->resize(screenGeometry.width() / 2, screenGeometry.height() / 2);
        // set center display
        int x = (screenGeometry.width() - plot_->width()) / 2;
        int y = (screenGeometry.height() - plot_->height()) / 2;
        plot_->move(x, y);

        plot_->show();
    }
}

void PvPlot::clearViewer()
{
    for (auto params : params_)
        if (params)
            delete params;

    params_.clear();

    configPlot_ = {};
    graphsValues_.clear();
    //    graphs_.clear();
    plot_->clearGraphs();
    plot_->replot();
}

void PvPlot::addValues(const Graph &values)
{
    graphsValues_.append(values);
}

void PvPlot::addValues(const std::vector<std::pair<double, double>> &values)
{
    Graph graphValues;

    for (auto v : values)
        graphValues.append(qMakePair(v.first, v.second));

    graphsValues_.append(graphValues);
}

void PvPlot::addGraph(const Graph &values, const QString &nameGraph)
{
    PvPlotParams *params = new PvPlotParams(defaultParams);
    params->setPlotName(nameGraph);

    auto graph = createGraph(params);

    if (!graph)
        return;

    graphsValues_ << values;

    QVector<double> xs;
    QVector<double> ys;

    for (const auto& pointValue : values) {
        xs << pointValue.first;
        ys << pointValue.second;
    }

    graph->setData(xs, ys);
    params_ << params;
    updateAxis();
    repaint();
}

void PvPlot::addGraph(Graph&& values, QString&& nameGraph)
{
    Q_UNUSED(values)
    Q_UNUSED(nameGraph)
}

void PvPlot::xAxisMinChanged(double value)
{
    configPlot_.xAxisMin = value;
    repaint();
}

void PvPlot::xAxisMaxChanged(double value)
{
    configPlot_.xAxisMax = value;
    repaint();
}

void PvPlot::yAxisMinChanged(double value)
{
    configPlot_.yAxisMin = value;
    repaint();
}

void PvPlot::yAxisMaxChanged(double value)
{
    configPlot_.yAxisMax = value;
    repaint();
}

void PvPlot::showHideLegend()
{
    plot_->legend->setVisible(!plot_->legend->visible());
    plot_->replot();
}

void PvPlot::draw()
{
    for (auto graphValues : graphsValues_) {

        PvPlotParams *params = new PvPlotParams(defaultParams);
        QCPGraph* graph = createGraph(params);

        if (graph != nullptr) {
            QVector<double> xValues;
            QVector<double> yValues;

            for (auto pointValue : graphValues) {
                xValues << pointValue.first;
                yValues << pointValue.second;
            }
            graph->setData(xValues, yValues);
            params_ << params;
        }
    }
    updateAxis();
    repaint();
}

void PvPlot::applyParams()
{
    repaint();
}

QCPGraph* PvPlot::createGraph(PvPlotParams *params)
{
    QCPGraph *pgraph = plot_->addGraph();

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

void PvPlot::updateAxis()
{
    double xMax = 0;
    double yMax = 0;
    double xMin = 0;
    double yMin = 0;

    for (const auto& graphValue : graphsValues_) {  //int i = 0; i < graphsValues_.size(); i++

        for (const auto& point : graphValue){

            auto x = point.first;
            auto y = point.second;

            xMax = qMax(xMax, x);
            yMax = qMax(yMax, y);

            if (xMin == 0 || yMin == 0) {
                xMin = xMax;
                yMin = yMax;
            }

            xMin = qMin(xMin, x);
            yMin = qMin(yMin, y);
        }
    }

    configPlot_.xAxisMax = xMax;
    configPlot_.xAxisMin = xMin;
    configPlot_.yAxisMax = yMax;
    configPlot_.yAxisMin = yMin;
}

void PvPlot::repaint()
{
    QMap<int, QCPGraph*> graphWithGenColor;

    for (int j = 0; j < plot_->graphCount(); j++) {

        auto graph = plot_->graph(j);

        if (params_.size() < j)
            break;

        auto ls = params_.at(j)->getLineStyle();
        auto sc = params_.at(j)->getScatterStyle();
        auto color = params_.at(j)->getColor();
        graph->setLineStyle(ls);
        graph->setScatterStyle(QCPScatterStyle(sc));

        if (params_.at(j)->isCustomColor()) {
            setGraphColor(graph, color);
            continue;
        }

        graphWithGenColor.insert(j, graph);
    }

    // Set begin color for first graph
    QColor color = QColor::fromHsv(220, 71, 100);
    double goldenRatioConjugate = 0.618033988749895;
    double hue = color.hsvHueF();

    for (auto k : graphWithGenColor.keys()) {
        hue += goldenRatioConjugate;
        hue = fmod(hue, 1.);
        setGraphColor(graphWithGenColor.value(k), hsvToRgb(hue, 0.5, 0.95));        // s=0.99, v=0.99; s=0.25, h=0.8; s=0.3, v=0.99

        if (k < params_.size())
            params_[k]->setColor(hsvToRgb(hue, 0.5, 0.95));
    }

    plot_->xAxis->setRange(configPlot_.xAxisMin, configPlot_.xAxisMax);
    plot_->yAxis->setRange(configPlot_.yAxisMin, configPlot_.yAxisMax);
    plot_->replot();
}

void PvPlot::setGraphColor(QCPGraph* graph, QColor color)
{
    if (graph == nullptr)
        return;

    qDebug() << graph << color;
    auto pen = graph->pen();
    pen.setColor(color);
    pen.setWidthF(3);
    graph->setPen(pen);
}

QColor PvPlot::hsvToRgb(float h, float s, float v)
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

QColor PvPlot::generateColor(QColor color)
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
