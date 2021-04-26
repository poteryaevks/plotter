#include "plotparams.h"

PlotParams::PlotParams(Qt::GlobalColor color, QCPScatterStyle::ScatterShape shape, QCPGraph::LineStyle ls, const QString &name, QWidget *parent)
    : QObject(parent)
    ,m_customColor(false)
    ,m_plotName(name)
    ,m_color(color)
    ,m_scShape(shape)
    ,m_ls(ls)
{

}

PlotParams::PlotParams(const PlotParams& params)
{
    this->setColor(params.getColor());
    this->setLineStyle(params.getLineStyle());
    this->setScatterStyle(params.getScatterStyle());
}
