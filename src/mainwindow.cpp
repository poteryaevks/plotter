#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plotparams.h"
#include "paramsdialog.h"
#include "customplot.h"

#include <QFileDialog>
#include <QDir>
#include <QIcon>
#include <QFile>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , pfdialog(new functionStringDialog)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    ui->customPlotLayout->setContextMenuPolicy(Qt::CustomContextMenu);

    customPlotBuilderPtr = CustomPlot::CreateInstance();
    m_graphWidget = customPlotBuilderPtr->widget();
    m_graphWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QHBoxLayout  *layout = new QHBoxLayout(ui->frame);
    ui->frame->setLayout(layout);
    layout->addWidget(m_graphWidget);

    connect(ui->cbLegend, SIGNAL(toggled(bool)), this, SLOT(showHideLegend(bool)));
    connect(ui->openAction, SIGNAL(triggered(bool)), this, SLOT(openFile()));
    connect(ui->clearAction, SIGNAL(triggered(bool)), this, SLOT(clear()));
    connect(ui->quitAction, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    connect(pfdialog, SIGNAL(sendString(const QString&)), this, SLOT(functionExecute(const QString&)));
    connect(ui->action_function, SIGNAL(triggered(bool)), pfdialog, SLOT(show()));
    connect(m_graphWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    setWindowIcon(QIcon(":/graph.png"));
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
        globalPos = m_graphWidget->mapToGlobal(pos);

    QMenu menu;
    QAction *openParams = new QAction(QString::fromUtf8("Открыть параметры..."), this);
    menu.addAction(openParams);

    connect(openParams, SIGNAL(triggered()), this, SLOT(showParamsPlot()));

    menu.exec(globalPos);
}

void MainWindow::showParamsPlot()
{
    if (customPlotBuilderPtr != nullptr) {
        ParamsDialog *dialog= new ParamsDialog(&customPlotBuilderPtr->params(), this);

        if (dialog->exec() == QDialog::Accepted) {
            customPlotBuilderPtr->applyParams();
        }
    }
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

        for (std::size_t i{0}; i < 100; ++i) {
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

//    customPlotBuilderPtr->addGraph();

    if(!err_info.isEmpty())
        QMessageBox::warning(m_graphWidget, "Error" , err_info);
}

void MainWindow::clear()
{
    foreach(QFile* pfile, files) {
        if(pfile != nullptr)
            delete pfile;
    }

    customPlotBuilderPtr->clearViewer();

    files.clear();
}

QVector<QPair<double, double>> MainWindow::parserLine(const QString& line)
{
    QVector<QPair<double, double>> data;
    QStringList list = line.simplified().replace(",", ".").split(QRegExp("[\\;\\s]+"));       // remove \n, \r and split
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

void MainWindow::showHideLegend(bool /*p_onOff*/)
{
    customPlotBuilderPtr->showHideLegend();
}

void MainWindow::on_Xmin_valueChanged(double xmin)
{
    customPlotBuilderPtr->xAxisMinChanged(xmin);
}

void MainWindow::on_Ymin_valueChanged(double ymin)
{
    customPlotBuilderPtr->yAxisMinChanged(ymin);
}

void MainWindow::on_Xmax_valueChanged(double xmax)
{
    customPlotBuilderPtr->xAxisMaxChanged(xmax);
}

void MainWindow::on_Ymax_valueChanged(double ymax)
{
    customPlotBuilderPtr->yAxisMaxChanged(ymax);
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

    for (int i = 0; i < dataGraphs.size(); i++) {
        QVector<QPair<double, double>>::const_iterator it(dataGraphs.at(i).begin());
        QVector<double> x;
        QVector<double> y;

        while (it != dataGraphs.at(i).end()) {
            x << (*it).first;
            y << (*it).second;
            ++it;
        }

        customPlotBuilderPtr->addGraph(dataGraphs.at(i), fileName);
    }
//    files << pfile;
}
