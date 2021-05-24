#pragma once

#include "parser_interface.h"

//!
//! \brief The DefaultParser class
//!
class DefaultParser final : public IPvParser
{

public:

    //!
    //! \brief parse
    //! \return
    //!
    virtual plot_viewer::LineRawData parse(QString);

    DefaultParser() = default;
};




