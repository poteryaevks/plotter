#ifndef FUNCTIONSTRINGDIALOG_H
#define FUNCTIONSTRINGDIALOG_H

#include <QDialog>

namespace Ui {
class functionStringDialog;
}

class FunctionStringDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FunctionStringDialog(QWidget *parent = 0);
    ~FunctionStringDialog();

signals:
    void sendString(const QString& expression);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::functionStringDialog *ui;
};

#endif // FUNCTIONSTRINGDIALOG_H
