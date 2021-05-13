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

static const double step { 1 };
static const double POINTS_NUMBER { 10000 };

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
    connect(ui->printAction, SIGNAL(triggered(bool)), this, SLOT(print()));
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
        ParamsDialog dialog(&customPlotBuilderPtr->params(), this);

        if (dialog.exec() == QDialog::Accepted) {
            customPlotBuilderPtr->applyParams();
        }
    }
}

void MainWindow::functionExecute(const QString& expression)
{
    clear();

    QString err_info;
    Graph graph;

    if (files.isEmpty()) {

        QString function_x(expression);
        if (function_x.isEmpty())
            return;

        for (std::size_t x{ 0 }; x < 500; ++x) {

            QString func = function_x;
            QString str_i = QString::number(x);
            func.replace("x", str_i);

            try {
                auto y = calc_.Calc(func.toStdString());
                graph << Point(x, y);
            }
            catch (const std::exception &ex) {
                err_info = ex.what();
                break;
            }
        }

        customPlotBuilderPtr->addGraph(graph, QString("User function"));
    }

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

QVector<QPair<double, double>> MainWindow::parseLine(const QString& line)
{
    QVector<QPair<double, double>> data;
    QStringList list = line.simplified().replace(",", ".").split(QRegExp("[\\;\\s]+"));       // remove \n, \r and split
    int countColData = list.size() / 2;

    for (int i = 0; i < countColData; i++)
        data << Point(list[i * 2].toDouble(), list[i * 2 + 1].toDouble());

    return data;
}

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
    auto file = std::make_unique<QFile>(fileName);
    if (!file->open(QFile::OpenModeFlag::ReadOnly))
        return;

    QVector<QVector<Point>> data;
    data.reserve(POINTS_NUMBER);

    // ..read file..
    while (!file->atEnd()) {
        auto line = file->readLine();
        data.push_back(parseLine(line));
    }

    file->close();

    // ..fill graphs..
    QList<Graph> graphs;
    for (const auto& row : data) {

        if (graphs.size() < row.size()) {
            for (const auto& point : row) {
                Graph graph;
                graph << point;
                graphs << graph;
            }
        }
        else {

            int i { 0 };
            for (const auto& point : row) {
                graphs[i] << point;
                i++;
            }
        }
    }

    for(const auto& graph : graphs)
        customPlotBuilderPtr->addGraph(graph, fileName);

}

void MainWindow::print()
{
    QPrinter printer;



    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Document"));

//    if (editor->textCursor().hasSelection())
//        dialog.addEnabledOption(QAbstractPrintDialog::PrintSelection);
    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        painter.setWindow(m_graphWidget->rect());
        m_graphWidget->render(&painter);
    }
}
