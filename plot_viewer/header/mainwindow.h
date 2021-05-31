#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "common_types.h"

using namespace plot_viewer;

namespace Ui {
class MainWindow;
}

class IPvParser;
class IPlot;
class ParserDialog;

class MainWindow : public QMainWindow
{
    using FilePtr = std::shared_ptr<QFile>;
    using IPvParserPtr = std::shared_ptr<IPvParser>;
    using ParserCheckDialogPtr = std::unique_ptr<ParserDialog>;
    using ParseFuncType = std::function<Graph(QString)>;
    using FileParserPair = QPair<FilePtr, IPvParserPtr>;
    using LibraryPtr = std::shared_ptr<QLibrary>;
    typedef IPvParser* (*ParserLoader)();
    using RowData = QPair<QString, QString>;
    using TableDataType = QList<RowData>;
    using CustomPlotBuilderPtr = std::shared_ptr<IPlot>;

    Q_OBJECT

public:

    //!
    //! \brief MainWindow
    //! \param parent
    //!
    MainWindow(QWidget *parent = 0);

    //!
    //! \brief MainWindow
    //!
    ~MainWindow();

public slots:

    //!
    //! \brief loadParsers
    //!
    void loadParsers(TableDataType);

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
    void setTitleChart();
    void runParserDialog();

private:

    //!
    //! \brief parseLine
    //! \param line
    //! \param lineRawData
    //! \param parse
    //! \return
    //!
    bool parseLine(QString line,
                   LineRawData& lineRawData,
                   const ParseFuncType& parse);

    //!
    //! \brief loadFile
    //! \param fileName
    //! \return
    //!
    FilePtr loadFile(QString fileName);

    //!
    //! \brief extractGraphs
    //! \param rawData
    //! \return
    //!
    std::vector<Graph> extractGraphs(GraphRawData rawData);

    //!
    //! \brief renderGraph
    //! \param file
    //! \param parser
    //!
    //!
    void render(FileParserPair pair);

    //!
    //! \brief loadParser
    //! \param library
    //! \return
    //!
    IPvParserPtr loadParser(LibraryPtr library);

    //!
    //! \brief createLibrary
    //! \param fileName
    //! \return
    //!
    LibraryPtr createLibrary(const QString& fileName);

    //!
    //! \brief createPlot
    //! \param type
    //! \return
    //!
    CustomPlotBuilderPtr createPlot(plot_viewer::ePlotType type);

private:

    Ui::MainWindow* ui_;
    ParserCheckDialogPtr parserDialog_;
    CustomPlotBuilderPtr plotImpl_;
    std::list<FileParserPair> files_;
    std::list<LibraryPtr> libraries_;                   //! загрузчик библиотеки парсера
    std::map<IPvParser*, QLibrary*> libraryConnecter_;
    QWidget* graphWidget_;
};

#endif // MAINWINDOW_H
