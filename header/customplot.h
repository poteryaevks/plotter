#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include "icustomplotbuilder.h"
#include "qcustomplot.h"
#include "plotparams.h"
#include "libraryglobal.h"
#include <QWidget>

struct ConfigPlot {
        double xAxisMax;
        double xAxisMin;
        double yAxisMax;
        double yAxisMin;
};

using GraphValues = QVector<QPair<double, double>>;

class Library_EXPORT CustomPlot : public ICustomPlotBuilder
{
    public:
        CustomPlot();
        ~CustomPlot();

        QWidget* widget() override;
        void showViewer() override;
        void clearViewer() override;
        void addValues(const QVector<QPair<double, double>> &values) override;
        void addValues(const std::vector<std::pair<double, double>> &values) override;
        void addGraph(const QVector<QPair<double, double>> &values, const QString &nameGraph = QString()) override;
//        void addGraph(const std::vector<std::pair<double, double>> &values, const QString &nameGraph = QString()) override;
        void xAxisMinChanged(double value) override;
        void xAxisMaxChanged(double value) override;
        void yAxisMinChanged(double value) override;
        void yAxisMaxChanged(double value) override;
        void showHideLegend() override;
        void draw() override;
        void applyParams() override;

        QList<PlotParams*>& params() override { return m_params; }
        QColor generateColor(QColor color);
        QColor hsvToRgb(float h, float s, float v);

    private:
        ConfigPlot m_configPlot;
        QCustomPlot *m_plot;
        QVector<QCPGraph*> m_graphs;
        QList<PlotParams*> m_params;
        QList<GraphValues> m_graphsValues;

        QCPGraph* createGraph(PlotParams *params);
        void updateAxis();
        void repaintPlot();
        void setGraphColor(QCPGraph* g, QColor color);
};

#endif // CUSTOMPLOT_H
