#include "paramsdialog.h"
#include "ui_paramsdialog.h"

PvParamsDialog::PvParamsDialog(QList<PvPlotParams*> data, QWidget *parent) :
    QDialog(parent),
    data_(data),
    ui(new Ui::ParamsDialog)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Color") << tr("Line style") << tr("Point style"));
    setupModel();
//    ui->tableWidget->setModel(model);

//    ui->tableWidget->setItemDelegateForColumn(1, new ColorDelegate(defaultColors, this));
    ui->tableWidget->setItemDelegateForColumn(2, new ListDelegate(defaultLs.keys(), this));
    ui->tableWidget->setItemDelegateForColumn(3, new ListDelegate(defaultScatters.keys(), this));
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->applyButton, &QPushButton::clicked, this, &PvParamsDialog::writeParams);
    connect(ui->applyButton, &QPushButton::clicked, this, &PvParamsDialog::accept);
    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &PvParamsDialog::showColorDialog);
}

PvParamsDialog::~PvParamsDialog()
{
    delete ui;
}

void PvParamsDialog::setupModel()
{
//    model = new QStandardItemModel(this);
//    model->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Color") << tr("Line style") << tr("Point style"));
//    QStandardItem *item = nullptr;

    for(int i = 0; i < data_.size(); ++i) {

        int numRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(numRow);
        ui->tableWidget->setItem(numRow, 0, new QTableWidgetItem(data_.at(i)->getPlotName()));

        QTableWidgetItem *itemColor = new QTableWidgetItem;
        itemColor->setBackgroundColor(data_.at(i)->getColor());
        ui->tableWidget->setItem(numRow, 1, itemColor);

        auto ls_index = data_.at(i)->getLineStyle();
//        item = new QStandardItem(defaultLs.key(ls_index));
        ui->tableWidget->setItem(numRow, 2, new QTableWidgetItem(defaultLs.key(ls_index)));

        auto sc_index = data_.at(i)->getScatterStyle();
//        item = new QStandardItem(defaultScatters.key(sc_index));
        ui->tableWidget->setItem(numRow, 3, new QTableWidgetItem(defaultScatters.key(sc_index)));
    }
}

void PvParamsDialog::writeParams()
{
    int i = 0;

    for (auto it = data_.begin(); it != data_.end(); ++it) {
        auto ls = ui->tableWidget->item(i, 2)->text();
        auto cs_style = ui->tableWidget->item(i, 3)->text();
        (*it)->setColor(ui->tableWidget->item(i, 1)->backgroundColor(), true);
        (*it)->setLineStyle(defaultLs[ls]);
        (*it)->setScatterStyle(defaultScatters[cs_style]);
        ++i;
    }
}

void PvParamsDialog::showColorDialog(int row, int column)
{
    if (column == 1) {
        QColorDialog *colorDialog = new QColorDialog(this);
        if (colorDialog->exec() == QDialog::Accepted)
            ui->tableWidget->item(row, column)->setBackgroundColor(colorDialog->selectedColor());
    }
}
