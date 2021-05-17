#include "paramsdialog.h"
#include "ui_paramsdialog.h"
#include "base.h"

ParamsDialog::ParamsDialog(QList<PlotParams*>* data, QWidget *parent) :
    QDialog(parent),
    m_pdata(data),
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

    connect(ui->applyButton, &QPushButton::clicked, this, &ParamsDialog::writeParams);
    connect(ui->applyButton, &QPushButton::clicked, this, &ParamsDialog::accept);
    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &ParamsDialog::showColorDialog);
}

ParamsDialog::~ParamsDialog()
{
    delete ui;
}

void ParamsDialog::setupModel()
{
//    model = new QStandardItemModel(this);
//    model->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Color") << tr("Line style") << tr("Point style"));
//    QStandardItem *item = nullptr;

    for(int i = 0; i < m_pdata->size(); ++i) {
        int numRow = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(numRow);
        ui->tableWidget->setItem(numRow, 0, new QTableWidgetItem(m_pdata->at(i)->getPlotName()));

        QTableWidgetItem *itemColor = new QTableWidgetItem;
        itemColor->setBackgroundColor(m_pdata->at(i)->getColor());
        ui->tableWidget->setItem(numRow, 1, itemColor);

        auto ls_index = m_pdata->at(i)->getLineStyle();
//        item = new QStandardItem(defaultLs.key(ls_index));
        ui->tableWidget->setItem(numRow, 2, new QTableWidgetItem(defaultLs.key(ls_index)));

        auto sc_index = m_pdata->at(i)->getScatterStyle();
//        item = new QStandardItem(defaultScatters.key(sc_index));
        ui->tableWidget->setItem(numRow, 3, new QTableWidgetItem(defaultScatters.key(sc_index)));
    }
}

void ParamsDialog::writeParams()
{
    int i = 0;

    for (auto it = (*m_pdata).begin(); it != (*m_pdata).end(); ++it) {
        auto ls = ui->tableWidget->item(i, 2)->text();
        auto cs_style = ui->tableWidget->item(i, 3)->text();
        (*it)->setColor(ui->tableWidget->item(i, 1)->backgroundColor(), true);
        (*it)->setLineStyle(defaultLs[ls]);
        (*it)->setScatterStyle(defaultScatters[cs_style]);
        ++i;
    }
}

void ParamsDialog::showColorDialog(int row, int column)
{
    if (column == 1) {
        QColorDialog *colorDialog = new QColorDialog(this);
        if (colorDialog->exec() == QDialog::Accepted)
            ui->tableWidget->item(row, column)->setBackgroundColor(colorDialog->selectedColor());
    }
}
