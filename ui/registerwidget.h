
#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#define FROM_CPP
#include "../engine/sim.h"

#include <QWidget>
#include <QMouseEvent>
#include <QObject>


QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE


class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    RegisterWidget();
    void Initialize(simulator sim, void *mainWindow);
    void containerSizeChanged(int width);

    void paintEvent(QPaintEvent *) override;

    simulator sim;
    void *mainWindow = 0;
    bool hex = false;

protected:
    void showContextMenu(const QPoint &pos);
    void mousePressEvent(QMouseEvent *event) override
    {
        if(event->button() == Qt::RightButton)
            showContextMenu(event->pos());
        else
            QWidget::mousePressEvent(event);
    }

private slots:
    void changeRegister();

private:
    bool getIsGreen(int registerNumber);
    bool getIsFGreen(int registerNumber);
    int registerNumber(int x, int y);

    int textWidth = 0;
    int columns = 1;
    QPoint clickPos;
    int registerXPos[64];
    int registerYPos[64];
    int registerWidth = 0;
    int registerHeight = 0;
};

#endif // REGISTERWIDGET_H
