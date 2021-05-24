#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <QString>
#include <QPair>
#include <QVector>

#include <memory>
#include <functional>


namespace plot_viewer {

using Point = QPair<double, double>;
using LineRawData = QVector<Point>;
using Graph = QVector<Point>;
using GraphRawData = QVector<LineRawData>;

}


#endif

