
#include <QPainter>
#include <QtWidgets>

#include "memorywidget.h"
#include "mainwindow.h"
#include "explainintdialog.h"
#include "explainfloatdialog.h"


MemoryWidget::MemoryWidget()
{
}

void MemoryWidget::paintEvent(QPaintEvent *)
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
    QString column2Prefix("00000: 00 00 00 00   ");
    int column2XIncrement = fm.width(column2Prefix);
    QString column3Prefix("00000: 00 00 00 00   00 00 00 00   ");
    int column3XIncrement = fm.width(column3Prefix);
    QString column4Prefix("00000: 00 00 00 00   00 00 00 00   00 00 00 00   ");
    int column4XIncrement = fm.width(column4Prefix);
    wordWidth = fm.width(QString ("00 00 00 00"));
    wordHeight = fm.height();

    unsigned char *mem = get_memory(sim, address);
    int increment = columns == 1 ? 4 : (columns == 2 ? 8 : 16);
    for (int i = 0; i < 1000; i += increment) {
        char chStr[100];
        if (columns == 1)
            sprintf (chStr, "%05d: %02x %02x %02x %02x", address + i, mem[i + 0], mem[i + 1], mem[i + 2], mem[i + 3]);
        if (columns == 2)
            sprintf (chStr, "%05d: %02x %02x %02x %02x   %02x %02x %02x %02x", address + i,
                    mem[i + 0], mem[i + 1], mem[i + 2], mem[i + 3],
                    mem[i + 4], mem[i + 5], mem[i + 6], mem[i + 7]);
        if (columns == 4)
            sprintf (chStr, "%05d: %02x %02x %02x %02x   %02x %02x %02x %02x   %02x %02x %02x %02x   %02x %02x %02x %02x\r\n", address + i,
                    mem[i + 0], mem[i + 1], mem[i + 2], mem[i + 3],
                    mem[i + 4], mem[i + 5], mem[i + 6], mem[i + 7],
                    mem[i + 8], mem[i + 9], mem[i +10], mem[i +11],
                    mem[i +12], mem[i +13], mem[i +14], mem[i +15]);

        QString *str = new QString(chStr);
        int x = 10;
        int y = 18;
        int row = i / increment;
        y += 18 * row;

        for (int j = i; j < i + increment; j++) {
            bool isGreen = getIsGreen(address + j);
            if (isGreen) {
                int xByteOffset = (j % (columns * 4));
                int xExtra = ((j-i) / 4) * singleByteTextWidth / 2;
                p.fillRect(x - 2 + prefixTextWidth + xByteOffset * singleByteTextWidth + xExtra, y - 11, twoHexDigitsTextWidth + 4, 16, green);
            }
        }

        int yForArray = y - wordHeight / 2;
        wordXPos[i / 4] = x + column1XIncrement;
        wordYPos[i / 4] = yForArray;
        if (columns == 2) {
            wordXPos[i / 4 + 1] = x + column2XIncrement;
            wordYPos[i / 4 + 1] = yForArray;
        }
        if (columns == 4) {
            wordXPos[i / 4 + 1] = x + column2XIncrement;
            wordYPos[i / 4 + 1] = yForArray;
            wordXPos[i / 4 + 2] = x + column3XIncrement;
            wordYPos[i / 4 + 2] = yForArray;
            wordXPos[i / 4 + 3] = x + column4XIncrement;
            wordYPos[i / 4 + 3] = yForArray;
        }

        p.drawText(x, y, *str);
    }
}

bool MemoryWidget::getIsGreen(unsigned long long address)
{
    MainWindow *theMainWindow = (MainWindow *) mainWindow;
    for (int i = 0; i < theMainWindow -> deltas_used; i++)
        if (theMainWindow -> deltas[i].type == memory_delta &&
                theMainWindow -> deltas[i].address <= address &&
                theMainWindow -> deltas[i].address + theMainWindow -> deltas[i].memory_bytes - 1 >= address)
            return true;
    return false;
}

void MemoryWidget::Initialize(simulator sim, void *mainWindow)
{
    this->sim = sim;
    this->mainWindow = mainWindow;
}

void MemoryWidget::containerSizeChanged(int width)
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

    int numColumns = (width - prefixTextWidth - 10 - 20) / textWidth;
    if (numColumns <= 1) {
        columns = 1;
        resize(prefixTextWidth + textWidth + 10, 18 * (1000 / 4 + 1));
    }
    else if (numColumns < 4) {  // 2 columns
        columns = 2;
        resize(prefixTextWidth + textWidth * 2, 18 * (1000 / 8 + 1));
    }
    else {    // 4 columns
        columns = 4;
        resize(prefixTextWidth + textWidth * 4, 18 * (1000 / 16 + 1));
    }

    update();
}

void MemoryWidget::showContextMenu(const QPoint &pos)
{
    clickPos = pos;

    int enable = memoryByte(clickPos.x(), clickPos.y()) != -1;

    QMenu contextMenu(tr("MemoryContextMenu"), this);

    QAction showIntegerAct("Explain Signed Integer (Word)", this);
    connect(&showIntegerAct, SIGNAL(triggered()), this, SLOT(showInteger()));
    showIntegerAct.setEnabled(enable);
    contextMenu.addAction(&showIntegerAct);

    QAction showFloatAct("Explain Floating Point Number (Doubleword)", this);
    connect(&showFloatAct, SIGNAL(triggered()), this, SLOT(showFloat()));
    showFloatAct.setEnabled(enable);
    contextMenu.addAction(&showFloatAct);

    contextMenu.exec(mapToGlobal(pos));
}

int MemoryWidget::memoryByte(int x, int y)
{
    for (int i = 0; i < 250; i++) {
        if (x >= wordXPos[i] && x <= wordXPos[i] + wordWidth &&
                y >= wordYPos[i] && y <= wordYPos[i] + wordHeight)
            return i * 4;

        if (y < wordYPos[i] - wordHeight * 3)
            break;  // if we've passed the current row, no point in checking further
    }
    return -1;
}

void MemoryWidget::showInteger()
{
    int byte = memoryByte(clickPos.x(), clickPos.y());
    if (byte == -1)
        return;
    byte += address;
    unsigned char *mem = get_memory(sim, byte);

    explainintdialog *dialog = new explainintdialog(this /* parent */);
    dialog -> Initialize(byte, mem);
    dialog -> exec();
}

void MemoryWidget::showFloat()
{
    int byte = memoryByte(clickPos.x(), clickPos.y());
    if (byte == -1)
        return;
    if (byte % 8 == 4)
        byte -= 4;
    byte += address;
    unsigned char *mem = get_memory(sim, byte);

    explainfloatdialog *dialog = new explainfloatdialog(this /* parent */);
    dialog -> Initialize(byte, mem);
    dialog -> exec();
}
