#ifndef SPIRIT_PARSER_H
#define SPIRIT_PARSER_H

#include "parser_interface.h"

#include <boost/fusion/adapted.hpp>

template <typename Iterator>
class GrammarImpl;

//!
//! \brief The PvSpiritLineParser class
//!
class CsvProxyParser final : public IPvParser
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
    CsvProxyParser();

private:

    GrammarImplPtr grammar_;
};


//!
//! \brief The LineData struct
//!
struct CsvProxyLineData
{
    unsigned int id;
    double time;
    double b;
    double l;
    double h;
    std::vector<double> coord;
    std::vector<double> velocity;
    std::string state;
};

BOOST_FUSION_ADAPT_STRUCT(
        CsvProxyLineData,
        (unsigned int, id)
        (double, time)
        (double, b)
        (double, l)
        (double, h)
        (std::vector<double>, coord)
        (std::vector<double>, velocity)
        (std::string, state)
        )


#endif
