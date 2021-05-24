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

using namespace plot_viewer;

class MainWindow : public QMainWindow
{
    using FilePtr = std::unique_ptr<QFile, std::function<void(QFile*)>>;
    using LibraryPtr = std::unique_ptr<QLibrary, std::function<void(QLibrary*)>>;
    using ParseFuncType = std::function<Graph(QString)>;
    typedef IPvParser* (*ParserLoader)();

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
    void openParser();

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
    QFile* loadFile(QString fileName);

    //!
    //! \brief loadParser
    //! \param fileName
    //! \return
    //!
    MainWindow::ParserLoader loadParser(QString fileName);

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
    void renderGraph(QFile* file,
                     const ParseFuncType& parser);

private:

    std::list<FilePtr> files_;
    functionStringDialog *FunctionDialog_;
    Ui::MainWindow *ui;
    PvLineCalculator lineCalculator_;
    QWidget *m_graphWidget;
    CustomPlotBuilderPtr plotImpl_;
    ParseFuncType parse_;      //! указатель на функцию парсинга строки
    IPvParser* parser_;        //! интерфейс парсера
    LibraryPtr library_;       //! загрузчик библиотеки парсера
};

#endif // MAINWINDOW_H
