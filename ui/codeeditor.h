
#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#define FROM_CPP
#include "../engine/sim.h"

#include <QPlainTextEdit>
#include <QObject>

typedef void *assembly_instruction_placeholder;


QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    int lineNumberToAddress(int lineNumber, bool *error);
    void lineNumberToErrorMessage(int lineNumber, char *error_message, int error_message_size);
    int addressToLineNumber(int address);
    void updateInstructions ();
    void initialize(void *mainWindow);
    void lineNumberAreaRightClickEvent(const QPoint &pos);
    assembly_instruction* instructionFromLineNumber(int lineNumber);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void ShowContextMenu(const QPoint &pos);
    bool event(QEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);
    void onTextChanged();
    void setPC();
    void toggleBreakpoint();
    void explainInstruction();

public:
    void doToggleBreakpoint();
    void ensurePCVisible();
    QPoint clickPos;

private:
    int lineNumberFromPos(const QPoint &pos);
    int addressFromLineNumber(int lineNumber);
    int lineNumberFromY(int y);

    QWidget *lineNumberArea;
    void *mainWindow;
    QImage *breakpointImage;
    QImage *pcImage;
    QImage *errorImage;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }

    QSize sizeHint() const override {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        codeEditor->lineNumberAreaPaintEvent(event);
    }
    void showContextMenu(const QPoint &pos);
    void toggleBreakpoint(const QPoint &pos);
    void mousePressEvent(QMouseEvent *event) override
    {
        if(event->button() == Qt::RightButton)
            showContextMenu(event->pos());
        else if (event->button() == Qt::LeftButton)
            toggleBreakpoint(event->pos());
        else
            QWidget::mousePressEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

#endif
