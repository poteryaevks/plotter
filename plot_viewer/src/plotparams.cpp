#include "plotparams.h"

PvPlotParams::PvPlotParams(Qt::GlobalColor color,
                           QCPScatterStyle::ScatterShape shape,
                           QCPGraph::LineStyle ls,
                           const QString &name)
    : bCustomColor_(false)
    , name_(name)
    , color_(color)
    , shape_(shape)
    , ls_(ls)
{
}

void PvPlotParams::setColor(QColor color, bool customColor)
{
    bCustomColor_ = customColor;
    color_ = color;
}


PvPlotParams DEFAULT_PLOT_PARAMS(Qt::black, QCPScatterStyle::ssDot, QCPGraph::lsLine);
