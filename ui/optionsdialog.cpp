#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBoxOkCancel, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBoxOkCancel, SIGNAL(rejected()), this, SLOT(reject()));
}

void OptionsDialog::Initialize(int address)
{
    QString addressStr = QString::number(address);
    ui->lineEditMemoryPaneStartAddress->setText(addressStr);
}

int OptionsDialog::GetStartAddress()
{
    QString addressStr = ui->lineEditMemoryPaneStartAddress->text();
    int address = addressStr.toInt();
    return address;
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}
