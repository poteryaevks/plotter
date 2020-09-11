#include "plot_gui.h"
#include "plotparams.h"
#include "paramsdialog.h"

#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QStringList>


static QTextDecoder qtd(QTextCodec::codecForName("windows-1251"));
static QTextCodec* tcUtf8 = QTextCodec::codecForName("UTF-8");

plot_gui::plot_gui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::plot_gui)
    , pfdialog(new functionStringDialog)
{
    ui->setupUi(this);
    ui->customPlot->installEventFilter(this);

    QObject::connect(ui->cbLegend, SIGNAL(toggled(bool)), this, SLOT(showHideLegend(bool)));
    QObject::connect(ui->action_openfile, SIGNAL(triggered(bool)), this, SLOT(loadFile()));
    QObject::connect(ui->action_clear, SIGNAL(triggered(bool)), this, SLOT(clear()));
    QObject::connect(ui->action_clear, SIGNAL(triggered(bool)), this, SLOT(clear()));
    QObject::connect(ui->action_exit, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    QObject::connect(pfdialog, SIGNAL(sendString(const QString&)), this, SLOT(functionExecute(const QString&)));
    QObject::connect(ui->action_function, SIGNAL(triggered(bool)), pfdialog, SLOT(show()));

    setWindowIcon(QIcon(":/graph.png"));
    Qt::WindowFlags w_flags = pfdialog->windowFlags();
    pfdialog->setWindowFlags(w_flags & ~Qt::WindowContextHelpButtonHint);
}

plot_gui::~plot_gui()
{  
    delete ui;
}


bool plot_gui::eventFilter(QObject *object, QEvent *event)
{
    QWidget* widget = qobject_cast<QWidget*>(object);
    if(object != nullptr) {
        if(event->type() == QEvent::ContextMenu
                && !graphs.isEmpty()) {
            QMenu* menu = new QMenu(widget);
            QAction *params = new QAction(qtd.toUnicode("Открыть параметры.."), menu);
            paramsDialog *dialog= new paramsDialog (&m_params, this);
            connect(params, SIGNAL(triggered()), dialog, SLOT(exec()));
            connect(dialog, SIGNAL(replot()), this, SLOT(configurePlot()));
            menu->addAction(params);
            menu->popup(QCursor::pos());
        }
    }
    return false;
}

void plot_gui::configurePlot()
{
    ui->customPlot->xAxis->setRange(m_plot.min.rx(), m_plot.max.rx());
    ui->customPlot->yAxis->setRange(m_plot.min.ry(), m_plot.max.ry());

    std::size_t i{0};
    foreach(QCPGraph* pgraph, graphs) {
        auto ls = m_params[i]->getLineStyle();
        auto sc = m_params[i]->getScatterStyle();
        auto color = m_params[i++]->getColor();
        pgraph->setLineStyle(ls);
        setGraphColor(pgraph, color);
        pgraph->setScatterStyle(QCPScatterStyle(sc));
    }
    ui->customPlot->replot();
}

void plot_gui::basicConfigurePlot()
{
    ui->customPlot->xAxis->setLabel(qtd.toUnicode("X"));
    ui->customPlot->yAxis->setLabel(qtd.toUnicode("Y(X)"));
}


void plot_gui::functionExecute(const QString& expression)
{
    clear();
    QString err_info;
    QString expressionName = QString("f(x) = %1").arg(expression);
    //
    QVector<double> Xs(points_number);
    QVector<double> Ys(points_number);
    if(files.isEmpty()) {
        QString function_x(expression);
        if (function_x.isEmpty())
            return;
        //
        for(std::size_t i{0}; i < 100; ++i) {
            QString func = function_x;
            QString str_i = QString::number(i);
            func.replace("x", str_i);
            Xs[i] = i;
            try {
                Ys[i] = c.Calc(func.toStdString());
            }
            catch (const std::exception &ex) {
                err_info = ex.what();
                break;
            }
        }
    }
    plotParams *params = new plotParams(defaultParams);

    params->setPlotName(expressionName);
    auto graph = addGraph(params);
    if(graph != nullptr) {
        graph->setData(Xs, Ys);
        ui->customPlot->replot();
    }
    if(!err_info.isEmpty())
        QMessageBox::warning(ui->customPlot, "Error" , err_info);
}

QCPGraph *plot_gui::addGraph(const plotParams *params)
{
    QCPGraph * pgraph = ui->customPlot->addGraph();
    plotParams *temp_params = const_cast<plotParams*>(params);
    if(pgraph != nullptr) {
        pgraph->setLineStyle(params->getLineStyle());
        setGraphColor(pgraph, params->getColor());
        pgraph->setScatterStyle(QCPScatterStyle(params->getScatterStyle()));
        graphs << pgraph;
        m_params << temp_params;
        return pgraph;
    }
    return nullptr;
}

void plot_gui::clear()
{
    foreach(QFile* pfile, files) {
        if(pfile != nullptr)
            delete pfile;
    }
    foreach(plotParams* pparams, m_params) {
        if(pparams != nullptr)
            delete pparams;
    }
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();
    graphs.clear();
    files.clear();
    m_params.clear();
}

QPair<double, double> plot_gui::processLine(const QString& line)
{
    QString edit_line = line;
    edit_line.remove("\n");
    edit_line.remove("\r");
    QStringList list = edit_line.split(";");
    if(list.size() < 2)
        return QPair<double, double>();
    else
        return (QPair<double, double>(list[0].toDouble(), list[1].toDouble()));
}

void plot_gui::setGraphColor(QCPGraph* g, Qt::GlobalColor color)
{
    if(g == nullptr) return;
    auto pen = g->pen();
    pen.setColor(color);
    g->setPen(pen);
}

void plot_gui::showHideLegend(bool /*p_onOff*/)
{
    ui->customPlot->legend->setVisible(ui->cbLegend->isChecked());
    ui->customPlot->replot();
}

void plot_gui::on_Xmin_valueChanged(double xmin)
{
    m_plot.min.setX(xmin);
    configurePlot();
    ui->customPlot->replot();
}

void plot_gui::on_Ymin_valueChanged(double ymin)
{
    m_plot.min.setY(ymin);
    configurePlot();
    ui->customPlot->replot();
}

void plot_gui::on_Xmax_valueChanged(double xmax)
{
    m_plot.max.setX(xmax);
    configurePlot();
    ui->customPlot->replot();
}


void plot_gui::on_Ymax_valueChanged(double ymax)
{
    m_plot.max.setY(ymax);
    configurePlot();
    ui->customPlot->replot();
}

void plot_gui::on_openfile_released()
{
    loadFile();
}

void plot_gui::loadFile()
{
    auto fileName = QFileDialog::getOpenFileName(this, qtd.toUnicode("Открытие файла"), QDir::currentPath(), tr("Data Files (*.csv)"));
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void plot_gui::loadFile(const QString &fileName)
{
    QFile *pfile = new QFile(fileName);
    if(!pfile->open(QFile::OpenModeFlag::ReadOnly)) {
        delete pfile;
        return;
    }
    //
    QVector<double> Xs(points_number);
    QVector<double> Ys(points_number);
    //
    int i { 0 };
    while (!pfile->atEnd()) {
        QString line = pfile->readLine();
        QPair<double, double> pair = processLine(line);
        Xs[i] = pair.first;
        Ys[i++] = pair.second;
    }
    pfile->reset();
    //
    plotParams *params = new plotParams(defaultParams);
    params->setPlotName(fileName);
    //
    auto pgraph= addGraph(params);
    pgraph->setData(Xs, Ys);
    ui->customPlot->replot();
    files << pfile;
}
