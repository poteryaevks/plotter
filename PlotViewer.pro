#-------------------------------------------------
#
# Project created by QtCreator 2020-02-27T09:30:10
#
#-------------------------------------------------

QT += printsupport widgets xml sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlotViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += header

SOURCES += \
        src/main.cpp \
        src/mainwindow.cpp \
        src/qcustomplot.cpp \
        src/plot_gui.cpp \
        src/calc.cpp \
        src/plotparams.cpp \
        src/paramsdialog.cpp \
        src/comboboxdelegate.cpp \
        src/base.cpp \
        src/functionstringdialog.cpp

HEADERS += \
    header/mainwindow.h \
    header/qcustomplot.h \
    header/plot_gui.h \
    header/calc.h \
    header/plotparams.h \
    header/paramsdialog.h \
    header/comboboxdelegate.h \
    header/base.h \
    header/functionstringdialog.h

FORMS += \
    ui/mainwindow.ui \
    ui/plot_gui.ui \
    ui/paramsdialog.ui \
    ui/functionstringdialog.ui

DISTFILES +=

RESOURCES += \
    res/res.qrc
