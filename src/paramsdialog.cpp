#include "paramsdialog.h"
#include "ui_paramsdialog.h"
#include "base.h"


paramsDialog::paramsDialog(QVector<PlotParams*>* data, QWidget *parent) :
    QDialog(parent),
    m_pdata(data),
    ui(new Ui::paramsDialog),
    pcolorDel(new ColorDelegate)
{
    ui->setupUi(this);

    QList<QString> list = defaultLs.keys();
    pLsListDel = new ListDelegate(list);
    //
    list = defaultScatters.keys();
    pScListDel = new ListDelegate(list);
    installEventFilter(this);
}


paramsDialog::~paramsDialog()
{
    delete ui;
}

bool paramsDialog::eventFilter(QObject */*object*/, QEvent *event)
{
    if(m_pdata->isEmpty())
        return false;
    if(event->type() == QEvent::Show) {
        setupModel();
        ui->tableView->setModel(model);
        ui->tableView->setItemDelegateForColumn(1, pcolorDel);
        ui->tableView->setItemDelegateForColumn(2, pLsListDel);
        ui->tableView->setItemDelegateForColumn(3, pScListDel);
    }
    if(event->type() == QEvent::Hide) {
        clearModel();
    }
    return false;
}


void paramsDialog::setupModel()
{
    auto row_number = m_pdata->size();
    model = new QStandardItemModel(row_number, COLUMNS_NUM, this);
    model->setHeaderData(0, Qt::Horizontal, tr("Name"));
    model->setHeaderData(1, Qt::Horizontal, tr("Color"));
    model->setHeaderData(2, Qt::Horizontal, tr("Line style"));
    model->setHeaderData(3, Qt::Horizontal, tr("Point style"));
    //
    for(std::size_t i {0}; i < model->rowCount(); ++i) {
        Qt::GlobalColor color = m_pdata->at(i)->getColor();
        auto color_index = defaultColors.indexOf(color);
        //
        auto ls_index = m_pdata->at(i)->getLineStyle();
        auto ls_name = defaultLs.key(ls_index);
        //
        auto sc_index = m_pdata->at(i)->getScatterStyle();
        auto sc_name = defaultScatters.key(sc_index);
        //
        model->setData(model->index(i, 0, QModelIndex()), m_pdata->at(i)->getPlotName());
        model->setData(model->index(i, 1, QModelIndex()), color_index);
        model->setData(model->index(i, 2, QModelIndex()), ls_name);
        model->setData(model->index(i, 3, QModelIndex()), sc_name);
    }
}

void paramsDialog::clearModel()
{
    delete model;
}

void paramsDialog::on_replot_released()
{
    auto it = (*m_pdata).begin();
    for(std::size_t i { 0 }; it != (*m_pdata).end(); ++it, ++i) {
        auto color_index = model->index(i, 1).data().toInt();
        auto ls = model->index(i, 2).data().toString();
        auto cs_style = model->index(i, 3).data().toString();
        (*it)->setColor(defaultColors[color_index]);
        (*it)->setLineStyle(defaultLs[ls]);
        (*it)->setScatterStyle(defaultScatters[cs_style]);
    }
    emit replot();
}

void paramsDialog::on_Cancel_released()
{
    accept();
}
