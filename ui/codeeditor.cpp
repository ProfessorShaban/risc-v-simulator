
#include <QtWidgets>

#include "codeeditor.h"

#define FROM_CPP
#include "../engine/utils.h"
#include "../engine/sim.h"

#include "mainwindow.h"
#include "explaindialog.h"


CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    QFont font = document()->defaultFont();
    font.setFamily("Courier New");
    font.setPointSize(11);
    document()->setDefaultFont(font);

    breakpointImage = new QImage(":/images/breakpoint.png");
    pcImage = new QImage(":/images/pc.png");
    errorImage = new QImage(":/images/error.png");

    setMouseTracking(true);
}

void CodeEditor::initialize(void *mainWindow)
{
    this->mainWindow = mainWindow;
    document()->clear();
}

bool CodeEditor::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {
           QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
           int x = helpEvent->pos().x();
           int y = helpEvent->pos().y();

           // we are only interested in error message icon
           if (x >= 12 && x <= 24) {

               int lineNumber = lineNumberFromY(y);

               char error_message[250];
               lineNumberToErrorMessage(lineNumber, error_message, sizeof(error_message)-1);

               if (error_message[0]) {
                   QString toolTipStr = QString(error_message);
                   QToolTip::showText(helpEvent->globalPos(), toolTipStr);
               } else {
                   QToolTip::hideText();
                   event->ignore();
               }
               return true;
           }
       }
       return QPlainTextEdit::event(event);
}

int CodeEditor::lineNumberFromY(int y)
{
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    while (block.isValid()) {
        if (block.isVisible()) {
            int lineNumber = blockNumber + 1;
            if (y >= top && y <= bottom)
                return lineNumber;
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
    return -1;
}

int CodeEditor::lineNumberAreaWidth()
{
#if OLD
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
#endif
    int digits = 6;

    int space = 13 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::updateInstructions ()
{
    lineNumberArea->update();
}

void CodeEditor::lineNumberToErrorMessage(int lineNumber, char *error_message, int error_message_size)
{
    error_message[0] = 0;
    MainWindow *theMainWindow = (MainWindow*) mainWindow;
    for (int i = 0; i < theMainWindow -> num_of_instructions; i++) {
        assembly_instruction *instruction = theMainWindow -> instructions[i];
        if (lineNumber == instruction->source_line_number && instruction -> error) {
            copy_string_len (error_message, instruction->error_message, error_message_size - 1);
            error_message[error_message_size-1] = 0;
            return;
        }
    }
}

int CodeEditor::lineNumberToAddress(int lineNumber, bool *error)
{
    MainWindow *theMainWindow = (MainWindow*) mainWindow;
    for (int i = 0; i < theMainWindow -> num_of_instructions; i++) {
        assembly_instruction *instruction = theMainWindow -> instructions[i];
        if (lineNumber == instruction->source_line_number) {
            *error = instruction->error != 0;
            return instruction->error == 0 ? instruction->address : 0;
        }
    }
    return 0;
}

int CodeEditor::addressToLineNumber(int address)
{
    MainWindow *theMainWindow = (MainWindow*) mainWindow;
    for (int i = 0; i < theMainWindow -> num_of_instructions; i++) {
        assembly_instruction *instruction = theMainWindow -> instructions[i];
        if (address == (int) instruction->address && instruction->error == 0)
            return instruction->source_line_number;
    }
    return 0;
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    MainWindow *theMainWindow = (MainWindow*) mainWindow;
    int pc = get_pc(theMainWindow -> sim);

    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    int *breakpoints;
    int numBreakpoints = 0;
    get_breakpoints(theMainWindow->sim, &numBreakpoints, &breakpoints);

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            int lineNumber = blockNumber + 1;
            bool error = false;
            int address = lineNumberToAddress(lineNumber, &error);
            QString number = "";
            if (address != 0)
                number = QString::number(address);
            painter.setPen(Qt::black);
            painter.drawText(22, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignLeft, number);

            bool foundBreakpoint = false;
            for (int i = 0; i < numBreakpoints; i++) {
                if (breakpoints[i] == address) {
                    foundBreakpoint = true;
                    break;
                }
            }
            if (foundBreakpoint)
                painter.drawImage(10, top + 2, *breakpointImage);
            if (pc == address && pc != 0)
                painter.drawImage(2, top + 2, *pcImage);
            if (error)
                painter.drawImage(10, top, *errorImage);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::lineNumberAreaRightClickEvent(const QPoint &pos)
{
    clickPos = pos;

    assembly_instruction *instruction = instructionFromLineNumber(lineNumberFromPos(clickPos));
    int enable = instruction != 0 && !instruction->error;

    QMenu contextMenu(tr("LineNumberContextMenu"), this);
    QAction setPCAct("Set PC", this);
    connect(&setPCAct, SIGNAL(triggered()), this, SLOT(setPC()));
    setPCAct.setEnabled(enable);
    contextMenu.addAction(&setPCAct);

    QAction toggleBreakpointAct("Toggle Breakpoint", this);
    connect(&toggleBreakpointAct, SIGNAL(triggered()), this, SLOT(toggleBreakpoint()));
    toggleBreakpointAct.setEnabled(enable);
    contextMenu.addAction(&toggleBreakpointAct);

    QAction explainInstructionAct("Explain Instruction", this);
    connect(&explainInstructionAct, SIGNAL(triggered()), this, SLOT(explainInstruction()));
    explainInstructionAct.setEnabled(enable);
    contextMenu.addAction(&explainInstructionAct);

    contextMenu.exec(mapToGlobal(pos));
}

void CodeEditor::doToggleBreakpoint()
{
    toggleBreakpoint();
}

int CodeEditor::lineNumberFromPos(const QPoint &pos)
{
    int lineNumber = 20;

    int posY = pos.y();
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    while (block.isValid()) {
        if (block.isVisible()) {
            int lineNumber = blockNumber + 1;
            if (posY >= top && posY <= top + (int) blockBoundingRect(block).height())
                return lineNumber;
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }

    return lineNumber;
}

int CodeEditor::addressFromLineNumber(int lineNumber)
{
    MainWindow *theMainWindow = (MainWindow*) mainWindow;
    assembly_instruction **instructions = theMainWindow->instructions;
    for (int i = 0; i < theMainWindow->num_of_instructions; i++)
        if (lineNumber <= instructions[i]->source_line_number)
            return instructions[i]->address;
    return 0;
}

assembly_instruction* CodeEditor::instructionFromLineNumber(int lineNumber)
{
    MainWindow *theMainWindow = (MainWindow*) mainWindow;
    assembly_instruction **instructions = theMainWindow->instructions;
    for (int i = 0; i < theMainWindow->num_of_instructions; i++)
        if (lineNumber <= instructions[i]->source_line_number)
            return instructions[i];
    return 0;
}

void CodeEditor::setPC()
{
    MainWindow *theMainWindow = (MainWindow*) mainWindow;

    int lineNumber = lineNumberFromPos(clickPos);
    int address = addressFromLineNumber(lineNumber);

    if (address != 0)
        set_pc(theMainWindow->sim, address);
    theMainWindow->doUpdateDisplay();
}

void CodeEditor::explainInstruction()
{
    MainWindow *theMainWindow = (MainWindow*) mainWindow;

    int lineNumber = lineNumberFromPos(clickPos);
    assembly_instruction *instruction = instructionFromLineNumber(lineNumber);

    if (instruction != 0) {
        explaindialog *dialog = new explaindialog();
        dialog->Initialize(instruction);
        dialog->exec();
    }
    theMainWindow->doUpdateDisplay();
}

void CodeEditor::toggleBreakpoint()
{
    MainWindow *theMainWindow = (MainWindow*) mainWindow;

    int lineNumber = lineNumberFromPos(clickPos);
    int address = addressFromLineNumber(lineNumber);

    if (address != 0) {
        int *breakpoints;
        int numBreakpoints = 0;
        get_breakpoints(theMainWindow->sim, &numBreakpoints, &breakpoints);
        bool found = false;
        for (int i = 0; i < numBreakpoints; i++) {
            if (breakpoints[i] == address) {
                found = true;
                break;
            }
        }
        if (found)
            delete_breakpoint(theMainWindow->sim, address);
        else
            add_breakpoint(theMainWindow->sim, address);
    }
    theMainWindow->doUpdateDisplay();
}

void LineNumberArea::toggleBreakpoint(const QPoint &pos)
{
    codeEditor->clickPos = pos;
    codeEditor->doToggleBreakpoint();
}

void LineNumberArea::showContextMenu(const QPoint &pos)
{
    codeEditor->lineNumberAreaRightClickEvent(pos);
}

void CodeEditor::onTextChanged()
{
    MainWindow *theMainWindow = (MainWindow*) mainWindow;
    theMainWindow->doBuild();
}

void CodeEditor::ensurePCVisible()
{
    MainWindow *theMainWindow = (MainWindow*) mainWindow;
    int pc = (int) get_pc(theMainWindow->sim);
    int lineNumber = addressToLineNumber(pc);
    QTextCursor cursor(document()->findBlockByLineNumber(lineNumber-1));
    setTextCursor(cursor);
}
