#ifndef CHANGEREGISTERDIALOG_H
#define CHANGEREGISTERDIALOG_H

#include <QDialog>

#define FROM_CPP
#include "../engine/sim.h"


namespace Ui {
class changeregisterdialog;
}

class changeregisterdialog : public QDialog
{
    Q_OBJECT

public:
    explicit changeregisterdialog(QWidget *parent = 0);
    ~changeregisterdialog();
    void Initialize(int registerNumber, unsigned long long registerValue);
    void widgetPaintEvent(QPaintEvent *);
    unsigned long long GetRegisterValue();

private:
    Ui::changeregisterdialog *ui;
    int registerNumber;
    unsigned long long registerValue;
};

#endif // CHANGEREGISTERDIALOG_H
