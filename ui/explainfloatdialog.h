#ifndef EXPLAINFLOATDIALOG_H
#define EXPLAINFLOATDIALOG_H

#include <QDialog>

#define FROM_CPP
#include "../engine/sim.h"


namespace Ui {
class explainfloatdialog;
}

class ExplainFloatWidget;


class explainfloatdialog : public QDialog
{
    Q_OBJECT

public:
    explicit explainfloatdialog(QWidget *parent = 0);
    ~explainfloatdialog();
    void Initialize(int address, unsigned char *memory);
    void widgetPaintEvent(QPaintEvent *);

private:
    void drawStringAt(QPainter *painter, const char *str, int x, int y, int center = 0);
    void drawRange(QPainter *painter, int bits[], int xBitPos[], int y, int firstBit, int lastBit, const char *name, int showExponent);

    Ui::explainfloatdialog *ui;
    unsigned char bytes[8];
    int address;
    ExplainFloatWidget *widget;
    int exponent = 0;
    double fraction = 0;
};


class ExplainFloatWidget : public QWidget
{
public:
    ExplainFloatWidget(explainfloatdialog *dialog) : QWidget(dialog) {
        explainFloatDialog = dialog;
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        explainFloatDialog->widgetPaintEvent(event);
    }

private:
    explainfloatdialog *explainFloatDialog;
};

#endif // EXPLAINFLOATDIALOG_H
