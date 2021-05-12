#ifndef ICUSTOMPLOTBUILDER_H
#define ICUSTOMPLOTBUILDER_H

#include "ibasegraphbuilder.h"
#include <memory>
#include <QWidget>

class PlotParams;
class ICustomPlotBuilder;
using CustomPlotBuilderPtr = std::shared_ptr<ICustomPlotBuilder>;

/*!
 * \brief The ICustomPlotBuilder class Интерфейсный класс для взаимодей с QCustomPlot
 */
class ICustomPlotBuilder : public IBaseGraphBuilder
{
    protected:
        virtual ~ICustomPlotBuilder() = default;

    public:
        virtual QWidget* widget() = 0;

        /*!
         * \brief showViewer Отображение виджета для вывода графиков
         */
        virtual void showViewer() = 0;
        /*!
         * \brief clearViewer Очистка вижжета от графиков
         */
        virtual void clearViewer() = 0;
        /*!
         * \brief addValues Добавить данные
         * \param values
         */
        virtual void addValues(const QVector<QPair<double, double>> &values) = 0;
        /*!
         * \brief addValues Добавить данные
         * \param values
         */
        virtual void addValues(const std::vector<std::pair<double, double>> &values) = 0;
        /*!
         * \brief addGraph Добавить график
         * \param values Данные для графика
         * \param nameGraph Название графика, отображается в легенде
         */
        virtual void addGraph(const QVector<QPair<double, double>> &values, const QString &nameGraph = QString()) = 0;
//        virtual void addGraph(const std::vector<std::pair<double, double>> &values, const QString &nameGraph = QString()) = 0;
        /*!
         * \brief xAxisMinChanged
         * \param value
         */
        virtual void xAxisMinChanged(double value) = 0;
        /*!
         * \brief xAxisMaxChanged
         * \param value
         */
        virtual void xAxisMaxChanged(double value) = 0;
        /*!
         * \brief yAxisMinChanged
         * \param value
         */
        virtual void yAxisMinChanged(double value) = 0;
        /*!
         * \brief yAxisMaxChanged
         * \param value
         */
        virtual void yAxisMaxChanged(double value) = 0;
        /*!
         * \brief showHideLegend Отобразить/скрыть легенду по графикам
         */
        virtual void showHideLegend() = 0;
        /*!
         * \brief draw Рисует графики по данным, добавленным ранее
         */
        virtual void draw() = 0;

        /*!
         * \brief updateParams Применить параметры отображения графиков
         */
        virtual void applyParams() = 0;
        /*!
         * \brief params Получить список параметров для графиков
         */

        virtual QList<PlotParams*>& params() = 0;
        static CustomPlotBuilderPtr CreateInstance(); // функция-фабрика
};

#endif // ICUSTOMPLOTBUILDER_H
