
#include <QtWidgets>

#include "explainintdialog.h"
#include "ui_explainintdialog.h"

explainintdialog::explainintdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::explainintdialog)
{
    ui->setupUi(this);
    widget = new ExplainIntWidget(this);
    QRect cr = contentsRect();
    widget->setGeometry(QRect(cr.left() + 20, cr.top() + 80, cr.width() - 40, cr.height() - 150));
}

void explainintdialog::Initialize(int address, unsigned char *memory)
{
    this->address = address;
    for (int i = 0; i < 4; i++)
        bytes[i] = memory[i];

    char str [40];
    sprintf (str, "Address: %d", address);
    ui->labelAddress->setText(QString(str));
}

explainintdialog::~explainintdialog()
{
    delete ui;
}

void explainintdialog::drawStringAt(QPainter *painter, const char *str, int x, int y, int center)
{
    QString qStr = QString(str);

    if (center) {
        QFontMetrics fm (painter->font());
        int width = fm.width(qStr);
        x -= width/2;
    }

    painter->drawText(x, y, qStr);
}

void explainintdialog::widgetPaintEvent(QPaintEvent *)
{
    QPainter painter(widget);

    int value = *(int *) (&bytes[0]);

    painter.setPen(Qt::black);

    int width = contentsRect().width() - 40;
    char str[200];
    for (int i = 0; i < 4; i++) {
        int byte = bytes[i];
        sprintf(str, "0x%02x", byte);
        int xOffset = width - (i * (width / 4) + width / 8 + 24);
        drawStringAt(&painter, str, xOffset, 10);

        int x1 = xOffset + 12 - width / 4 / 2 + 14;
        int y1 = 20;
        int x2 = xOffset + 12 + width / 4 / 2 - 6;
        int y2 = 20;
        painter.drawLine(x1, y1, x2, y2);
        painter.drawLine(x1, y1, x1, y1+10);
        painter.drawLine(x2, y2, x2, y2+10);

        for (int j = 0; j < 8; j++) {
            sprintf(str, "%d", byte & 1);
            int furtherOffset = - (width / 8) + (8-j) * width / 4 / 9 - 12;
            int x = xOffset + 24 + furtherOffset;
            drawStringAt(&painter, str, x, 50);
            byte = byte >> 1;
        }
    }

    painter.setPen(Qt::magenta);

    // sign
    int xOffset = width - (3 * (width / 4) + width / 8 + 24);
    int x = xOffset + 12 - width / 4 / 2 + 20;
    int y = 60;
    painter.drawLine(x + 3, y, x + 3, y + 16);
    painter.drawLine(x + 4, y, x + 4, y + 16);
    QPainterPath path(QPointF(x+3, y));
    path.lineTo(x, y+6);
    path.lineTo(x+8, y+6);
    path.lineTo(x+4, y);
    painter.fillPath(path, QBrush(Qt::magenta));
    drawStringAt(&painter, "Sign", x - 8, y + 32);

    QFontMetrics fm (painter.font());
    y += 48;
    if (value >= 0) {
        sprintf(str, "0x%08x", value);
        int strWidth = fm.width(QString(str));
        drawStringAt(&painter, str, width - strWidth - 24, y);
        sprintf(str, "%d", value);
        strWidth = fm.width(QString(str));
        drawStringAt(&painter, str, width - strWidth - 24, y + 20);
    }
    else {
        y += 10;
        painter.drawLine(0, y, width - 10, y);
        y += 18;

        sprintf(str, "Inverted:");
        drawStringAt(&painter, str, x - 8, y);

        for (int i = 0; i < 4; i++) {
            int byte = bytes[i];
            for (int j = 0; j < 8; j++) {
                sprintf(str, "%d", (byte & 1) ? 0 : 1);
                int xOffset = width - (i * (width / 4) + width / 8 + 24);
                int furtherOffset = - (width / 8) + (8-j) * width / 4 / 9 - 12;
                int x = xOffset + 24 + furtherOffset;
                drawStringAt(&painter, str, x, y + 18);
                byte = byte >> 1;
            }
        }

        y += 40;
        sprintf(str, "+1");
        int strWidth = fm.width(QString(str));
        drawStringAt(&painter, str, width - strWidth - 24, y);
        y += 20;
        sprintf(str, "Result: %d", value);
        strWidth = fm.width(QString(str));
        drawStringAt(&painter, str, width - strWidth - 24, y);
    }
}
