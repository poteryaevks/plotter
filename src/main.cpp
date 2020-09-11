#include "mainwindow.h"
#include "plot_gui.h"
#include <QApplication>


void setDarkPalette(QPalette &palette);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    plot_gui w;

    QPalette app_palette;
    setDarkPalette(app_palette);
    a.setStyle(QStyleFactory::create("Fusion"));

    w.show();
    return a.exec();
}


void setDarkPalette(QPalette &palette)
{
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(60, 52, 52));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::white);
}
