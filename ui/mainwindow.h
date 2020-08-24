#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "registerwidget.h"
#include "memorywidget.h"
#include "disassemblerwidget.h"

#define FROM_CPP
#include "../engine/sim.h"
#include "../engine/sim2.h"


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
class QSessionManager;
class QLabel;
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
    void doBuildSim2();
    int doPartialBuildSim2(int lineNumber, char *line);
    int enterKeyHit(int lineNumber);
    void doRun();
    void startRun();
    void doStep();
    void doAnimate();
    bool eventFilter(QObject *object, QEvent *event) override;
    void doOptions();
    void doDecreaseFontSize();
    void doIncreaseFontSize();
    void setFont();
    void writeLine(QString str);
    void readLine(char *line);
    void doUpdateDisplay();
    void ensurePCVisible();

private:
    Ui::MainWindow *ui;
    QPlainTextEdit *consoleTextEdit;
    RegisterWidget *registerWidget;
    MemoryWidget *memoryWidget;
    DisassemblerWidget *disassemblerWidget;
    DisassemblerWidget *disassemblerWidget2;
    QWidget* registerContainerWidget;
    QWidget* memoryContainerWidget;
    QWidget* disassemblerContainerWidget;
    QWidget* disassemblerContainerWidget2;
    QAction *animateAct;
    QLabel *statusText;
    QLabel *time1Text;
    QLabel *time2Text;
    QLabel *timeRatioText;
    bool animating = false;
    int animateMilliseconds = 200;
    int fontSize = 12;

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
    void showTimes();

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
    void resetTimers();
    void setStatusBarText(const char text[]);
    void compareDisassemblies();

    QString curFile;
    int timerId;

    QTimer *timer;

    long long time_sim;   // milliseconds
    long long time_sim2;  // milliseconds

public:
    simulator sim;
    assembly_instruction **instructions;
    int num_of_instructions;
    simulator2 *sim2;
    assembly_instruction **instructions_sim2;
    int num_of_instructions_sim2;
    delta deltas[100];
    int num_deltas = 100;
    int deltas_used = 0;
};

#endif // MAINWINDOW_H
