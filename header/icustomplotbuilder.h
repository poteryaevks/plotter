#ifndef ICUSTOMPLOTBUILDER_H
#define ICUSTOMPLOTBUILDER_H

#include "ibasegraphbuilder.h"
#include <memory>
#include <QWidget>

class ICustomPlotBuilder;
using CustomPlotBuilderPtr = std::shared_ptr<ICustomPlotBuilder>;

class ICustomPlotBuilder : public IBaseGraphBuilder
{
    protected:
        virtual ~ICustomPlotBuilder() = default;

    public:
        virtual QWidget* widget() = 0;
        virtual void showViewer() = 0;
        virtual void clearViewer() = 0;
        virtual void addValues(const QVector<QPair<double, double>> &values) = 0;
        virtual void addGraph(const QVector<QPair<double, double>> &values, const QString &nameGraph = QString()) = 0;
        virtual void xAxisMinChanged(double value) = 0;
        virtual void xAxisMaxChanged(double value) = 0;
        virtual void yAxisMinChanged(double value) = 0;
        virtual void yAxisMaxChanged(double value) = 0;
        virtual void showHideLegend() = 0;
        static CustomPlotBuilderPtr CreateInstance(); // функция-фабрика
};

#endif // ICUSTOMPLOTBUILDER_H
