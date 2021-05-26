#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "plotparams.h"
#include "paramsdialog.h"
#include "customplot.h"
#include "pv_parsers.h"
#include "parser_check.h"

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
    , functionDialog_(std::make_unique<FunctionStringDialog>())
    , ui_(new Ui::MainWindow)
    , parserDialog_(std::make_unique<ParserCheckDialog>())
{
    ui_->setupUi(this);

    //        ui->customPlotLayout->setContextMenuPolicy(Qt::CustomContextMenu);

    plotImpl_ = PvPlot::CreateInstance();
    m_graphWidget = plotImpl_->widget();
    m_graphWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    QHBoxLayout* layout = new QHBoxLayout(ui_->frame);
    ui_->frame->setLayout(layout);
    layout->addWidget(m_graphWidget);

    connect(ui_->cbLegend,         SIGNAL(toggled(bool)),                      this, SLOT(showHideLegend(bool)));
    connect(ui_->openAction,       SIGNAL(triggered(bool)),                    this, SLOT(openFile()));
    connect(ui_->actionSet_parser, SIGNAL(triggered(bool)),                    this, SLOT(runParserDialog()));
    connect(ui_->clearAction,      SIGNAL(triggered(bool)),                    this, SLOT(clear()));
    connect(ui_->printAction,      SIGNAL(triggered(bool)),                    this, SLOT(showPrint()));
    connect(ui_->previewAction,    SIGNAL(triggered(bool)),                    this, SLOT(showPrintPreview()));
    connect(ui_->titleChartAction, SIGNAL(triggered(bool)),                    this, SLOT(setTitleChart()));
    connect(ui_->quitAction,       SIGNAL(triggered(bool)),                    qApp, SLOT(quit()));
    connect(ui_->action_function,  SIGNAL(triggered(bool)),         functionDialog_.get(), SLOT(show()));
    connect(functionDialog_.get(),      SIGNAL(sendString(const QString&)),         this, SLOT(lineCalcExec(const QString&)));
    connect(m_graphWidget,        SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    connect(parserDialog_.get(),  &ParserCheckDialog::send,       this, &MainWindow::loadParsers);

    setWindowIcon(QIcon(":/graph.png"));
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
        globalPos = m_graphWidget->mapToGlobal(pos);

    QMenu menu;
    QAction *openParams = new QAction(QString::fromUtf8("Options..."), this);
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

    for(auto pair : list){

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

    for(auto file : files_)
        render(file);
}
