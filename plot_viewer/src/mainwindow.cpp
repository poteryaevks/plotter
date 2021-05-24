#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plotparams.h"
#include "paramsdialog.h"
#include "customplot.h"
#include "pv_parsers.h"

#include <algorithm>

#include <QFileDialog>
#include <QDir>
#include <QIcon>
#include <QFile>
#include <QLoggingCategory>
#include <QStringList>

static const double POINTS_NUMBER { 10000 };

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , FunctionDialog_(new functionStringDialog)
    , ui(new Ui::MainWindow)
    , parse_(std::bind(defaultParse, std::placeholders::_1))
    , parser_(nullptr)
{
    ui->setupUi(this);

    library_ = LibraryPtr(new QLibrary, [](QLibrary* library){
            library->unload();
});

    //        ui->customPlotLayout->setContextMenuPolicy(Qt::CustomContextMenu);

    plotImpl_ = PvPlot::CreateInstance();
    m_graphWidget = plotImpl_->widget();
    m_graphWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QHBoxLayout* layout = new QHBoxLayout(ui->frame);
    ui->frame->setLayout(layout);
    layout->addWidget(m_graphWidget);

    connect(ui->cbLegend,         SIGNAL(toggled(bool)),                      this, SLOT(showHideLegend(bool)));
    connect(ui->openAction,       SIGNAL(triggered(bool)),                    this, SLOT(openFile()));
    connect(ui->actionSet_parser, SIGNAL(triggered(bool)),                    this, SLOT(openParser()));
    connect(ui->clearAction,      SIGNAL(triggered(bool)),                    this, SLOT(clear()));
    connect(ui->printAction,      SIGNAL(triggered(bool)),                    this, SLOT(showPrint()));
    connect(ui->previewAction,    SIGNAL(triggered(bool)),                    this, SLOT(showPrintPreview()));
    connect(ui->titleChartAction, SIGNAL(triggered(bool)),                    this, SLOT(setTitleChart()));
    connect(FunctionDialog_,      SIGNAL(sendString(const QString&)),         this, SLOT(lineCalcExec(const QString&)));
    connect(m_graphWidget,        SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->quitAction,       SIGNAL(triggered(bool)),                    qApp, SLOT(quit()));
    connect(ui->action_function,  SIGNAL(triggered(bool)),         FunctionDialog_, SLOT(show()));

    setWindowIcon(QIcon(":/graph.png"));
}

MainWindow::~MainWindow()
{  
    if(parser_)
        delete parser_;

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
    if (plotImpl_ != nullptr) {
        ParamsDialog dialog(&plotImpl_->params(), this);

        if (dialog.exec() == QDialog::Accepted) {
            plotImpl_->applyParams();
        }
    }
}

void MainWindow::lineCalcExec(const QString& expression)
{
    QString errInfo;
    Graph graph;

    QString function_x(expression);
    if (function_x.isEmpty())
        return;

    for (std::size_t x{ 0 }; x < 500; ++x) {

        QString func = function_x;
        QString str_i = QString::number(x);
        func.replace("x", str_i);

        try {
            auto y = lineCalculator_.calculate(func.toStdString());
            graph << Point(x, y);
        }
        catch (const std::exception &ex) {
            errInfo = ex.what();
            break;
        }
    }

    plotImpl_->addGraph(graph, QString("User function"));

    if(!errInfo.isEmpty())
        QMessageBox::warning(m_graphWidget, "Error" , errInfo);
}

void MainWindow::openParser()
{        
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 QString::fromUtf8("Open file"),
                                                 QDir::currentPath(),
                                                 QString::fromUtf8("Dynamic libraries (*.so *.dll)")
                                                 );


    auto loadParserImpl = loadParser(std::move(fileName));
    if(loadParserImpl){

        parser_ = loadParserImpl();
        parse_ = std::bind(&IPvParser::parse, parser_, std::placeholders::_1);
    }
    else{
        parse_ = std::bind(defaultParse, std::placeholders::_1);
    }

    for(auto& file : files_){

        plotImpl_->clearViewer();
        file->reset();
        renderGraph(file.get(), parse_);
    }
}

MainWindow::ParserLoader MainWindow::loadParser(QString fileName)
{
    library_->setFileName(fileName);

    if(!library_->load())
        return nullptr;

    return (ParserLoader)library_->resolve("loadParser");
}

void MainWindow::clear()
{
    plotImpl_->clearViewer();
    files_.clear();
}

bool MainWindow::parseLine(QString line,
                           LineRawData& lineRawData,
                           const ParseFuncType& parse)
{
    try {

        lineRawData = parse(std::move(line));
        return true;
    }
    catch(const std::exception& ex){

        QString err = QString("Error while parsing accured. Reason: %1")
                .arg(ex.what());

        qWarning(err.toStdString().data());
        return false;
    }
    catch(...){

        qWarning("Error while parsing accured");
        return false;
    }
}

void MainWindow::showHideLegend(bool /*p_onOff*/)
{
    plotImpl_->showHideLegend();
}

void MainWindow::on_Xmin_valueChanged(double xmin)
{
    plotImpl_->xAxisMinChanged(xmin);
}

void MainWindow::on_Ymin_valueChanged(double ymin)
{
    plotImpl_->yAxisMinChanged(ymin);
}

void MainWindow::on_Xmax_valueChanged(double xmax)
{
    plotImpl_->xAxisMaxChanged(xmax);
}

void MainWindow::on_Ymax_valueChanged(double ymax)
{
    plotImpl_->yAxisMaxChanged(ymax);
}

void MainWindow::openFile()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 QString::fromUtf8("Open file"),
                                                 QDir::currentPath()
//                                                 , QString::fromUtf8("Text file (*.txt *.csv)")
                                                 );
    auto file = loadFile(std::move(fileName));
    if(!file){

        qWarning("Unable to load data file");
        return;
    }

    renderGraph(file, parse_);
}

void MainWindow::renderGraph(QFile* file, const ParseFuncType& parser)
{
    if(!file->isOpen()){

        qWarning("Data file is not opened");
        return;
    }

    GraphRawData rawData;

    // ..read file..
    while (!file->atEnd()) {

        LineRawData lineRawData;

        auto success = parseLine(file->readLine(),
                                   lineRawData,
                                   parser);

        if(success)
            rawData << std::move(lineRawData);
    }

    for(const auto& graph : extractGraphs(rawData))
        plotImpl_->addGraph(graph, file->fileName());
}

QFile* MainWindow::loadFile(QString fileName)
{
    auto file = FilePtr(new QFile(fileName),
                        [](QFile* f){ f->close(); });

    if (!file->open(QFile::OpenModeFlag::ReadOnly)){

        qWarning("Unable to open data file");
        return nullptr;
    }

    files_.push_back(std::move(file));
    return files_.back().get();
}

QList<Graph> MainWindow::extractGraphs(GraphRawData rawData)
{
    QList<Graph> graphs;

    for (auto& row : rawData) {

        if (graphs.size() < row.size()) {

            for (auto& point : row) {
                Graph graph;
                graph << std::move(point);
                graphs << std::move(graph);
            }
        }
        else {

            int i { 0 };
            for (auto& point : row)
                graphs[i++] << std::move(point);
        }
    }

    return std::move(graphs);
}

void MainWindow::showPrintPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Orientation::Landscape);
    QPrintPreviewDialog previewDialog(&printer, this);
    previewDialog.resize(this->size().width() * 0.8, this->size().height() * 0.8);

    connect(&previewDialog, SIGNAL(paintRequested(QPrinter*)), SLOT(print(QPrinter*)));
    previewDialog.exec();
}

void MainWindow::showPrint()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Orientation::Landscape);
    QPrintDialog printDialog(&printer, this);
    //    dialog.setWindowTitle(tr("Print Document"));

    connect(&printDialog, SIGNAL(accepted(QPrinter*)), SLOT(print(QPrinter*)));
    printDialog.exec();
}

void MainWindow::print(QPrinter *printer)
{
    if (printer != nullptr) {
        QPainter painter(printer);
        painter.setWindow(m_graphWidget->rect());
        m_graphWidget->render(&painter);
    }
}

void MainWindow::setTitleChart()
{
    QString title = plotImpl_->title();

    title = QInputDialog::getText(this,
                                  tr("Enter title"),
                                  tr("Title:"),
                                  QLineEdit::Normal,
                                  title);

    if (title.isEmpty())
        return;

    plotImpl_->setTitle(title);
}
