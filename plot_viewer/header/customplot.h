#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include "iplot.h"
#include "libraryglobal.h"
#include "common_types.h"

#include <QWidget>

class PvPlotParams;
class QCustomPlot;

class Library_EXPORT PvPlot final
        : public IPlot
{
    using CustomPlotPtr = std::unique_ptr<QCustomPlot>;
    using GraphPtr = std::unique_ptr<QCPGraph>;
    using PvPlotParamsPtr = std::shared_ptr<PvPlotParams>;
    using ParamsPtr = std::unique_ptr<PvPlotParams>;
    using GraphParamsType =  std::pair<QCPGraph*, ParamsPtr>;

    using Xs = QVector<double>;
    using Ys = QVector<double>;

public:

    PvPlot();

    ~PvPlot();

    QWidget* widget() override;
    QString title() const override;
    void setTitle(const QString &title) override;
    void showViewer() override;
    void clearViewer() override;
    void addValues(const Graph& values) override;
    void addGraph(const Graph& values, const QString &nameGraph = QString()) override;
    void addGraph(Graph&& values, QString&& nameGraph = QString()) override;
    void xAxisMinChanged(double value) override;
    void xAxisMaxChanged(double value) override;
    void yAxisMinChanged(double value) override;
    void yAxisMaxChanged(double value) override;
    void showHideLegend() override;
    void draw() override;
    void applyParams() override;

    QList<PvPlotParams*> params() override;
//    QColor generateColor(QColor color);

private:

    QCPGraph* createGraph(PvPlotParams *params);
    void updateAxis();
    void repaint();
    void setGraphColor(QCPGraph* graph, QColor color);

private:

    CustomPlotPtr plot_;
    std::vector<GraphParamsType> graphs_;


    double xMin_, xMax_, yMin_, yMax_;
};


#endif // CUSTOMPLOT_H
