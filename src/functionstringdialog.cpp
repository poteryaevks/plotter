#include "functionstringdialog.h"
#include "ui_functionstringdialog.h"

#include <QIcon>

functionStringDialog::functionStringDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::functionStringDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/graph.png"));
}

functionStringDialog::~functionStringDialog()
{
    delete ui;
}

void functionStringDialog::on_buttonBox_accepted()
{
    sendString(ui->expression->text());
}

void functionStringDialog::on_buttonBox_rejected()
{
    reject();
}
