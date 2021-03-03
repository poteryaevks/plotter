#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include "icustomplotbuilder.h"
#include "qcustomplot.h"
#include "plotparams.h"
#include <QWidget>

struct ConfigPlot {
        double xAxisMax;
        double xAxisMin;
        double yAxisMax;
        double yAxisMin;
};

class CustomPlot : public ICustomPlotBuilder
{
    public:
        CustomPlot();
        ~CustomPlot();

        QWidget* widget() override;
        void showViewer() override;
        void clearViewer() override;
        void addValues(const QVector<QPair<double, double>> &values) override;
        void addGraph(const QVector<QPair<double, double>> &values, const QString &nameGraph = QString()) override;
        void xAxisMinChanged(double value) override;
        void xAxisMaxChanged(double value) override;
        void yAxisMinChanged(double value) override;
        void yAxisMaxChanged(double value) override;
        void showHideLegend() override;

    private:
        ConfigPlot m_configPlot;
        QCustomPlot *m_plot;
        QVector<QCPGraph*> m_graphs;
        QVector<PlotParams*> m_params;
        QList<QVector<QPair<double, double>>> m_values;

        void updateAxis();
        void repaintPlot();
        void setGraphColor(QCPGraph* g, QColor color);
};

#endif // CUSTOMPLOT_H
