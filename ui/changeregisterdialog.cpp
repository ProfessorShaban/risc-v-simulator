
#include "changeregisterdialog.h"
#include "ui_changeregisterdialog.h"


changeregisterdialog::changeregisterdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changeregisterdialog)
{
    ui->setupUi(this);

    connect(ui->buttonBoxOkCancel, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBoxOkCancel, SIGNAL(rejected()), this, SLOT(reject()));
}

void changeregisterdialog::Initialize(int registerNumber, unsigned long long registerValue)
{
    this->registerNumber = registerNumber;
    this->registerValue = registerValue;
    QString registerValueStr = QString::number(registerValue);
    ui->registerValue->setText(registerValueStr);
}

unsigned long long changeregisterdialog::GetRegisterValue()
{
    QString registerValueStr = ui->registerValue->text();
    this->registerValue = registerValueStr.toLongLong();
    return this->registerValue;
}

changeregisterdialog::~changeregisterdialog()
{
    delete ui;
}
