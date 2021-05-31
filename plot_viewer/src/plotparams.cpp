#include "plotparams.h"

void PvPlotParams::setColor(QColor color, bool customColor)
{
    bCustomColor_ = customColor;
    color_ = color;
}


PvPlotParams DEFAULT_PLOT_PARAMS(Qt::black,
                                 QCPScatterStyle::ssDot,
                                 QCPGraph::lsLine,
                                 "default");
