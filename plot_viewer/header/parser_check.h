#ifndef PARSER_CHECK_H
#define PARSER_CHECK_H

#include <QDialog>
#include <QFile>

#include <memory>

namespace Ui {
class ParserCheck;
}

class QTableWidget;
class QLibrary;
class QTableWidgetItem;
class IPvParser;

//!
//! \brief The ParserCheckDialog class
//!
class ParserCheckDialog final : public QDialog
{
    Q_OBJECT

//    using FilePtr = std::shared_ptr<QFile>;
//    using RawDataType = QPair<FilePtr, IPvParser*>;
//    using RawDataTypeList = QList<RawDataType>;
    using RowData = QPair<QString, QString>;
    using TableDataType = QList<RowData>;


public:


    ParserCheckDialog(QWidget *parent = nullptr);

    ~ParserCheckDialog();

    void setData(TableDataType&& data);

    void reset();

private slots:
    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

    void on_buttonBox_accepted();

private:

    void setupModel(QTableWidget* table, const TableDataType& data);



signals:

    void send(TableDataType);

private:

    TableDataType data_;
    Ui::ParserCheck* ui_;
    QList<IPvParser*> parsers_;
};


#endif

