#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "functionstringdialog.h"
#include "calc.h"
#include "customplot.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    using Point = QPair<double, double>;
    using Graph = QVector<Point>;
    using FilePtr = std::unique_ptr<QFile>;


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
    void showPrint();
    void showPrintPreview();
    void print(QPrinter *printer);

private:

    QVector<Point> parseLine(const QString&);
    void loadFile(const QString &fileName);

private:

    QVector<QFile*> files;
    functionStringDialog *pfdialog;
    Ui::MainWindow *ui;
    Calculator calc_;
    QWidget *m_graphWidget;
    CustomPlotBuilderPtr customPlotBuilderPtr;
};

#endif // MAINWINDOW_H
