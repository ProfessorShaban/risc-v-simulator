#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>

#define FROM_CPP
#include "../engine/sim.h"


class RegisterWidget : public QWidget
{
public:
    RegisterWidget();
    void Initialize(simulator sim, void *mainWindow);
    void containerSizeChanged(int width);

    void paintEvent(QPaintEvent *);

    simulator sim;
    void *mainWindow = 0;
    bool hex = false;

private:
    bool getIsGreen(int registerNumber);
    bool getIsFGreen(int registerNumber);

    int textWidth = 0;
    int columns = 1;
};

#endif // REGISTERWIDGET_H
