#pragma once

#include "../plot_viewer/header/common_types.h"

//!
//! \brief The IPvParser class
//!
class IPvParser
{
public:

    //!
    //! \brief parse
    //! \return
    //!
    virtual plot_viewer::Graph parse(QString) = 0;

    virtual ~IPvParser(){}
};


#ifdef WIN32

#else

#define __declspec(a)

#endif

#define DeclareLoadableParser(parser) \
    extern "C" __declspec(dllexport) parser* loadParser() { return new parser(); }
