#ifndef MEMORYWIDGET_H
#define MEMORYWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QObject>

#define FROM_CPP
#include "../engine/sim.h"


class MemoryWidget : public QWidget
{
    Q_OBJECT

public:
    MemoryWidget();
    void Initialize(simulator sim, void *mainWindow);
    void containerSizeChanged(int width);

    void paintEvent(QPaintEvent *) override;

    simulator sim;
    void *mainWindow = 0;
    int address = 2000;

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
    void showInteger();
    void showFloat();

private:
    bool getIsGreen(unsigned long long address);
    int memoryByte(int x, int y);

    int textWidth = 0;
    int prefixTextWidth = 0;
    int singleByteTextWidth = 0;
    int twoHexDigitsTextWidth = 0;
    int columns = 1;
    QPoint clickPos;
    int wordXPos[260];
    int wordYPos[260];
    int wordWidth = 0;
    int wordHeight = 0;
};

#endif // MEMORYWIDGET_H
