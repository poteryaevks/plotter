#include "plotparams.h"

PlotParams::PlotParams(Qt::GlobalColor color, QCPScatterStyle::ScatterShape shape, QCPGraph::LineStyle ls, const QString &name, QWidget *parent)
    : QObject(parent)
     ,m_color(color)
     ,m_scShape(shape)
     ,m_ls(ls)
     ,m_plotName(name)
{

}

PlotParams::PlotParams(const PlotParams& params)
{
    auto color = params.getColor();
    auto ls = params.getLineStyle();
    auto sc_style = params.getScatterStyle();
    this->setColor(color);
    this->setLineStyle(ls);
    this->setScatterStyle(sc_style);
}

PlotParams::~PlotParams()
{

}
