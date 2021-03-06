#include "csv_proxy_parser.h"

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

#include <boost/format.hpp>

//namespace {

namespace qi = boost::spirit::qi;

template <typename Iterator>
class GrammarImpl
        : public qi::grammar<Iterator, CsvProxyLineData()>
{
    public:

    GrammarImpl()
        : GrammarImpl::base_type(start)
    {
        using boost::spirit::standard_wide::space;
        using boost::spirit::standard_wide::char_;

        string_value %= qi::raw[+(char_ - (space | '|'))];

        vec3 %=
                qi::double_ >> qi::omit[*(space | '|')]
                >> qi::double_ >> qi::omit[*(space | '|')]
                >> qi::double_ >> qi::omit[*(space | '|')];

        start %=
                qi::omit[*(space | '|')]
                >> qi::int_     >> qi::omit[*(space | '|')]  // id
                >> qi::double_  >> qi::omit[*(space | '|')]  // время
                >> qi::double_  >> qi::omit[*(space | '|' | "n.latt" | "s.latt" | "n,latt" | "s,latt")]  // b
                >> qi::double_  >> qi::omit[*(space | '|' | "e.long" | "w.long" | "e,long" | "w,long")]  // l
                >> qi::double_  >> qi::omit[*(space | '|')]  // h
                >> vec3         >> qi::omit[*(space | '|')]  // xyz
                >> vec3         >> qi::omit[*(space | '|')]  // Vxyz
                >> string_value >> qi::omit[*(space | '|')]; // state

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

    qi::rule<Iterator, std::string()> string_value;
    qi::rule<Iterator, std::vector<double>()> vec3;
    qi::rule<Iterator, CsvProxyLineData()> start;
};

//}

using namespace plot_viewer;


CsvProxyParser::CsvProxyParser()
    : grammar_(std::make_unique<GrammarImpl<IteratorType>>()),
      id_(std::numeric_limits<int>::min())
{
}

CsvProxyParser::~CsvProxyParser()
{
}

LineRawData CsvProxyParser::parse(QString line)
{
    LineRawData rawData;
    CsvProxyLineData data;

    std::string stdLine{ line.toStdString() };

    std::replace(
                std::begin(stdLine),
                std::end(stdLine),
                ',', '.'
                );

    auto success = qi::parse(std::begin(stdLine),
                             std::end(stdLine),
                             *(grammar_.get()),
                             data);

    if(!success){

        auto err = boost::format("Unable to parse line: \"%s\"") % line.toStdString();
        throw std::runtime_error(err.str());
    }


    if(id_ == std::numeric_limits<int>::min())
        id_ = data.id;

    if(id_ == data.id){

        rawData.push_back(Point(data.time, data.b));
        rawData.push_back(Point(data.time, data.l));
        rawData.push_back(Point(data.time, data.h));

        rawData.push_back(Point(data.time, data.coord[0]));
        rawData.push_back(Point(data.time, data.coord[1]));
        rawData.push_back(Point(data.time, data.coord[2]));

        rawData.push_back(Point(data.time, data.velocity[0]));
        rawData.push_back(Point(data.time, data.velocity[1]));
        rawData.push_back(Point(data.time, data.velocity[2]));
    }

    return std::move(rawData);
}
