#ifndef PLOTPARAMS_H
#define PLOTPARAMS_H

#include <QDialog>
#include "qcustomplot.h"

class PvPlotParams : public QObject
{
    Q_OBJECT
public:
    explicit PvPlotParams(QWidget *parent = 0) = delete;
    PvPlotParams(Qt::GlobalColor color, QCPScatterStyle::ScatterShape shape, QCPGraph::LineStyle ls, const QString &name = "plot", QWidget *parent = nullptr);
    PvPlotParams(const PvPlotParams& params);
    ~PvPlotParams() {}

public:
    inline QColor getColor() const { return m_color; }
    inline QCPScatterStyle::ScatterShape getScatterStyle() const { return m_scShape; }
    inline QCPGraph::LineStyle getLineStyle() const { return m_ls; }
    inline QString getPlotName() const { return m_plotName; }
    inline QString getParser() const { return m_parser; }

    void setColor(QColor color, bool customColor = false);
    inline void setScatterStyle(QCPScatterStyle::ScatterShape scShape) { m_scShape = scShape; }
    inline void setLineStyle(QCPGraph::LineStyle ls) { m_ls = ls; }
    inline void setPlotName(const QString &name) { m_plotName = name; }
    inline void setParser(const QString &parser) { m_parser = parser; }

    inline bool isCustomColor() { return m_customColor; }

private:

    bool m_customColor;
    QString m_plotName;
    QColor m_color;
    QCPScatterStyle::ScatterShape m_scShape;
    QCPGraph::LineStyle m_ls;
    QString m_parser;
};

Q_DECLARE_TYPEINFO(PvPlotParams, Q_MOVABLE_TYPE);

const static PvPlotParams defaultParams(Qt::black, QCPScatterStyle::ssDot, QCPGraph::lsLine);

#endif // PLOTPARAMS_H
