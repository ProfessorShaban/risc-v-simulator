
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

void changeregisterdialog::InitializeDouble(int registerNumber, double registerValue)
{
    this->registerNumber = registerNumber;
    this->registerValueDouble = registerValue;
    QString registerValueStr = QString::number(registerValue);
    ui->registerValue->setText(registerValueStr);
}

unsigned long long changeregisterdialog::GetRegisterValue()
{
    QString registerValueStr = ui->registerValue->text();

    if (registerValueStr.length() > 2 && registerValueStr[0] == '0' && (registerValueStr[1] == 'x' || registerValueStr[1] == 'X')) {
        unsigned long long answer = 0;
        for (int i = 2; i < registerValueStr.length(); i++) {
            char ch = (char) registerValueStr[i].toLatin1();
            ch = tolower(ch);
            int value = ch - '0';
            if (ch <= 'f' && ch >= 'a')
                value = 10 + ch - 'a';
            answer = answer * 16 + value;
        }
        this->registerValue = answer;
    }
    else
        this->registerValue = registerValueStr.toLongLong();
    return this->registerValue;
}

double changeregisterdialog::GetRegisterValueDouble()
{
    QString registerValueStr = ui->registerValue->text();
    this->registerValueDouble = registerValueStr.toDouble();
    return this->registerValueDouble;
}

changeregisterdialog::~changeregisterdialog()
{
    delete ui;
}
