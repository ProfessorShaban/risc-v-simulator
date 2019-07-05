
#include <QPainter>
#include <QtWidgets>

#include "registerwidget.h"
#include "mainwindow.h"
#include "changeregisterdialog.h"


RegisterWidget::RegisterWidget()
{
    QString maxString("x01: 0x0000000000000000");
    QFont font ("Courier New");
    QFontMetrics fm(font);
    int textWidth = fm.width(maxString);
    registerWidth = textWidth + 4;
    registerHeight = 16;
}

void RegisterWidget::paintEvent(QPaintEvent *)
{
    QPainter p{this};
    p.setRenderHint(QPainter::Antialiasing);

    QFont font ("Courier New");
    font.setPointSize(12);
    font.setBold(true);
    p.setFont(font);

    QString maxString("x01: 0x0000000000000000");
    QFontMetrics fm(font);
    int textWidth = fm.width(maxString);

    QColor green = QColor(180,255,180);

    char chStr[100];
    sprintf (chStr, "PC:  %d", static_cast<int>(get_pc(sim)));
    QString *str = new QString(chStr);
    p.fillRect(10 - 2, 16 - 11, textWidth + 4, 16, green);
    p.drawText(10, 16, *str);

    unsigned long long *registers = get_registers(sim);
    for (int i = 0; i < 32; i++) {
        char padded[100];
        int max_width = 18;
        if (hex)
            sprintf(padded, "0x%016llx", registers[i]);
        else
            sprintf(padded, "%*lld", max_width, registers[i]);

        // move spaces to the back
        size_t nonspaceIndex = 0;
        while (nonspaceIndex < strlen(padded) && padded[nonspaceIndex] == ' ')
            nonspaceIndex++;
        strcpy(padded, &padded[nonspaceIndex]);
        size_t len = strlen(padded);
        for (size_t i = 0; i < nonspaceIndex; i++)
            padded[len + i] = ' ';
        padded[len + nonspaceIndex] = 0;

        char chStr[100];
        if (hex)
            sprintf (chStr, "x%02d: %s ", i, padded);
        else
            sprintf (chStr, "x%02d: %s ", i, padded);

        QString *str = new QString(chStr);
        bool isGreen = getIsGreen(i);
        int x = 10;
        int y = 16 + 18 * 2;
        if (columns == 1) {
            y += 18 * i;
        }
        if (columns == 2) {
            x += (i % 2) * (textWidth + 20);
            y += 18 * (i / 2);
        }
        if (columns == 4) {
            x += (i % 4) * (textWidth + 20);
            y += 18 * (i / 4);
        }
        if (isGreen)
            p.fillRect(x - 2, y - 11, textWidth + 4, 16, green);
        p.drawText(x, y, *str);
        registerXPos[i] = x - 2;
        registerYPos[i] = y - 11;
    }

    double *fregisters = get_fregisters(sim);
    for (int i = 0; i < 32; i++) {

        char padded [25];
        sprintf (padded, "%18.8g", fregisters[i]);

        // move spaces to the back
        size_t nonspaceIndex = 0;
        while (nonspaceIndex < strlen(padded) && padded[nonspaceIndex] == ' ')
            nonspaceIndex++;
        strcpy(padded, &padded[nonspaceIndex]);
        size_t len = static_cast<size_t>(strlen(padded));
        for (size_t i = 0; i < nonspaceIndex; i++)
            padded[len + i] = ' ';
        padded[len + nonspaceIndex] = 0;

        char chStr[100];
        sprintf (chStr, "f%02d: %s ", i, padded);
        QString *str = new QString(chStr);
        bool isGreen = getIsFGreen(i);
        int x = 10;
        int y = 16 + 18 * 2;
        if (columns == 1) {
            y += 18 * i + (18 * 33);
        }
        if (columns == 2) {
            x += (i % 2) * (textWidth + 20);
            y += 18 * 17;
            y += 18 * (i / 2);
        }
        if (columns == 4) {
            x += (i % 4) * (textWidth + 20);
            y += 18 * 9;
            y += 18 * (i / 4);
        }
        if (isGreen)
            p.fillRect(x - 2, y - 11, textWidth + 4, 16, green);
        p.drawText(x, y, *str);
        registerXPos[i+32] = x - 2;
        registerYPos[i+32] = y - 11;
    }
}

bool RegisterWidget::getIsGreen(int registerNumber)
{
    MainWindow *theMainWindow = static_cast<MainWindow *>(mainWindow);
    for (int i = 0; i < theMainWindow -> deltas_used; i++)
        if (theMainWindow -> deltas[i].type == register_delta && theMainWindow -> deltas[i].register_number == registerNumber)
            return true;
    return false;
}

bool RegisterWidget::getIsFGreen(int registerNumber)
{
    MainWindow *theMainWindow = static_cast<MainWindow *>(mainWindow);
    for (int i = 0; i < theMainWindow -> deltas_used; i++)
        if (theMainWindow -> deltas[i].type == fregister_delta && theMainWindow -> deltas[i].register_number == registerNumber)
            return true;
    return false;
}

void RegisterWidget::Initialize(simulator sim, void *mainWindow)
{
    this->sim = sim;
    this->mainWindow = mainWindow;
}

void RegisterWidget::containerSizeChanged(int width)
{
    if (textWidth == 0) {
        QFont font ("Courier New");
        font.setPointSize(12);
        font.setBold(true);
        QString maxString("x01: 0x0000000000000000");
        QFontMetrics fm(font);
        textWidth = fm.width(maxString);
    }

    int numColumns = width / (textWidth + 32);
    if (numColumns <= 1) {
        columns = 1;
        resize(textWidth + 24, 1220);
    }
    else if (numColumns < 4) {  // 2 columns
        columns = 2;
        resize((textWidth + 24) * 2 - 10, 660);
    }
    else {    // 4 columns
        columns = 4;
        resize((textWidth + 24) * 4, 360);
    }

    update();
}

int RegisterWidget::registerNumber(int x, int y)
{
    for (int i = 0; i < 64; i++)
        if (x >= registerXPos[i] && x <= registerXPos[i] + registerWidth && y >= registerYPos[i] && y <= registerYPos[i] + registerHeight)
            return i;
    return -1;
}

void RegisterWidget::showContextMenu(const QPoint &pos)
{
    clickPos = pos;

    int enable = registerNumber(clickPos.x(), clickPos.y()) != -1;

    QMenu contextMenu(tr("RegisterContextMenu"), this);

    QAction changeRegisterAct("Change This Register", this);
    connect(&changeRegisterAct, SIGNAL(triggered()), this, SLOT(changeRegister()));
    changeRegisterAct.setEnabled(enable);
    contextMenu.addAction(&changeRegisterAct);

    contextMenu.exec(mapToGlobal(pos));
}

void RegisterWidget::changeRegister()
{
    int registerNum = registerNumber(clickPos.x(), clickPos.y());
    if (registerNum == -1)
        return;

    if (registerNum < 32) {
        unsigned long long *registers = get_registers(sim);
        unsigned long long registerValue = registers[registerNum];

        changeregisterdialog *dialog = new changeregisterdialog(this /* parent */);
        dialog -> Initialize(registerNum, registerValue);
        if (dialog -> exec() == QDialog::Accepted)
            registers[registerNum] = dialog -> GetRegisterValue();
    }
    else {
        registerNum -= 32;

        double *fregisters = get_fregisters(sim);
        double registerValue = fregisters[registerNum];

        changeregisterdialog *dialog = new changeregisterdialog(this /* parent */);
        dialog -> InitializeDouble(registerNum, registerValue);
        if (dialog -> exec() == QDialog::Accepted)
            fregisters[registerNum] = dialog -> GetRegisterValueDouble();
    }
}
