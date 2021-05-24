#ifndef BASE_H
#define BASE_H

#include "qcustomplot.h"
#include <QMap>


static const QVector<Qt::GlobalColor> defaultColors = {
    Qt::green
    ,Qt::red
    ,Qt::black
    ,Qt::yellow
    ,Qt::blue
};

static const QMap<QString, QCPGraph::LineStyle> defaultLs = {
    {"lsNone"       , QCPGraph::LineStyle::lsNone       }
    ,{"lsLine"      , QCPGraph::LineStyle::lsLine       }
    ,{"lsStepLeft"  , QCPGraph::LineStyle::lsStepLeft   }
    ,{"lsStepRight" , QCPGraph::LineStyle::lsStepRight  }
    ,{"lsStepCenter", QCPGraph::LineStyle::lsStepCenter }
    ,{"lsImpulse"   , QCPGraph::LineStyle::lsImpulse    }
};

static const QMap<QString, QCPScatterStyle::ScatterShape> defaultScatters = {
    {"ssNone" ,QCPScatterStyle::ScatterShape::ssNone}
    ,{"ssDot" ,QCPScatterStyle::ScatterShape::ssDot}
    ,{"ssCross" ,QCPScatterStyle::ScatterShape::ssCross}
    ,{"ssPlus" ,QCPScatterStyle::ScatterShape::ssPlus}
    ,{"ssCircle" ,QCPScatterStyle::ScatterShape::ssCircle}
    ,{"ssDisc" ,QCPScatterStyle::ScatterShape::ssDisc}
    ,{"ssSquare" ,QCPScatterStyle::ScatterShape::ssSquare}
    ,{"ssDiamond" ,QCPScatterStyle::ScatterShape::ssDiamond}
    ,{"ssStar" ,QCPScatterStyle::ScatterShape::ssStar}
    ,{"ssTriangle" ,QCPScatterStyle::ScatterShape::ssTriangle}
    ,{"ssTriangleInverted" ,QCPScatterStyle::ScatterShape::ssTriangleInverted}
    ,{"ssCrossSquare" ,QCPScatterStyle::ScatterShape::ssCrossSquare}
    ,{"ssPlusSquare" ,QCPScatterStyle::ScatterShape::ssPlusSquare}
    ,{"ssCrossCircle" ,QCPScatterStyle::ScatterShape::ssCrossCircle}
    ,{"ssPlusCircle" ,QCPScatterStyle::ScatterShape::ssPlusCircle}
    ,{"ssPeace" ,QCPScatterStyle::ScatterShape::ssPeace}
    ,{"ssPixmap" ,QCPScatterStyle::ScatterShape::ssPixmap}
    ,{"ssCustom" ,QCPScatterStyle::ScatterShape::ssCustom}
};

#endif // BASE_H

