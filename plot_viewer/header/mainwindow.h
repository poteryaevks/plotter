#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "parsers/parser_interface.h"

#include "qcustomplot.h"
#include "functionstringdialog.h"
#include "PvLineCalculator.h"
#include "customplot.h"
#include "common_types.h"

namespace Ui {
class MainWindow;
}

class ParserCheckDialog;
using namespace plot_viewer;

class MainWindow : public QMainWindow
{
    using FilePtr = std::shared_ptr<QFile>; //, std::function<void(QFile*)>
    using IPvParserPtr = IPvParser*;
    using ParserCheckDialogPtr = std::unique_ptr<ParserCheckDialog>;
    using ParseFuncType = std::function<Graph(QString)>;
    using FileParserPair = QPair<FilePtr, IPvParserPtr>;
    using LibraryPtr = std::shared_ptr<QLibrary>;
    using FunctionStringDialogPtr = std::unique_ptr<FunctionStringDialog>;
    typedef IPvParser* (*ParserLoader)();

    using RowData = QPair<QString, QString>;
    using TableDataType = QList<RowData>;

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
    //! \brief lineCalcExec
    //! \param expression
    //!
    void lineCalcExec(const QString& expression);

    //!
    //! \brief loadParsers
    //!
    void loadParsers(QList<QPair<QString, QString>>);

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
    QList<Graph> extractGraphs(GraphRawData rawData);

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
    IPvParser* loadParser(LibraryPtr library);

    //!
    //! \brief createLibrary
    //! \param fileName
    //! \return
    //!
    LibraryPtr createLibrary(const QString& fileName);

private:

    FunctionStringDialogPtr functionDialog_;
    Ui::MainWindow *ui_;
    PvLineCalculator lineCalculator_;
    QWidget *m_graphWidget;
    ParserCheckDialogPtr parserDialog_;
    CustomPlotBuilderPtr plotImpl_;

    QList<FileParserPair> files_;
    QList<LibraryPtr> libraries_;                   //! загрузчик библиотеки парсера
    QMap<IPvParser*, QLibrary*> libraryConnecter_;
};

#endif // MAINWINDOW_H
