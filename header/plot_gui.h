#ifndef plot_gui_H
#define plot_gui_H

#include <QtWidgets/QMainWindow>
#include "ui_plot_gui.h"
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

struct plot {
    graphs plot_graphs;
    QPoint max;
    QPoint min;
};

class plot_gui : public QMainWindow
{
    Q_OBJECT
public:
    plot_gui(QWidget *parent = 0);
    ~plot_gui();

    void setGraphColor(QCPGraph* g, Qt::GlobalColor color);
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
    QPair<double, double> processLine(const QString& line);

private:
    void loadFile(const QString &fileName);

    QVector<plotParams*> m_params;
    QVector<QFile*> files;
    QVector<QCPGraph*> graphs;

    functionStringDialog *pfdialog;
    Ui::plot_gui *ui;
    Calculator c;
    plot m_plot;
};

#endif // plot_gui_H
