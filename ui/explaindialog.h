#ifndef EXPLAINDIALOG_H
#define EXPLAINDIALOG_H

#include <QDialog>

#define FROM_CPP
#include "../engine/sim.h"


namespace Ui {
class explaindialog;
}

class ExplainWidget;


class explaindialog : public QDialog
{
    Q_OBJECT

public:
    explicit explaindialog(QWidget *parent = 0);
    ~explaindialog();
    void Initialize(assembly_instruction *instruction);
    void widgetPaintEvent(QPaintEvent *);

private:
    void drawStringAt(QPainter *painter, const char *str, int x, int y, int center = 0);
    void drawRanges(QPainter *painter, int *bits, int *xBitPos, int y);
    void drawRange(QPainter *painter, int *bits, int *xBitPos, int y, int firstBit, int lastBit, const char *name, bool signedNumber = false);

    Ui::explaindialog *ui;
    assembly_instruction *instruction = 0;
    ExplainWidget *widget;
};


class ExplainWidget : public QWidget
{
public:
    ExplainWidget(explaindialog *dialog) : QWidget(dialog) {
        explainDialog = dialog;
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        explainDialog->widgetPaintEvent(event);
    }

private:
    explaindialog *explainDialog;
};

#endif // EXPLAINDIALOG_H
