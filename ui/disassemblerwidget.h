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
    void refreshDisassembly();

    void paintEvent(QPaintEvent *) override;

    simulator sim = 0;
    void *mainWindow = 0;
    int address = 1000;

private:
    bool getIsGreen(unsigned long long address);
    void DeallocateMemory();

    int charWidth = 20;
    int textWidth = 1024;
    int prefixTextWidth = 0;
    int singleByteTextWidth = 0;
    int twoHexDigitsTextWidth = 0;
    QPoint clickPos;
    int wordXPos[260];
    int wordYPos[260];
    int wordHeight = 0;

    char **lines = 0;
    int numLines = 0;
};

#endif // DISASSEMBLERWIDGET_H
