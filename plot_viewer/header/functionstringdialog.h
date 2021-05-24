#ifndef FUNCTIONSTRINGDIALOG_H
#define FUNCTIONSTRINGDIALOG_H

#include <QDialog>

namespace Ui {
class functionStringDialog;
}

class functionStringDialog : public QDialog
{
    Q_OBJECT
public:
    explicit functionStringDialog(QWidget *parent = 0);
    ~functionStringDialog();

signals:
    void sendString(const QString& expression);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::functionStringDialog *ui;
};

#endif // FUNCTIONSTRINGDIALOG_H
