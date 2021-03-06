#-------------------------------------------------
#
# Project created by QtCreator 2020-02-27T09:30:10
#
#-------------------------------------------------

QT += printsupport widgets xml sql
CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix: {
        CONFIG (debug, debug|release) {
                TARGET = PlotViewerd
        } else {
                TARGET = PlotViewer
        }
} else {
        TARGET = $$qtLibraryTarget(PlotViewer)
}

VERSION = 1.0.0

CONFIG += debug build_all

TARGET = PlotViewer
TEMPLATE = app

INCLUDEPATH += $$PWD/../

DEFINES += Library_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += header \
               $$PWD/../depends

SOURCES += \
        src/main.cpp \
        src/plotparams.cpp \
        src/paramsdialog.cpp \
        src/comboboxdelegate.cpp \
        src/mainwindow.cpp \
        src/ibasegraphbuilder.cpp \
        src/plot_factory.cpp \
        src/customplot.cpp \
        src/parserdialog.cpp \
        $$PWD/../depends/qcustomplot.cpp


HEADERS += \
        header/common_types.h \
        header/defaultparser.h \
        header/plotparams.h \
        header/paramsdialog.h \
        header/comboboxdelegate.h \
        header/mainwindow.h \
        header/ibasegraphbuilder.h \
        header/iplot.h \
        header/customplot.h \
        header/parserdialog.h \
        header/libraryglobal.h \
        header/plot_factory.h  \
        $$PWD/../depends/qcustomplot.h

FORMS += \
        ui/mainwindow.ui \
        ui/paramsdialog.ui \
        ui/parser_check.ui

DISTFILES +=

RESOURCES += res.qrc

TRANSLATIONS += plot_viewer_ru.ts



