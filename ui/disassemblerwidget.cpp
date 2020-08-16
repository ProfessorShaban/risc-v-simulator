
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
    lines = 0;
}

void DisassemblerWidget::paintEvent(QPaintEvent *)
{
    if (lines == 0)
        return;

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

    unsigned char *mem = get_memory(sim, 0);
    int increment = 4;
    int longestLine = 0;

    // deallocate previous lines
    DeallocateMemory();

    MainWindow *theMainWindow = (MainWindow*) mainWindow;
    int address = 1000;
    numLines = 0;
    int arraySize = 10;
    lines = (char **) malloc(sizeof(char *) * (arraySize + 1));

    int num_of_instructions = theMainWindow -> num_of_instructions;
    assembly_instruction **instructions = theMainWindow -> instructions;
    if (sim == theMainWindow -> sim2) {
        num_of_instructions = theMainWindow -> num_of_instructions_sim2;
        instructions = theMainWindow -> instructions_sim2;
    }

    for (int i = 0; i < num_of_instructions; i++) {
        assembly_instruction *instruction = instructions[i];

        if (instruction -> error == 0)
            while (address < (int) instruction -> address) {
                // print 4 bytes
                lines[numLines] = (char *) malloc(sizeof(char) * 150);
                sprintf (lines[numLines], "%05d: %02x %02x %02x %02x", address, mem[address + 0], mem[address + 1], mem[address + 2], mem[address + 3]);
                address += increment;
                numLines ++;
                if (numLines + 1 >= arraySize) {
                    arraySize += 10;
                    lines = (char **) realloc (lines, sizeof (char *) * (arraySize + 1));
                }
            }

        lines[numLines] = (char *) malloc(sizeof(char) * 150);
        char chInstruction [100];

        if (instruction -> error != 0) {
            // print 4 bytes
            sprintf (lines[numLines], "%05d: %02x %02x %02x %02x", address, mem[address + 0], mem[address + 1], mem[address + 2], mem[address + 3]);
        }
        else {
            // disassemble
            get_instruction_string (sim, address, chInstruction);
            sprintf (lines[numLines], "%05d: %02x %02x %02x %02x  %s", address, mem[address + 0], mem[address + 1], mem[address + 2], mem[address + 3], chInstruction);
        }

        if ((int) strlen(lines[numLines]) > longestLine)
            longestLine = strlen(lines[numLines]);

        address += increment;
        numLines ++;

        if (numLines + 1 >= arraySize) {
            arraySize += 10;
            lines = (char **) realloc (lines, sizeof (char *) * (arraySize + 1));
        }
    }

    // set width properly
    textWidth = charWidth * (longestLine + 3);
    resize(textWidth, 18 * (numLines + 1));
}
