#ifndef ICUSTOMPLOTBUILDER_H
#define ICUSTOMPLOTBUILDER_H

#include "ibasegraphbuilder.h"
#include "common_types.h"

#include <memory>

#include <QWidget>

class PvPlotParams;

using namespace plot_viewer;

/*!
 * \brief The IPlot class Интерфейсный класс
 */
class IPlot : public IBaseGraphBuilder
{
public:
    virtual QWidget* widget() = 0;

    /*!
         * \brief title Заголовок графиков
         * \return
         */
    virtual QString title() const = 0;
    /*!
         * \brief setTitle Установить заголовок для графиков
         * \param title название заголовка
         */
    virtual void setTitle(const QString &title) = 0;
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
    virtual void addValues(const Graph &values) = 0;
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
    virtual void addGraph(const Graph &values, const QString &nameGraph = QString()) = 0;

    /*!
     * \brief addGraph
     * \param values
     * \param nameGraph
     */
    virtual void addGraph(Graph&& values, QString&& nameGraph = QString()) = 0;

    /*!
         * \brief addGraph
         * \param values
         * \param nameGraph
         */
    //    virtual void addGraph(const std::vector<std::pair<double, double>>& values, const QString &nameGraph = QString()) = 0;

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

    virtual QList<PvPlotParams*>& params() = 0;

    virtual ~IPlot() = default;
};

#endif // ICUSTOMPLOTBUILDER_H
