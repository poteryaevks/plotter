#include "plot_factory.h"


namespace plot_viewer {

almaz::misc::IFactory<IPlot, plot_viewer::ePlotType, Creator> FACTORY;

}
