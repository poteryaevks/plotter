#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "functionstringdialog.h"
#include "calc.h"
#include "customplot.h"

static const double step { 1 };
static const double points_number { 10000 };

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void functionExecute(const QString& expression);

private slots:
    void showHideLegend(bool p_onOff);
    void on_Xmin_valueChanged(double xmin);
    void on_Ymin_valueChanged(double ymin);
    void on_Xmax_valueChanged(double xmax);
    void on_Ymax_valueChanged(double ymax);
    void clear();
    void showContextMenu(QPoint);
    void showParamsPlot();
    void openFile();

//    QPair<double, double> processLine(const QString& line);
    QVector<QPair<double, double>> parserLine(const QString&);

private:
    void loadFile(const QString &fileName);

    QVector<QFile*> files;

    functionStringDialog *pfdialog;
    Ui::MainWindow *ui;
    Calculator c;
    QWidget *m_graphWidget;
    CustomPlotBuilderPtr customPlotBuilderPtr;
};

#endif // MAINWINDOW_H
