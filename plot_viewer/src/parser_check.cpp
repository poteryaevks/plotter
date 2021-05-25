#include "parser_check.h"
#include "ui_parser_check.h"
#include "parsers/parser_interface.h"

#include <QFileDialog>
#include <QLibrary>

ParserCheckDialog::ParserCheckDialog(QWidget *parent)
    : QDialog(parent),
      ui_(new Ui::ParserCheck)
{
    ui_->setupUi(this);

    ui_->tableWidget->setColumnCount(2);

    ui_->tableWidget->setHorizontalHeaderLabels(
                QStringList{ tr("File"),
                             tr("Parser library") }
                );

    ui_->tableWidget->horizontalHeader()->setSectionResizeMode(
                QHeaderView::ResizeToContents
                );

    ui_->tableWidget->horizontalHeader()->setVisible(true);

    setupModel(ui_->tableWidget, data_);
}


void ParserCheckDialog::setupModel(QTableWidget* table, const TableDataType& data)
{
    // ..resize table..
    while(table->rowCount() < data.size())
        table->insertRow(table->rowCount());

    int currentRow { 0 };

    for(const auto &pair : data) {

        table->setItem(currentRow, 0, new QTableWidgetItem(pair.first));
        table->setItem(currentRow, 1, new QTableWidgetItem(pair.second));

        currentRow++;
    }
}

ParserCheckDialog::~ParserCheckDialog()
{
    delete ui_;
}

void ParserCheckDialog::setData(ParserCheckDialog::TableDataType&& data)
{
    data_ = std::move(data);
    setupModel(ui_->tableWidget, data_);
}

void ParserCheckDialog::reset()
{
    data_.clear();
    ui_->tableWidget->clear();
}


void ParserCheckDialog::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    auto column = item->column();
    auto row = item->row();

    if(column == 1){

        auto fileName = QFileDialog::getOpenFileName(this,
                                                     QString::fromUtf8("Open file"),
                                                     QDir::currentPath(),
                                                     QString::fromUtf8("Dynamic libraries (*.so *.dll)")
                                                     );



        data_[row].second = fileName;
        setupModel(item->tableWidget(), data_);
    }

}

void ParserCheckDialog::on_buttonBox_accepted()
{
    send(data_);
}
