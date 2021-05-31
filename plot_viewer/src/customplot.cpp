#include "customplot.h"
#include "plotparams.h"

#include "qcustomplot.h"

#include "plot_factory.h"

#include <QScreen>


namespace {

const bool bPvPlotCreated = plot_viewer::FACTORY.Register(QCUSTOM_PLOT, std::make_unique<PvPlot>);

QColor hsvToRgb(float h, float s, float v)
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

const QColor COLOR = QColor::fromHsv(220, 71, 100);
double hue = COLOR.hsvHueF();

auto colorGen = [](){

    constexpr double goldenRatioConjugate = 0.618033988749895;

    hue += goldenRatioConjugate;
    hue = fmod(hue, 1.);

    return hsvToRgb(hue, 0.5, 0.95);
};

auto getMaxX = [](QCPGraph* graph){

    auto graphData = std::max_element(graph->data()->begin(),
                                      graph->data()->end(),
                                      [](const auto& left, const auto& right){

        return left.key < right.key; }
    );

    return graphData->key;
};

auto getMaxY = [](QCPGraph* graph){

    auto graphData = std::max_element(graph->data()->begin(),
                                      graph->data()->end(),
                                      [](const auto& left, const auto& right){

        return left.value < right.value; }
    );

    return graphData->value;
};

auto getMinX = [](QCPGraph* graph){

    auto graphData = std::min_element(graph->data()->begin(),
                                      graph->data()->end(),
                                      [](const auto& left, const auto& right){

        return left.key < right.key; }
    );

    return graphData->key;
};

auto getMinY = [](QCPGraph* graph){

    auto graphData = std::min_element(graph->data()->begin(),
                                      graph->data()->end(),
                                      [](const auto& left, const auto& right){

        return left.value < right.value; }
    );

    return graphData->value;
};

}

using namespace plot_viewer;

extern "C" Library_EXPORT std::shared_ptr<IPlot> Instance()
{
    return plot_viewer::FACTORY.Create(QCUSTOM_PLOT);
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
        plot_->plotLayout()->addElement(0, 0,
                                        new QCPTextElement(plot_.get(),
                                                           title,
                                                           QFont("sans", 12, QFont::Bold))
                                        );
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
    graphs_.clear();
    plot_->clearGraphs();
    plot_->replot();
}

void PvPlot::addValues(const Graph &values)
{
    Q_UNUSED(values)
}

void PvPlot::addGraph(const Graph &values, const QString &nameGraph)
{
    auto params = std::make_unique<PvPlotParams>(colorGen(),
                                                 QCPScatterStyle::ssDot,
                                                 QCPGraph::lsLine,
                                                 nameGraph);
    auto graph = createGraph(params.get());

    if (!graph)
        return;

    QVector<double> xs;
    QVector<double> ys;

    for (const auto& pointValue : values) {
        xs << pointValue.first;
        ys << pointValue.second;
    }

    graph->setData(std::move(xs),
                   std::move(ys));

    graphs_.emplace_back(
                GraphParamsType(graph,
                                std::move(params))
                );

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
    Q_UNUSED(value)
    //    zoom_.xMin = value;
    repaint();
}

void PvPlot::xAxisMaxChanged(double value)
{
    Q_UNUSED(value)
    //    zoom_.xMax = value;
    repaint();
}

void PvPlot::yAxisMinChanged(double value)
{
    Q_UNUSED(value)
    //    zoom_.yMin = value;
    repaint();
}

void PvPlot::yAxisMaxChanged(double value)
{
    Q_UNUSED(value)
    //    zoom_.yMax = value;
    repaint();
}

void PvPlot::showHideLegend()
{
    plot_->legend->setVisible(!plot_->legend->visible());
    plot_->replot();
}

void PvPlot::draw()
{
    //        for (auto graphValues : graphsValues_) {

    //            auto params = std::make_unique<PvPlotParams>(colorGen(),
    //                                                         QCPScatterStyle::ssDot,
    //                                                         QCPGraph::lsLine);

    //            QCPGraph* graph = createGraph(params.get());

    //            if (!graph)
    //                return;

    //            Xs xs;
    //            Ys ys;

    //            for (auto pointValue : graphValues) {
    //                xs << pointValue.first;
    //                ys << pointValue.second;
    //            }

    //            graph->setData(xs, ys);

    //            graphs_.emplace_back(
    //                        std::make_pair(graph, std::move(params))
    //                        );
    //        }

    updateAxis();
    repaint();
}

void PvPlot::applyParams()
{
    repaint();
}

QList<PvPlotParams*> PvPlot::params()
{
    QList<PvPlotParams*> params;

    for(const auto& graph : graphs_){
        params << graph.second.get();
    }

    return std::move(params);
}

QCPGraph* PvPlot::createGraph(PvPlotParams *params)
{
    QCPGraph *pgraph = plot_->addGraph();

    if (pgraph != nullptr && params != nullptr) {

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
    double xMax {};
    double xMin {};
    double yMax {};
    double yMin {};

    for (const auto& graph : graphs_) {

        QCPGraph* graphImpl = graph.first;
        if(!graphImpl)
            return;

        auto minX = getMinX(graphImpl);
        xMin = std::min(minX, xMin);

        auto maxX = getMaxX(graphImpl);
        xMax = std::max(maxX, xMax);

        auto minY = getMinY(graphImpl);
        yMin = std::min(minY, yMin);

        auto maxY = getMaxY(graphImpl);
        yMax = std::max(maxY, yMax);
    }

    plot_->xAxis->setRange(xMin, xMax);
    plot_->yAxis->setRange(yMin, yMax);
}

void PvPlot::repaint()
{
    for (const auto& graph : graphs_) {

        auto graphImpl = graph.first;
        auto params = graph.second.get();

        if(!params || !graphImpl)
            continue;

        graphImpl->setLineStyle(
                    params->getLineStyle()
                    );

        graphImpl->setScatterStyle(
                    params->getScatterStyle()
                    );

        setGraphColor(graphImpl,
                      params->getColor()
                      );

    }

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


//QColor PvPlot::generateColor(QColor color)
//{
//    //    int red = QRandomGenerator::global()->bounded(256);
//    //    int green = QRandomGenerator::global()->bounded(256);
//    //    int blue = QRandomGenerator::global()->bounded(256);
//    int red = 0;
//    int green = 128;
//    int blue = 255;

//    // mix the color
//    if (color.isValid()) {
//        red = (red + color.red()) / 2;
//        green = (green + color.green()) / 2;
//        blue = (blue + color.blue()) / 2;
//    }

//    return QColor(red, green, blue);
//}
