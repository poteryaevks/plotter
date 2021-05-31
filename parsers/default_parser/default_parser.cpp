#include "default_parser.h"

#include <QStringList>

plot_viewer::LineRawData DefaultParser::parse(QString line)
{
    plot_viewer::LineRawData rawData;

    QStringList list = line.simplified().
            replace(",", ".").
            split(QRegExp("[\\;\\s]+")); // remove \n, \r and split

    for (int i = 0; i < (list.size() / 2); i++){

        rawData.push_back(
                    plot_viewer::Point(list[i * 2].toDouble(),
                    list[i * 2 + 1].toDouble())
                );
    }

    return std::move(rawData);
}
