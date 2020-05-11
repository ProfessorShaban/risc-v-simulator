
#include <QPainter>
#include <QtWidgets>

#include "disassemblerwidget.h"
#include "mainwindow.h"


DisassemblerWidget::DisassemblerWidget()
{
}

void DisassemblerWidget::paintEvent(QPaintEvent *)
{
    QPainter p{this};
    p.setRenderHint(QPainter::Antialiasing);

    QFont font ("Courier New");
    font.setPointSize(12);
    font.setBold(true);
    p.setFont(font);

    QColor green = QColor(180,255,180);

    QFontMetrics fm = QFontMetrics(font);
    QString column1Prefix("00000: ");
    int column1XIncrement = fm.width(column1Prefix);
    wordWidth = fm.width(QString ("00 00 00 00"));
    wordHeight = fm.height();

//???

    unsigned char *mem = get_memory(sim, address);
    int increment = 4;
    for (int i = 0; i < 1000; i += increment) {
        char chStr[100];
        sprintf (chStr, "%05d: %02x %02x %02x %02x", address + i, mem[i + 0], mem[i + 1], mem[i + 2], mem[i + 3]);

        QString *str = new QString(chStr);
        int x = 10;
        int y = 18;
        int row = i / increment;
        y += 18 * row;

        for (int j = i; j < i + increment; j++) {
            bool isGreen = getIsGreen(address + j);
            if (isGreen) {
                int xByteOffset = j % 4;
                int xExtra = ((j-i) / 4) * singleByteTextWidth / 2;
                p.fillRect(x - 2 + prefixTextWidth + xByteOffset * singleByteTextWidth + xExtra, y - 11, twoHexDigitsTextWidth + 4, 16, green);
            }
        }

        int yForArray = y - wordHeight / 2;
        wordXPos[i / 4] = x + column1XIncrement;
        wordYPos[i / 4] = yForArray;

        p.drawText(x, y, *str);
    }
}

bool DisassemblerWidget::getIsGreen(unsigned long long address)
{

//???

    MainWindow *theMainWindow = (MainWindow *) mainWindow;
    for (int i = 0; i < theMainWindow -> deltas_used; i++)
        if (theMainWindow -> deltas[i].type == memory_delta &&
                theMainWindow -> deltas[i].address <= address &&
                theMainWindow -> deltas[i].address + theMainWindow -> deltas[i].memory_bytes - 1 >= address)
            return true;
    return false;
}

void DisassemblerWidget::Initialize(simulator sim, void *mainWindow)
{
    this->sim = sim;
    this->mainWindow = mainWindow;
}

void DisassemblerWidget::containerSizeChanged(int width)
{
    if (textWidth == 0) {
        QFont font ("Courier New");
        font.setPointSize(12);
        font.setBold(true);
        QString maxString("00 00 00 00   ");
        QFontMetrics fm(font);
        textWidth = fm.width(maxString);

        QString prefixString("12345: ");
        prefixTextWidth = fm.width(prefixString);

        QString singleByteString("00 ");
        singleByteTextWidth = fm.width(singleByteString);

        QString twoHexDigitsString("00");
        twoHexDigitsTextWidth = fm.width(twoHexDigitsString);
    }

    // resize to one column
    resize(prefixTextWidth + textWidth + 10, 18 * (1000 / 4 + 1));

    update();
}
