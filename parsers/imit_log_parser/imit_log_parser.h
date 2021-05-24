#ifndef SPIRIT_PARSER_H
#define SPIRIT_PARSER_H

#include "parser_interface.h"

#include <boost/fusion/adapted.hpp>

template <typename Iterator>
class GrammarImpl;

//!
//! \brief The PvSpiritLineParser class
//!
class ImitLogParser final : public IPvParser
{
    using IteratorType = std::string::iterator;
    using GrammarImplPtr = std::unique_ptr<GrammarImpl<IteratorType>>;

public:

    //!
    //! \brief parse
    //! \return
    //!
    virtual plot_viewer::LineRawData parse(QString);

    //!
    //! \brief CsvProxyParser
    //!
    ImitLogParser();

private:

    GrammarImplPtr grammar_;
};


//!
//! \brief The LineData struct
//!
struct ImitLogLineData
{
    double time;
    unsigned int id;
    double b;
    double l;
    double h;
};

BOOST_FUSION_ADAPT_STRUCT(
        ImitLogLineData,
        (double, time)
        (unsigned int, id)
        (double, b)
        (double, l)
        (double, h)
        )


#endif
