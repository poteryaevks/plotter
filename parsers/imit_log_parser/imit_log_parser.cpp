#include "imit_log_parser.h"

#include <QStringList>
#include <QDebug>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>


#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>

//namespace {

namespace qi = boost::spirit::qi;

template <typename Iterator>
class GrammarImpl
        : public qi::grammar<Iterator, ImitLogLineData()>
{
    public:

    GrammarImpl()
        : GrammarImpl::base_type(start)
    {
        using boost::spirit::standard_wide::space;
        using boost::spirit::standard_wide::char_;

        start %=
                qi::omit[*(qi::char_- '>')] >> '>'
                                            >> qi::omit[*(qi::space)]
                                            >> qi::double_ >> ":" >> qi::omit[*(qi::space)]
                                            >> "ID" >> qi::omit[*(qi::space)]
                                            >> qi::int_ >> qi::omit[*(qi::char_- '[')] >> '['
                                            >> qi::double_ >> qi::omit[*(qi::space | "г.в.д." | "г.з.д.")]
                                            >> qi::double_ >> qi::omit[*(qi::space | "г.c.ш." | "г.ю.ш.")]
                                            >> qi::double_ >> qi::omit[*(qi::space | "км"     | ']')];
//                                            >> qi::double_ >> qi::omit[*(qi::space | "[км/c]")];

        qi::on_error<qi::fail>(start,
                               std::cout
                               << "Error! Expecting "
                               << qi::_4
                               << " here: '"
                               << boost::phoenix::construct<std::string>(qi::_3, qi::_2)
                               << "'\n"
                               );
    }

    private:

    qi::rule<Iterator, ImitLogLineData()> start;
};

//}

using namespace plot_viewer;


ImitLogParser::ImitLogParser()
    : grammar_(std::make_unique<GrammarImpl<IteratorType>>())
{
}

LineRawData ImitLogParser::parse(QString line)
{
    LineRawData rawData;
    ImitLogLineData data;

    std::string stdLine{ line.toStdString() };

    auto success = qi::parse(std::begin(stdLine),
                             std::end(stdLine),
                             *(grammar_.get()),
                             data);

    if(!success)
        throw std::runtime_error("Unable to parse line");

    rawData.push_back(Point(data.time, data.b));
    rawData.push_back(Point(data.time, data.l));
    rawData.push_back(Point(data.time, data.h));
    return rawData;
}
