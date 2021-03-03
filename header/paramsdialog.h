#ifndef PARAMSDIALOG_H
#define PARAMSDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

#include "plotparams.h"
#include "comboboxdelegate.h"

static const int COLUMNS_NUM { 4 };

namespace Ui {
class paramsDialog;
}

class paramsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit paramsDialog(QVector<PlotParams*>* data, QWidget *parent = 0);
    ~paramsDialog();

private slots:
    void on_replot_released();
    void on_Cancel_released();

private:
    bool eventFilter(QObject *object, QEvent *event);
    void setupModel();
    void clearModel();
signals:
    void replot();
private:
    QVector<PlotParams*>* m_pdata;
    Ui::paramsDialog *ui;
    QStandardItemModel *model;
    ColorDelegate *pcolorDel;
    ListDelegate *pLsListDel;
    ListDelegate *pScListDel;
};



#endif // PARAMSDIALOG_H
