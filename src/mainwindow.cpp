#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plotparams.h"
#include "paramsdialog.h"

#include <QFileDialog>
#include <QDir>
#include <QIcon>
#include <QFile>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pfdialog(new functionStringDialog)
{
    ui->setupUi(this);
//    ui->customPlot->installEventFilter(this);
    ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->cbLegend, SIGNAL(toggled(bool)), this, SLOT(showHideLegend(bool)));
    connect(ui->openAction, SIGNAL(triggered(bool)), this, SLOT(openFile()));
    connect(ui->clearAction, SIGNAL(triggered(bool)), this, SLOT(clear()));
    connect(ui->quitAction, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    connect(pfdialog, SIGNAL(sendString(const QString&)), this, SLOT(functionExecute(const QString&)));
    connect(ui->action_function, SIGNAL(triggered(bool)), pfdialog, SLOT(show()));
    connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    setWindowIcon(QIcon(":/graph.png"));
    Qt::WindowFlags w_flags = pfdialog->windowFlags();
    pfdialog->setWindowFlags(w_flags & ~Qt::WindowContextHelpButtonHint);
}

MainWindow::~MainWindow()
{  
    delete ui;
}

void MainWindow::showContextMenu(QPoint pos)
{
    QPoint globalPos;
    // если запрос от QAbstractScrollArea
    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = qobject_cast<QAbstractScrollArea*>(sender())->viewport()->mapToGlobal(pos);
    else
        globalPos = ui->customPlot->mapToGlobal(pos);

    QMenu menu;
    QAction *openParams = new QAction(QString::fromUtf8("Открыть параметры..."), this);
    menu.addAction(openParams);

    connect(openParams, SIGNAL(triggered()), this, SLOT(showParamsPlot()));

    menu.exec(globalPos);
}

void MainWindow::showParamsPlot()
{
    paramsDialog *dialog= new paramsDialog (&m_params, this);
    connect(dialog, SIGNAL(replot()), this, SLOT(configurePlot()));
    dialog->exec();
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    QWidget* widget = qobject_cast<QWidget*>(object);

    if (object != nullptr) {
    }
    return false;
}

void MainWindow::configurePlot()
{
    ui->customPlot->xAxis->setRange(m_plot.min.first, m_plot.max.first);
    ui->customPlot->yAxis->setRange(m_plot.min.second, m_plot.max.second);

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

void MainWindow::basicConfigurePlot()
{
    ui->customPlot->xAxis->setLabel("X");
    ui->customPlot->yAxis->setLabel("Y(X)");
}


void MainWindow::functionExecute(const QString& expression)
{
    clear();
    QString err_info;
    QString expressionName = QString("f(x) = %1").arg(expression);
    //
    QVector<double> Xs(points_number);
    QVector<double> Ys(points_number);
    if (files.isEmpty()) {
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

QCPGraph *MainWindow::addGraph(const plotParams *params)
{
    QCPGraph *pgraph = ui->customPlot->addGraph();
    plotParams *temp_params = const_cast<plotParams*>(params);

    if(pgraph != nullptr) {
        pgraph->setLineStyle(params->getLineStyle());
        setGraphColor(pgraph, QColor(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256)));
        pgraph->setScatterStyle(QCPScatterStyle(params->getScatterStyle()));
        graphs << pgraph;
        m_params << temp_params;
        return pgraph;
    }
    return nullptr;
}

void MainWindow::clear()
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

QVector<QPair<double, double>> MainWindow::parserLine(const QString& line)
{
    QVector<QPair<double, double>> data;
    QStringList list = line.simplified().split(QRegExp("[\\;\\s]+"));       // remove \n, \r and split
    int countColData = list.size() / 2;

    for (int i = 0; i < countColData; i++)
        data << QPair<double, double>(list[i * 2].toDouble(), list[i * 2 + 1].toDouble());

    return data;
}

//QPair<double, double> MainWindow::processLine(const QString& line)
//{
//    QString edit_line = line;
//    QStringList list = edit_line.simplified().split(";");       // remove \n, \r and split

//    if (list.size() < 2)
//        return QPair<double, double>();
//    else
//        return (QPair<double, double>(list[0].toDouble(), list[1].toDouble()));
//}

//void MainWindow::setGraphColor(QCPGraph* g, Qt::GlobalColor color)
//{
//    if (g == nullptr)
//        return;

//    auto pen = g->pen();
//    pen.setColor(color);
//    g->setPen(pen);
//}

void MainWindow::setGraphColor(QCPGraph* g, QColor color)
{
    if (g == nullptr)
        return;

    auto pen = g->pen();
    pen.setColor(color);
    g->setPen(pen);
}

void MainWindow::showHideLegend(bool /*p_onOff*/)
{
    ui->customPlot->legend->setVisible(ui->cbLegend->isChecked());
    ui->customPlot->replot();
}

void MainWindow::on_Xmin_valueChanged(double xmin)
{
    m_plot.min.first = xmin;
    configurePlot();
    ui->customPlot->replot();
}

void MainWindow::on_Ymin_valueChanged(double ymin)
{
    m_plot.min.second = ymin;
    configurePlot();
    ui->customPlot->replot();
}

void MainWindow::on_Xmax_valueChanged(double xmax)
{
    m_plot.max.first = xmax;
    configurePlot();
    ui->customPlot->replot();
}

void MainWindow::on_Ymax_valueChanged(double ymax)
{
    m_plot.max.second = ymax;
    configurePlot();
    ui->customPlot->replot();
}

void MainWindow::openFile()
{
    auto fileName = QFileDialog::getOpenFileName(this, QString::fromUtf8("Открытие файла"), QDir::currentPath(), QString::fromUtf8("Текстовые файлы (*.txt *.csv)"));

    if (!fileName.isEmpty())
        loadFile(fileName);
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile *pfile = new QFile(fileName);

    if (!pfile->open(QFile::OpenModeFlag::ReadOnly)) {
        delete pfile;
        return;
    }
    //
    QVector<double> Xs(points_number);
    QVector<double> Ys(points_number);
//    int i = 0;

//    while (!pfile->atEnd()) {
//        QString line = pfile->readLine();
//        QPair<double, double> pair = processLine(line);
//        Xs[i] = pair.first;
//        Ys[i] = pair.second;
//        i++;
//    }
//    pfile->reset();
//    //
//    plotParams *params = new plotParams(defaultParams);
//    params->setPlotName(fileName);
//    //
//    auto pgraph= addGraph(params);
//    pgraph->setData(Xs, Ys);
//    ui->customPlot->replot();
//    files << pfile;
    QList<QVector<QPair<double, double>>> dataGraphs;

    while (!pfile->atEnd()) {
        QString line = pfile->readLine();
        QVector<QPair<double, double>> data = parserLine(line);

        QVector<QPair<double, double>>::iterator it;

        int i = 0;
        QVector<QPair<double, double>> dataGraph;

        for (it = data.begin(); it != data.end(); ++it) {
            if (dataGraphs.size() < data.size()) {
                dataGraph.clear();
                dataGraph.append(*it);
                dataGraphs.append(dataGraph);
            }
            else {
                dataGraph = dataGraphs.value(i);
                dataGraph.append(*it);
                dataGraphs[i] = dataGraph;
            }
            i++;
        }
//        Xs[i] = pair.first;
//        Ys[i] = pair.second;
//        i++;
    }
    pfile->reset();
    //
    double xMax = 0;
    double yMax = 0;
    double xMin = 0;
    double yMin = 0;
    for (int i = 0; i < dataGraphs.size(); i++) {
        QVector<QPair<double, double>>::const_iterator it(dataGraphs.at(i).begin());
        QVector<double> x;
        QVector<double> y;

        while (it != dataGraphs.at(i).end()) {
            xMax = qMax(xMax, (*it).first);
            yMax = qMax(yMax, (*it).second);
            if (xMin == 0|| yMin == 0) {
                xMin = xMax;
                yMin = yMax;
            }
            xMin = qMin(xMin, (*it).first);
            yMin = qMin(yMin, (*it).second);
            x << (*it).first;
            y << (*it).second;
            ++it;
        }

        plotParams *params = new plotParams(defaultParams);
        params->setPlotName(fileName);
        //
        auto pgraph= addGraph(params);
        pgraph->setData(x, y);
//        break;
    }
    ui->customPlot->xAxis->setRange(xMin, xMax);
    ui->customPlot->yAxis->setRange(yMin, yMax);
    ui->customPlot->replot();
//    files << pfile;
}
