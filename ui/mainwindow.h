#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "registerwidget.h"
#include "memorywidget.h"

#define FROM_CPP
#include "../engine/sim.h"


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadFile(const QString &fileName);
    void doBuild();
    void doRun();
    void doStep();
    void doAnimate();
    bool eventFilter(QObject *object, QEvent *event) override;
    void doOptions();
    void writeLine(QString str);
    void readLine(char *line);
    void doUpdateDisplay();
    void ensurePCVisible();

private:
    Ui::MainWindow *ui;
    QPlainTextEdit *consoleTextEdit;
    RegisterWidget *registerWidget;
    MemoryWidget *memoryWidget;
    QWidget* registerContainerWidget;
    QWidget* memoryContainerWidget;
    QAction *animateAct;
    bool animating = false;
    int animateMilliseconds = 200;

protected:
    void closeEvent(QCloseEvent *event) override;
    void timerEvent(QTimerEvent *) override;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();

#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

    void updateDisplay();
    void showRegistersDecimal();
    void showRegistersHex();

private:
    void doResetSimulator(); // also prompts to save
    void resetSimulator();
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QString curFile;
    int timerId;

public:
    simulator sim;
    assembly_instruction **instructions;
    int num_of_instructions;
    delta deltas[100];
    int num_deltas = 100;
    int deltas_used = 0;
};

#endif // MAINWINDOW_H
