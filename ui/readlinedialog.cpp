
#include <QKeyEvent>

#include "readlinedialog.h"
#include "ui_readlinedialog.h"

ReadLineDialog::ReadLineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReadLineDialog)
{
    ui->setupUi(this);
    ui->lineEditUserInput->installEventFilter(this);
}

bool ReadLineDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->lineEditUserInput && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key = static_cast<QKeyEvent *>(event);
        if (key->key() == Qt::Key_Enter || key->key() == Qt::Key_Return) {
            setResult(DialogCode::Accepted);
            close();
        }
    }
    return QObject::eventFilter(obj, event);
}

ReadLineDialog::~ReadLineDialog()
{
    delete ui;
}

QString ReadLineDialog::GetUserInputLine()
{
    return ui->lineEditUserInput->text();
}
