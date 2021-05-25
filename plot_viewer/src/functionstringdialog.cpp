#include "functionstringdialog.h"
#include "ui_functionstringdialog.h"

#include <QIcon>

FunctionStringDialog::FunctionStringDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::functionStringDialog)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/graph.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

FunctionStringDialog::~FunctionStringDialog()
{
    delete ui;
}

void FunctionStringDialog::on_buttonBox_accepted()
{
    sendString(ui->expression->text());
}

void FunctionStringDialog::on_buttonBox_rejected()
{
    reject();
}
