#include "icustomplotbuilder.h"
#include "customplot.h"

CustomPlotBuilderPtr ICustomPlotBuilder::CreateInstance()
{
    return std::make_shared<CustomPlot>();
}
