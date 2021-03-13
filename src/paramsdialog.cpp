#include "paramsdialog.h"
#include "ui_paramsdialog.h"
#include "base.h"

ParamsDialog::ParamsDialog(QVector<PlotParams*>* data, QWidget *parent) :
    QDialog(parent),
    m_pdata(data),
    ui(new Ui::ParamsDialog)
{
    ui->setupUi(this);

    setupModel();
    ui->tableView->setModel(model);
    ui->tableView->setItemDelegateForColumn(1, new ColorDelegate(defaultColors, this));
    ui->tableView->setItemDelegateForColumn(2, new ListDelegate(defaultLs.keys(), this));
    ui->tableView->setItemDelegateForColumn(3, new ListDelegate(defaultScatters.keys(), this));
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->applyButton, &QPushButton::clicked, this, &ParamsDialog::writeParams);
}

ParamsDialog::~ParamsDialog()
{
    delete ui;
}

void ParamsDialog::setupModel()
{
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Color") << tr("Line style") << tr("Point style"));
    QStandardItem *item = nullptr;

    for(int i = 0; i < m_pdata->size(); ++i) {
        QList<QStandardItem*> items;

        item = new QStandardItem(m_pdata->at(i)->getPlotName());
        items << item;

        Qt::GlobalColor color = m_pdata->at(i)->getColor();
        auto color_index = defaultColors.indexOf(color);

        item = new QStandardItem();
        item->setData(color_index, Qt::UserRole);
        items << item;

        auto ls_index = m_pdata->at(i)->getLineStyle();
        item = new QStandardItem(defaultLs.key(ls_index));
        items << item;

        auto sc_index = m_pdata->at(i)->getScatterStyle();
        item = new QStandardItem(defaultScatters.key(sc_index));
        items << item;

        model->appendRow(items);
    }
}

void ParamsDialog::writeParams()
{
    int i = 0;

    for(auto it = (*m_pdata).begin(); it != (*m_pdata).end(); ++it) {
        auto color_index = model->index(i, 1).data().toInt();
        auto ls = model->index(i, 2).data().toString();
        auto cs_style = model->index(i, 3).data().toString();
        (*it)->setColor(defaultColors[color_index]);
        (*it)->setLineStyle(defaultLs[ls]);
        (*it)->setScatterStyle(defaultScatters[cs_style]);
        ++i;
    }
}
