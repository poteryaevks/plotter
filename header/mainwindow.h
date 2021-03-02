#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "plotparams.h"
#include "functionstringdialog.h"
#include "calc.h"

struct graph;
struct plot;
static const double step { 1 };
static const double points_number { 10000 };

using points = QVector<QPoint>;
using graphs =  QVector<graph>;

struct graph {
    points graph_points;
};

struct Plot {
    graphs plot_graphs;
    QPair<double, double> max;
    QPair<double, double> min;
};

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

//    void setGraphColor(QCPGraph* g, Qt::GlobalColor color);
    void setGraphColor(QCPGraph* g, QColor color);
    QCPGraph *addGraph(const plotParams *params);

public slots:
    void configurePlot();
    void functionExecute(const QString& expression);

private slots:
    void showHideLegend(bool p_onOff);
    void on_Xmin_valueChanged(double xmin);
    void on_Ymin_valueChanged(double ymin);
    void on_Xmax_valueChanged(double xmax);
    void on_Ymax_valueChanged(double ymax);
    void basicConfigurePlot();
    void clear();
    void showContextMenu(QPoint);
    void showParamsPlot();
    void openFile();

    bool eventFilter(QObject *object, QEvent *event);
//    QPair<double, double> processLine(const QString& line);
    QVector<QPair<double, double>> parserLine(const QString&);

private:
    void loadFile(const QString &fileName);

    QVector<plotParams*> m_params;
    QVector<QFile*> files;
    QVector<QCPGraph*> graphs;

    functionStringDialog *pfdialog;
    Ui::MainWindow *ui;
    Calculator c;
    Plot m_plot;
};

#endif // MAINWINDOW_H
