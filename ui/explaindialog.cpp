
#include <QtWidgets>

#include "explaindialog.h"
#include "ui_explaindialog.h"

explaindialog::explaindialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::explaindialog)
{
    ui->setupUi(this);
    widget = new ExplainWidget(this);
    QRect cr = contentsRect();
    widget->setGeometry(QRect(cr.left() + 20, cr.top() + 80, cr.width() - 40, cr.height() - 150));
}

void explaindialog::Initialize(assembly_instruction *instruction)
{
    this->instruction = instruction;

    char formatStr[100];
    if (instruction->format == 'J')
        sprintf(formatStr, "Format: UJ");
    else if (instruction->format == 'B')
        sprintf(formatStr, "Format: SB");
    else if (instruction->format == 'X')
        sprintf(formatStr, "Format: I (specialized)");
    else
        sprintf(formatStr, "Format: %c", instruction->format);
    ui->labelLine->setText(QString(instruction->source_line));
    ui->labelFormat->setText(QString(formatStr));
}

explaindialog::~explaindialog()
{
    delete ui;
}

void explaindialog::drawStringAt(QPainter *painter, const char *str, int x, int y, int center)
{
    QString qStr = QString(str);

    if (center) {
        QFontMetrics fm (painter->font());
        int width = fm.width(qStr);
        x -= width/2;
    }

    painter->drawText(x, y, qStr);
}

void explaindialog::drawRange(QPainter *painter, int bits[], int xBitPos[], int y, int firstBit, int lastBit, const char *name,
                              bool signedNumber)
{
    int x1 = xBitPos[firstBit] + 12;
    int y1 = y - 15;
    int x2 = xBitPos[lastBit] - 4;
    int y2 = y - 15;
    painter->drawLine(x1, y1, x2, y2);
    painter->drawLine(x1, y1, x1, y1+30);
    painter->drawLine(x2, y2, x2, y2+30);
    painter->drawLine(x1, y1+30, x2, y2+30);

    int x = (xBitPos[lastBit] + xBitPos[firstBit]) / 2;
    drawStringAt(painter, name, x, y + 45, 1 /* center */);

    char str[20];
    int num = 0;
    for (int i = lastBit; i >= firstBit; i--)
        num += bits[i] << (i - firstBit);
    sprintf (str, "0x%02x", num);
    drawStringAt(painter, str, x, y + 63, 1 /* center */);
    if (signedNumber && bits[lastBit])
        for (int i = lastBit - firstBit + 1; i < (int) (sizeof(int) * 8); i++)
            num += 1 << i;
    sprintf (str, "%d", num);
    drawStringAt(painter, str, x, y + 81, 1 /* center */);
}

void explaindialog::drawRanges(QPainter *painter, int *bits, int *xBitPos, int y)
{
    if (instruction -> format == 'R') {
        drawRange(painter, bits, xBitPos, y, 0, 6, "opcode");
        drawRange(painter, bits, xBitPos, y, 7, 11, "rd");
        drawRange(painter, bits, xBitPos, y, 12, 14, "funct3");
        drawRange(painter, bits, xBitPos, y, 15, 19, "rs1");
        drawRange(painter, bits, xBitPos, y, 20, 24, "rs2");
        drawRange(painter, bits, xBitPos, y, 25, 31, "funct7");
    }
    if (instruction -> format == 'I') {
        drawRange(painter, bits, xBitPos, y, 0, 6, "opcode");
        drawRange(painter, bits, xBitPos, y, 7, 11, "rd");
        drawRange(painter, bits, xBitPos, y, 12, 14, "funct3");
        drawRange(painter, bits, xBitPos, y, 15, 19, "rs1");
        drawRange(painter, bits, xBitPos, y, 20, 31, "imm[11:0]", true /* signedNumber */);
    }
    if (instruction -> format == 'X') {
        drawRange(painter, bits, xBitPos, y, 0, 6, "opcode");
        drawRange(painter, bits, xBitPos, y, 7, 11, "rd");
        drawRange(painter, bits, xBitPos, y, 12, 14, "funct3");
        drawRange(painter, bits, xBitPos, y, 15, 19, "rs1");
        drawRange(painter, bits, xBitPos, y, 20, 31, "imm[11:0] (special I format, see manual)");
    }
    if (instruction -> format == 'S') {
        drawRange(painter, bits, xBitPos, y, 0, 6, "opcode");
        drawRange(painter, bits, xBitPos, y, 7, 11, "imm[4:0]");
        drawRange(painter, bits, xBitPos, y, 12, 14, "funct3");
        drawRange(painter, bits, xBitPos, y, 15, 19, "rs1");
        drawRange(painter, bits, xBitPos, y, 20, 24, "rs2");
        drawRange(painter, bits, xBitPos, y, 25, 31, "imm[11:5]");
    }
    if (instruction -> format == 'B') {
        drawRange(painter, bits, xBitPos, y, 0, 6, "opcode");
        drawRange(painter, bits, xBitPos, y, 7, 11, "imm[4:1|11]");
        drawRange(painter, bits, xBitPos, y, 12, 14, "funct3");
        drawRange(painter, bits, xBitPos, y, 15, 19, "rs1");
        drawRange(painter, bits, xBitPos, y, 20, 24, "rs2");
        drawRange(painter, bits, xBitPos, y, 25, 31, "imm[12|10:5]");
    }
    if (instruction -> format == 'U') {
        drawRange(painter, bits, xBitPos, y, 0, 6, "opcode");
        drawRange(painter, bits, xBitPos, y, 7, 11, "rd");
        drawRange(painter, bits, xBitPos, y, 12, 31, "imm[31:12]");
    }
    if (instruction -> format == 'J') {
        drawRange(painter, bits, xBitPos, y, 0, 6, "opcode");
        drawRange(painter, bits, xBitPos, y, 7, 11, "rd");
        drawRange(painter, bits, xBitPos, y, 12, 31, "imm[20|10:1|11|19:12]");
    }
}

void explaindialog::widgetPaintEvent(QPaintEvent *)
{
    QPainter painter(widget);

    painter.setPen(Qt::black);

    int width = contentsRect().width() - 40;
    char str[200];
    int instructionWord = instruction->instruction;
    int xBitPos[32];
    int bits[32];
    int index = 0;
    for (int i = 0; i < 4; i++) {
        int byte = instructionWord & 0xff;
        sprintf(str, "0x%02x", byte);
        int xOffset = width - (i * (width / 4) + width / 8 + 24);
        drawStringAt(&painter, str, xOffset, 30);

        int x1 = xOffset + 12 - width / 4 / 2 + 14;
        int y1 = 40;
        int x2 = xOffset + 12 + width / 4 / 2 - 6;
        int y2 = 40;
        painter.drawLine(x1, y1, x2, y2);
        painter.drawLine(x1, y1, x1, y1+10);
        painter.drawLine(x2, y2, x2, y2+10);

        for (int j = 0; j < 8; j++) {
            sprintf(str, "%d", byte & 1);
            int furtherOffset = - (width / 8) + (8-j) * width / 4 / 9 - 12;
            int x = xOffset + 24 + furtherOffset;
            drawStringAt(&painter, str, x, 74);
            xBitPos[index] = x;
            bits[index++] = byte & 1;
            byte = byte >> 1;
        }

        instructionWord = instructionWord >> 8;
    }

    painter.setPen(Qt::magenta);
    drawRanges(&painter, bits, xBitPos, 70);
}
