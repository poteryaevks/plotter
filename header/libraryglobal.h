#ifndef LIBRARYGLOBAL_H
#define LIBRARYGLOBAL_H

#include <QtCore/qglobal.h>
#include "customplot.h"

#if defined(Library_LIBRARY)
#  define Library_EXPORT Q_DECL_EXPORT
#else
#  define Library_EXPORT Q_DECL_IMPORT
#endif

class CustomPlot;

#endif // LIBRARYGLOBAL_H
