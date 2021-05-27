#include "parserdialog.h"
#include "ui_parser_check.h"
#include "parsers/parser_interface.h"

#include <QFileDialog>
#include <QLibrary>

ParserDialog::ParserDialog(QWidget *parent)
    : QDialog(parent),
      ui_(new Ui::ParserCheck)
{
    ui_->setupUi(this);
    setupModel(ui_->tableWidget, data_);
}


void ParserDialog::setupModel(QTableWidget* table, const TableDataType& data)
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

    table->setColumnCount(2);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setVisible(true);

    table->setHorizontalHeaderLabels(
                QStringList{ tr("File"),
                             tr("Parser library") }
                );

    table->horizontalHeader()->setSectionResizeMode(
                QHeaderView::ResizeToContents
                );
}

ParserDialog::~ParserDialog()
{
    delete ui_;
}

void ParserDialog::setData(ParserDialog::TableDataType&& data)
{
    data_ = std::move(data);
    setupModel(ui_->tableWidget, data_);
}

void ParserDialog::reset()
{
    data_.clear();
    ui_->tableWidget->clear();
}


void ParserDialog::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
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

void ParserDialog::on_buttonBox_accepted()
{
    send(data_);
}
