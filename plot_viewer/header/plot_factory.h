#pragma once

#include <factory.h>

#include "iplot.h"
#include "common_types.h"

namespace plot_viewer {

using Creator = std::function<std::shared_ptr<IPlot>()>;
extern almaz::misc::IFactory<IPlot, plot_viewer::ePlotType, Creator> FACTORY;


}
