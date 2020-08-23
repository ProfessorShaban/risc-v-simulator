
#include <QPainter>
#include <QtWidgets>

#include "disassemblerwidget.h"
#include "mainwindow.h"
#include "../engine/sim.h"
#include "../engine/sim2.h"


DisassemblerWidget::DisassemblerWidget()
{
    DeallocateMemory();
}

void DisassemblerWidget::DeallocateMemory() {
    for (int line = 0; line < numLines; line++)
        free (lines[line]);
    numLines = 0;
}

void DisassemblerWidget::paintEvent(QPaintEvent *)
{
    QColor red = QColor(255,180,180);

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

    QString bytesText("00 00 00 00");
    int bytesWidth = fm.horizontalAdvance(bytesText);

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
        wordXPos[line] = x + column1XIncrement;
        wordYPos[line] = yForArray;

        if (compareFlags[line])
            p.fillRect(x + column1XIncrement - 2, y - 11, bytesWidth + 4, 16, red);

        p.drawText(x, y, *str);
    }
}

bool DisassemblerWidget::getIsGreen(unsigned long long)
{
    // for now, nothing is green
    return false;
}

void DisassemblerWidget::Initialize(simulator sim, void *mainWindow)
{
    this->sim = sim;
    this->mainWindow = mainWindow;
}

void DisassemblerWidget::containerSizeChanged(int)
{
    update();
}

void DisassemblerWidget::refreshDisassembly()
{
    if (sim == 0)
        return;

    MainWindow *theMainWindow = (MainWindow*) mainWindow;

    unsigned char *mem = sim == theMainWindow -> sim2 ? get_memory2((simulator2 *) sim, 0) : get_memory(sim, 0);
    int increment = 4;
    int longestLine = 0;

    // deallocate previous lines
    DeallocateMemory();

    int address = 1000;
    numLines = 0;

    int num_of_instructions = theMainWindow -> num_of_instructions;
    assembly_instruction **instructions = theMainWindow -> instructions;
    if (sim == theMainWindow -> sim2) {
        num_of_instructions = theMainWindow -> num_of_instructions_sim2;
        instructions = theMainWindow -> instructions_sim2;
    }

    // if not initialized yet, return
    if (instructions == 0)
        return;

    for (int i = 0; i < num_of_instructions; i++) {
        assembly_instruction *instruction = instructions[i];

        // skip blank lines
        if (instruction ->address == 0)
            continue;

        if (instruction -> error == 0)
            while (address < (int) instruction -> address) {
                // print 4 bytes
                lines[numLines] = (char *) malloc(sizeof(char) * 150);
                sprintf (lines[numLines], "%05d: %02x %02x %02x %02x", address, mem[address + 0], mem[address + 1], mem[address + 2], mem[address + 3]);
                address += increment;
                numLines ++;
            }

        lines[numLines] = (char *) malloc(sizeof(char) * 150);
        char chInstruction [100];

        if (instruction -> error != 0) {
            // print 4 bytes
            sprintf (lines[numLines], "%05d: %02x %02x %02x %02x", address, mem[address + 0], mem[address + 1], mem[address + 2], mem[address + 3]);
        }
        else {
            // disassemble
            if (sim == theMainWindow -> sim2)
                get_instruction_string2 ((simulator2 *) sim, address, chInstruction);
            else
                get_instruction_string (sim, address, chInstruction);
            sprintf (lines[numLines], "%05d: %02x %02x %02x %02x  %s", address, mem[address + 0], mem[address + 1], mem[address + 2], mem[address + 3], chInstruction);
        }

        if ((int) strlen(lines[numLines]) > longestLine)
            longestLine = strlen(lines[numLines]);

        address += increment;
        numLines ++;

        if (numLines >= SHOW_MAX_LINES - 1)
            break;
    }

    // set width properly
    textWidth = charWidth * (longestLine + 3);
    resize(textWidth, 18 * (numLines + 1));
}

void DisassemblerWidget::compareTo(DisassemblerWidget *otherWidget) {
    for (int i = 0; i < SHOW_MAX_LINES; i++)
        compareFlags[i] = 0;

    for (int i = 0; i < numLines; i++) {
        if (i >= otherWidget -> numLines ||
                (i < otherWidget -> numLines && strcmp(lines[i], otherWidget -> lines[i])))
            compareFlags[i] = 1;
    }

    update();
}
