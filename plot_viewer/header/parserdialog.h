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

//!
//! \brief The ParserCheckDialog class
//!
class ParserDialog final : public QDialog
{
    using RowData = QPair<QString, QString>;
    using TableDataType = QList<RowData>;

    Q_OBJECT

public:


    ParserDialog(QWidget *parent = nullptr);

    ~ParserDialog();

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
};


#endif

