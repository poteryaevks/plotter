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
    m_customColor = false;
    m_color = params.getColor();
    m_ls = params.getLineStyle();
    m_scShape = params.getScatterStyle();
}

void PlotParams::setColor(QColor color)
{
    m_customColor = true;
    m_color = color;
}
