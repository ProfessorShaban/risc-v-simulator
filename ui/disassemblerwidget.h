#ifndef DISASSEMBLERWIDGET_H
#define DISASSEMBLERWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QObject>

#define FROM_CPP
#include "../engine/sim.h"


class DisassemblerWidget : public QWidget
{
    Q_OBJECT

public:
    DisassemblerWidget();
    void Initialize(simulator sim, void *mainWindow);
    void containerSizeChanged(int width);

    void paintEvent(QPaintEvent *) override;

    simulator sim;
    void *mainWindow = 0;
    int address = 2000;

private:
    bool getIsGreen(unsigned long long address);

    int textWidth = 0;
    int prefixTextWidth = 0;
    int singleByteTextWidth = 0;
    int twoHexDigitsTextWidth = 0;
    QPoint clickPos;
    int wordXPos[260];
    int wordYPos[260];
    int wordWidth = 0;
    int wordHeight = 0;
};

#endif // DISASSEMBLERWIDGET_H
