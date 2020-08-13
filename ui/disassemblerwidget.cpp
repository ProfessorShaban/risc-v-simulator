
#include <QPainter>
#include <QtWidgets>

#include "disassemblerwidget.h"
#include "mainwindow.h"
#include "../engine/sim.h"


DisassemblerWidget::DisassemblerWidget()
{
    DeallocateMemory();
}

void DisassemblerWidget::DeallocateMemory() {
    if (lines != 0) {
        for (int line = 0; line < numLines; line++)
            free (lines[line]);
        free (lines);
    }

    numLines = 0;
}

void DisassemblerWidget::paintEvent(QPaintEvent *)
{
    QPainter p{this};
    p.setRenderHint(QPainter::Antialiasing);

    QFont font ("Courier New");
    font.setPointSize(12);
    font.setBold(true);
    p.setFont(font);

    QFontMetrics fm = QFontMetrics(font);
    QString column1Prefix("00000: ");
    int column1XIncrement = fm.horizontalAdvance(column1Prefix);
    wordHeight = fm.height();

    QString oneCharString("0");
    charWidth = fm.horizontalAdvance(oneCharString);

    int increment = 4;
    for (int i = 0; i < numLines * increment; i += increment) {
        int line = i / increment;
        QString *str = new QString(lines[line]);
        int x = 10;
        int y = 18;
        int row = i / increment;
        y += 18 * row;

        int yForArray = y - wordHeight / 2;
        wordXPos[i / 4] = x + column1XIncrement;
        wordYPos[i / 4] = yForArray;

        p.drawText(x, y, *str);
    }
}

bool DisassemblerWidget::getIsGreen(unsigned long long address)
{
    // for now, nothing is green
    return false;
}

void DisassemblerWidget::Initialize(simulator sim, void *mainWindow)
{
    this->sim = sim;
    this->mainWindow = mainWindow;
}

void DisassemblerWidget::containerSizeChanged(int width)
{
    update();
}

void DisassemblerWidget::refreshDisassembly()
{
    if (sim == 0)
        return;

    unsigned char *mem = get_memory(sim, address);
    int increment = 4;
    int longestLine = 0;
    int maxInstructions = 1000;

    // deallocate previous lines
    DeallocateMemory();

    for (int i = 0; i < maxInstructions; i += increment) {
        // disassemble until we see zeros
        if (mem[i+0] == 0 && mem[i+1] == 0 && mem[i+2] == 0 && mem[i+3] == 0)
            break;
        numLines ++;
    }

    lines = (char **) malloc(sizeof(char *) * (numLines + 1));
    for (int i = 0; i < numLines * increment; i += increment) {
        int line = i / increment;
        lines[line] = (char *) malloc(sizeof(char) * 150);

        char chInstruction [100];
        get_instruction_string (sim, address + i, chInstruction);
        sprintf (lines[line], "%05d: %02x %02x %02x %02x  %s", address + i, mem[i + 0], mem[i + 1], mem[i + 2], mem[i + 3], chInstruction);

        if ((int) strlen(lines[line]) > longestLine)
            longestLine = strlen(lines[line]);
    }

    // set width properly
    textWidth = charWidth * (longestLine + 3);
    resize(textWidth, 18 * (numLines + 1));
}
