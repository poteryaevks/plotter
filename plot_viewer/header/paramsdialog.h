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

class PvParamsDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit PvParamsDialog(QList<PvPlotParams*> data, QWidget *parent = 0);
        ~PvParamsDialog();

    private:
        void setupModel();

        QList<PvPlotParams*> data_;
        Ui::ParamsDialog *ui;
        QStandardItemModel *model;
        ListDelegate *pLsListDel;
        ListDelegate *pScListDel;

    private slots:
        void writeParams();
        void showColorDialog(int row, int column);
};



#endif // PARAMSDIALOG_H
