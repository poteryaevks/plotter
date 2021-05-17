#include <QApplication>
#include "mainwindow.h"

//void setDarkPalette(QPalette &palette);

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;

//    QPalette app_palette;
//    setDarkPalette(app_palette);
    app.setStyle(QStyleFactory::create("Fusion"));

    QTranslator translator;
    // look up e.g. :/translations/myapp_de.qm
    if (translator.load(QLocale(), QLatin1String("PlotViewer"), QLatin1String("_"), QLatin1String(":/translations/res")))
        app.installTranslator(&translator);
    qDebug() << QLatin1String("PlotViewer") + QLatin1String("_") + QLocale().name();

    mainWindow.show();
    return app.exec();
}

//void setDarkPalette(QPalette &palette)
//{
//    palette.setColor(QPalette::Window, QColor(53, 53, 53));
//    palette.setColor(QPalette::WindowText, Qt::white);
//    palette.setColor(QPalette::Base, QColor(60, 52, 52));
//    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
//    palette.setColor(QPalette::ToolTipBase, Qt::white);
//    palette.setColor(QPalette::Text, Qt::white);
//    palette.setColor(QPalette::Button, QColor(53, 53, 53));
//    palette.setColor(QPalette::ButtonText, Qt::white);
//    palette.setColor(QPalette::Link, QColor(42, 130, 218));
//    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
//    palette.setColor(QPalette::HighlightedText, Qt::white);
//}
