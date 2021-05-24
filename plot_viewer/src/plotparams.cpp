#include "plotparams.h"

PvPlotParams::PvPlotParams(Qt::GlobalColor color,
                           QCPScatterStyle::ScatterShape shape,
                           QCPGraph::LineStyle ls,
                           const QString &name,
                           QWidget *parent)
    : QObject(parent)
    ,m_customColor(false)
    ,m_plotName(name)
    ,m_color(color)
    ,m_scShape(shape)
    ,m_ls(ls)
{

}

PvPlotParams::PvPlotParams(const PvPlotParams& params)
{
    m_customColor = false;
    m_color = params.getColor();
    m_ls = params.getLineStyle();
    m_scShape = params.getScatterStyle();
}

void PvPlotParams::setColor(QColor color, bool customColor)
{
    m_customColor = customColor;
    m_color = color;
}
