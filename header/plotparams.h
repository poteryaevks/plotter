#ifndef PLOTPARAMS_H
#define PLOTPARAMS_H

#include <QDialog>
#include "qcustomplot.h"

class PlotParams : public QObject
{
    Q_OBJECT
public:
    explicit PlotParams(QWidget *parent = 0) = delete;
    PlotParams(Qt::GlobalColor color, QCPScatterStyle::ScatterShape shape, QCPGraph::LineStyle ls, const QString &name = "plot", QWidget *parent = nullptr);
    PlotParams(const PlotParams& params);
    ~PlotParams() {}

public:
    inline QColor getColor() const { return m_color; }
    inline QCPScatterStyle::ScatterShape getScatterStyle() const { return m_scShape; }
    inline QCPGraph::LineStyle getLineStyle() const { return m_ls; }
    inline QString getPlotName() const { return m_plotName; }

    inline void setColor(QColor color) { m_color = color; }
    inline void setScatterStyle(QCPScatterStyle::ScatterShape scShape) { m_scShape = scShape; }
    inline void setLineStyle(QCPGraph::LineStyle ls) { m_ls = ls; }
    inline void setPlotName(const QString &name) { m_plotName = name; }

    inline bool isCustomColor() { return m_customColor; }

private:
    bool m_customColor;
    QString m_plotName;
    QColor m_color;
    QCPScatterStyle::ScatterShape m_scShape;
    QCPGraph::LineStyle m_ls;
};

Q_DECLARE_TYPEINFO(PlotParams, Q_MOVABLE_TYPE);

const static PlotParams defaultParams(Qt::black, QCPScatterStyle::ssDot, QCPGraph::lsLine);

#endif // PLOTPARAMS_H
