#ifndef PLOTPARAMS_H
#define PLOTPARAMS_H

#include <QDialog>

#include "common_types.h"
#include "qcustomplot.h"

//!
//! \brief The PvPlotParams class
//!
class PvPlotParams
{

public:

    PvPlotParams(Qt::GlobalColor color,
                 QCPScatterStyle::ScatterShape shape,
                 QCPGraph::LineStyle ls,
                 const QString &name = "plot");

    PvPlotParams(const PvPlotParams&) = default;
    PvPlotParams(PvPlotParams&&) = default;

    ~PvPlotParams() {}

public:

    QColor getColor() const { return color_; }
    QCPScatterStyle::ScatterShape getScatterStyle() const { return shape_; }
    QCPGraph::LineStyle getLineStyle() const { return ls_; }
    QString getPlotName() const { return name_; }

    void setColor(QColor color, bool customColor = false);
    void setScatterStyle(QCPScatterStyle::ScatterShape scShape) { shape_ = scShape; }
    void setLineStyle(QCPGraph::LineStyle ls) { ls_ = ls; }
    void setPlotName(const QString &name) { name_ = name; }

    bool isCustomColor() { return bCustomColor_; }

private:

    bool bCustomColor_;
    QString name_;
    QColor color_;
    QCPScatterStyle::ScatterShape shape_;
    QCPGraph::LineStyle ls_;
};

extern PvPlotParams DEFAULT_PLOT_PARAMS;

#endif // PLOTPARAMS_H
