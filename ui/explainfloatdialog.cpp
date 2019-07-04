
#include <QtWidgets>

#include "explainfloatdialog.h"
#include "ui_explainfloatdialog.h"

explainfloatdialog::explainfloatdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::explainfloatdialog)
{
    ui->setupUi(this);
    widget = new ExplainFloatWidget(this);
    QRect cr = contentsRect();
    widget->setGeometry(QRect(cr.left() + 20, cr.top() + 80, cr.width() - 40, cr.height() - 150));
}

void explainfloatdialog::Initialize(int address, unsigned char *memory)
{
    this->address = address;
    for (int i = 0; i < 8; i++)
        bytes[i] = memory[i];

    char str [40];
    sprintf (str, "Address: %d", address);
    ui->labelAddress->setText(QString(str));
}

explainfloatdialog::~explainfloatdialog()
{
    delete ui;
}

void explainfloatdialog::drawStringAt(QPainter *painter, const char *str, int x, int y, int center)
{
    QString qStr = QString(str);

    if (center) {
        QFontMetrics fm (painter->font());
        int width = fm.width(qStr);
        x -= width/2;
    }

    painter->drawText(x, y, qStr);
}

void explainfloatdialog::drawRange(QPainter *painter, int bits[], int xBitPos[], int y, int firstBit, int lastBit, const char *name, int showExponent)
{
    int x1 = xBitPos[firstBit] + 9;
    int y1 = y - 7;
    int x2 = xBitPos[lastBit] - 1;
    int y2 = y - 7;
    painter->drawLine(x1, y1, x2, y2);
    painter->drawLine(x1, y1, x1, y1+30);
    painter->drawLine(x2, y2, x2, y2+30);
    painter->drawLine(x1, y1+30, x2, y2+30);

    int x = (xBitPos[lastBit] + xBitPos[firstBit]) / 2;
    drawStringAt(painter, name, x, y + 45, 1);
    char str[200];

    if (showExponent) {
        int num = 0;
        for (int i = lastBit; i >= firstBit; i--)
            num = num * 2 + bits[i];
        sprintf (str, "0x%03x", num);
        drawStringAt(painter, str, x, y + 63, 1);
        sprintf (str, "= %d", num);
        drawStringAt(painter, str, x, y + 81, 1);
        sprintf (str, "-1023 (bias)");
        drawStringAt(painter, str, x, y + 99, 1);
        num -= 1023;
        sprintf (str, "= %d", num);
        drawStringAt(painter, str, x, y + 117, 1);
        exponent = num;
    }
    else {
        sprintf (str, "1 + 0.");
        double fraction = 1;
        double power = 1;
        for (int i = lastBit; i >= firstBit; i--) {
            strcat (str, bits[i] ? "1" : "0");
            power *= 2;
            if (bits[i])
                fraction += (double) 1.0/power;
        }
        strcat (str, " (binary)");
        drawStringAt(painter, str, x, y + 63, 1);
        sprintf (str, "%f", fraction);
        drawStringAt(painter, str, x, y + 81, 1);
        this->fraction = fraction;
    }
}

void explainfloatdialog::widgetPaintEvent(QPaintEvent *)
{
    QPainter painter(widget);

    painter.setPen(Qt::black);

    int width = contentsRect().width() - 40;
    char str[400]; // double precision floating point number has max exponent of 308, I think
    int xBitPos[64];
    int bits[64];
    int index = 0;
    for (int i = 0; i < 8; i++) {
        int byte = bytes[i];
        sprintf(str, "0x%02x", byte);
        int xOffset = width - (i * (width / 8) + width / 16 + 24);
        drawStringAt(&painter, str, xOffset, 10);

        int x1 = xOffset + 12 - width / 8 / 2 + 7;
        int y1 = 20;
        int x2 = xOffset + 12 + width / 8 / 2 - 3;
        int y2 = 20;
        painter.drawLine(x1, y1, x2, y2);
        painter.drawLine(x1, y1, x1, y1+10);
        painter.drawLine(x2, y2, x2, y2+10);

        for (int j = 0; j < 8; j++) {
            sprintf(str, "%d", byte & 1);
            int furtherOffset = - (width / 16) + (8-j) * width / 8 / 9 - 12;
            int x = xOffset + 24 + furtherOffset;
            drawStringAt(&painter, str, x, 50);
            xBitPos[index] = x;
            bits[index++] = byte & 1;
            byte = byte >> 1;
        }
    }

    painter.setPen(Qt::magenta);
    // sign
    int xOffset = width - (3 * (width / 4) + width / 8 + 24);
    int x = xOffset + 12 - width / 4 / 2 + 15;
    int y = 60;
    painter.drawLine(x + 3, y, x + 3, y + 44);
    painter.drawLine(x + 4, y, x + 4, y + 44);
    QPainterPath path(QPointF(x+3, y));
    path.lineTo(x, y+6);
    path.lineTo(x+8, y+6);
    path.lineTo(x+4, y);
    painter.fillPath(path, QBrush(Qt::magenta));
    drawStringAt(&painter, "Sign", x - 3, y + 60);

    y -= 22;
    drawRange(&painter, bits, xBitPos, y, 0, 51, "Fraction (52 bits)", 0);
    drawRange(&painter, bits, xBitPos, y, 52, 62, "Exponent (11 bits)", 1);

    QFontMetrics fm (painter.font());
    y += 18 * 9;
    sprintf(str, "= %.20f * 2 ^ %d", (float) fraction, exponent);
    int strWidth = fm.horizontalAdvance(QString(str));
    drawStringAt(&painter, str, width - strWidth - 24, y);
    y += 18;
    sprintf(str, "= %.20f", fraction * pow(2,exponent));
    strWidth = fm.horizontalAdvance(QString(str));
    drawStringAt(&painter, str, width - strWidth - 24, y);
}
