#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plotparams.h"
#include "paramsdialog.h"
#include "defaultparser.h"
#include "parserdialog.h"
#include "iplot.h"
#include "parsers/parser_interface.h"
#include "plot_factory.h"

#include <algorithm>

#include <QFileDialog>
#include <QDir>
#include <QIcon>
#include <QFile>
#include <QLoggingCategory>
#include <QStringList>



static const double POINTS_NUMBER { 10000 };
static int GRAPH_NUM { 1 };

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
    , parserDialog_(std::make_unique<ParserDialog>())
{
    ui_->setupUi(this);

    //        ui->customPlotLayout->setContextMenuPolicy(Qt::CustomContextMenu);

    plotImpl_ = createPlot(QCUSTOM_PLOT);

    graphWidget_ = plotImpl_->widget();
    graphWidget_->setContextMenuPolicy(Qt::CustomContextMenu);

    QHBoxLayout* layout = new QHBoxLayout(ui_->frame);
    ui_->frame->setLayout(layout);
    layout->addWidget(graphWidget_);

    connect(ui_->cbLegend,         SIGNAL(toggled(bool)),                      this, SLOT(showHideLegend(bool)));
    connect(ui_->openAction,       SIGNAL(triggered(bool)),                    this, SLOT(openFile()));
    connect(ui_->clearAction,      SIGNAL(triggered(bool)),                    this, SLOT(clear()));
    connect(ui_->printAction,      SIGNAL(triggered(bool)),                    this, SLOT(showPrint()));
    connect(ui_->previewAction,    SIGNAL(triggered(bool)),                    this, SLOT(showPrintPreview()));
    connect(ui_->titleChartAction, SIGNAL(triggered(bool)),                    this, SLOT(setTitleChart()));
    connect(graphWidget_,          SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui_->quitAction,       SIGNAL(triggered(bool)),                    qApp, SLOT(quit()));
    connect(parserDialog_.get(),   &ParserDialog::send,                        this, &MainWindow::loadParsers);

    setWindowIcon(QIcon(":/graph.png"));
}

MainWindow::CustomPlotBuilderPtr MainWindow::createPlot(plot_viewer::ePlotType type)
{
    return plot_viewer::FACTORY.Create(type);
}

MainWindow::~MainWindow()
{
    clear();
    delete ui_;
}

void MainWindow::showContextMenu(QPoint pos)
{
    QPoint globalPos;

    // если запрос от QAbstractScrollArea
    if (sender()->inherits("QAbstractScrollArea"))
        globalPos = qobject_cast<QAbstractScrollArea*>(sender())->viewport()->mapToGlobal(pos);
    else
        globalPos = graphWidget_->mapToGlobal(pos);

    QMenu menu;

    QAction* openParams = new QAction(QString::fromUtf8("Options..."), this);
    menu.addAction(openParams);

    QAction* openParserBrowser = new QAction(QString::fromUtf8("Parsers..."), this);
    menu.addAction(openParserBrowser);

    connect(openParams, SIGNAL(triggered()), this, SLOT(showParamsPlot()));
    connect(openParserBrowser, SIGNAL(triggered()), this, SLOT(runParserDialog()));

    menu.exec(globalPos);
}

void MainWindow::showParamsPlot()
{
    if (plotImpl_ != nullptr) {

        PvParamsDialog dialog(&plotImpl_->params(), this);

        if (dialog.exec() == QDialog::Accepted) {
            plotImpl_->applyParams();
        }
    }
}

void MainWindow::runParserDialog()
{
    TableDataType data;

    for(auto file : files_){

        RowData rowData;

        rowData.first = file.first->fileName();

        if(file.second)
            rowData.second = libraryConnecter_[file.second]->fileName();
        else
            rowData.second = std::move(QString("default"));

        data << std::move(rowData);
    }

    parserDialog_->setData(std::move(data));
    parserDialog_->show();
}

void MainWindow::clear()
{
    plotImpl_->clearViewer();
    parserDialog_->reset();
    files_.clear();
    libraryConnecter_.clear();
    GRAPH_NUM = 1;
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
                                                 );
    auto file = loadFile(std::move(fileName));
    if(!file){

        qWarning("Unable to load data file");
        return;
    }

    files_.push_back(
                FileParserPair(file, nullptr)
                );

    render(files_.back());
}

void MainWindow::render(FileParserPair pair)
{
    ParseFuncType parseFunc;

    auto file = pair.first;
    auto parserLoader = pair.second;

    if(!file->isOpen()){

        qWarning("Data file is not opened");
        return;
    }

    if(parserLoader)
        parseFunc = std::bind(&IPvParser::parse, parserLoader, std::placeholders::_1);
    else
        parseFunc = std::bind(defaultParse, std::placeholders::_1);

    GraphRawData rawData;

    // ..read file..
    while (!file->atEnd()) {

        LineRawData lineRawData;

        auto success = parseLine(file->readLine(),
                                 lineRawData,
                                 parseFunc);
        if(success)
            rawData << std::move(lineRawData);
    }

    QFileInfo fileInfo(*file);

    for(auto& graph : extractGraphs(rawData)){

        QString graphName = std::move(
                    QString("%1_%2").arg(fileInfo.fileName()).arg(GRAPH_NUM++)
                    );

        plotImpl_->addGraph(
                    graph,
                    graphName
                    );
    }
}

MainWindow::FilePtr
MainWindow::loadFile(QString fileName)
{
    auto file = FilePtr(new QFile(fileName),
                        [](QFile* f){ f->close(); }
            );

    if (!file->open(QFile::OpenModeFlag::ReadOnly)){

        qWarning("Unable to open data file");
        return nullptr;
    }

    return file;
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
        painter.setWindow(graphWidget_->rect());
        graphWidget_->render(&painter);
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

MainWindow::LibraryPtr MainWindow::createLibrary(const QString& fileName)
{
    return LibraryPtr(new QLibrary(fileName),
                      [](QLibrary* lib) { lib->unload(); }
    );
}

IPvParser* MainWindow::loadParser(LibraryPtr library)
{
    if(!library->load())
        return nullptr;

    auto loadParserImpl = (ParserLoader)library->resolve("loadParser");

    if(loadParserImpl)
        return loadParserImpl();
    else
        return nullptr;
}

void MainWindow::loadParsers(QList<QPair<QString, QString>> list)
{
    clear();

    for(const auto& pair : list){

        auto fileName = pair.first;
        auto parserPath = pair.second;

        auto file = loadFile(fileName);

        auto library = createLibrary(parserPath);

        auto parser = loadParser(library);


        libraries_.push_back(
                    library
                    );

        files_.push_back(
                    FileParserPair(file, parser)
                    );

        libraryConnecter_.insert(parser,
                                 library.get()
                                 );
    }

    for(const auto& file : files_)
        render(file);
}
