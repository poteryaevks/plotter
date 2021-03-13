#ifndef PARAMSDIALOG_H
#define PARAMSDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

#include "plotparams.h"
#include "comboboxdelegate.h"

namespace Ui {
    class ParamsDialog;
}

class ParamsDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit ParamsDialog(QVector<PlotParams*>* data, QWidget *parent = 0);
        ~ParamsDialog();

    private:
        void setupModel();

        QVector<PlotParams*>* m_pdata;
        Ui::ParamsDialog *ui;
        QStandardItemModel *model;
        ListDelegate *pLsListDel;
        ListDelegate *pScListDel;

    private slots:
        void writeParams();
};



#endif // PARAMSDIALOG_H
