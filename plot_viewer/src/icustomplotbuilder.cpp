#include "icustomplotbuilder.h"
#include "customplot.h"

CustomPlotBuilderPtr IPlot::CreateInstance()
{
    return std::make_shared<PvPlot>();
}
