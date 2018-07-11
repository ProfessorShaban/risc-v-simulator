#ifndef EXPLAININTDIALOG_H
#define EXPLAININTDIALOG_H

#include <QDialog>

#define FROM_CPP
#include "../engine/sim.h"


namespace Ui {
class explainintdialog;
}

class ExplainIntWidget;


class explainintdialog : public QDialog
{
    Q_OBJECT

public:
    explicit explainintdialog(QWidget *parent = 0);
    ~explainintdialog();
    void Initialize(int address, unsigned char *memory);
    void widgetPaintEvent(QPaintEvent *);

private:
    void drawStringAt(QPainter *painter, const char *str, int x, int y, int center = 0);

    Ui::explainintdialog *ui;
    unsigned char bytes[4];
    int address;
    ExplainIntWidget *widget;
};


class ExplainIntWidget : public QWidget
{
public:
    ExplainIntWidget(explainintdialog *dialog) : QWidget(dialog) {
        explainIntDialog = dialog;
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        explainIntDialog->widgetPaintEvent(event);
    }

private:
    explainintdialog *explainIntDialog;
};

#endif // EXPLAININTDIALOG_H
