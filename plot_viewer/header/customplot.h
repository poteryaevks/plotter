#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include "icustomplotbuilder.h"
#include "plotparams.h"
#include "libraryglobal.h"
#include "common_types.h"


#include <QWidget>

class QCustomPlot;

struct ConfigPlot {
    double xAxisMax;
    double xAxisMin;
    double yAxisMax;
    double yAxisMin;
};

//using GraphValues = QVector<QPair<double, double>>;

class Library_EXPORT PvPlot final : public IPlot
{
    using CustomPlotPtr = std::unique_ptr<QCustomPlot>;

public:

    PvPlot();

    ~PvPlot();

    QWidget* widget() override;
    QString title() const override;
    void setTitle(const QString &title) override;
    void showViewer() override;
    void clearViewer() override;
    void addValues(const Graph& values) override;
    void addValues(const std::vector<std::pair<double, double>> &values) override;
    void addGraph(const Graph& values, const QString &nameGraph = QString()) override;
    void addGraph(Graph&& values, QString&& nameGraph = QString()) override;
    void xAxisMinChanged(double value) override;
    void xAxisMaxChanged(double value) override;
    void yAxisMinChanged(double value) override;
    void yAxisMaxChanged(double value) override;
    void showHideLegend() override;
    void draw() override;
    void applyParams() override;

    QList<PvPlotParams*>& params() override { return m_params; }
    QColor generateColor(QColor color);
    QColor hsvToRgb(float h, float s, float v);

private:

    QCPGraph* createGraph(PvPlotParams *params);
    void updateAxis();
    void repaintPlot();
    void setGraphColor(QCPGraph* g, QColor color);
private:

    ConfigPlot m_configPlot;
    CustomPlotPtr m_plot;
    QVector<QCPGraph*> m_graphs;
    QList<PvPlotParams*> m_params;
    QList<Graph> m_graphsValues;

};

#endif // CUSTOMPLOT_H
