
#include <QtWidgets>
#include <QFormLayout>
#include <chrono>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "codeeditor.h"
#include "optionsdialog.h"
#include "readlinedialog.h"

#ifndef FROM_CPP
#define FROM_CPP 1
#endif
#include "../engine/sim.h"


static MainWindow *singleton = 0;
static char line[1024];

void output_string (const char *str)
{
    singleton->writeLine(QString(str));
}

const char * input_string ()
{
    singleton->readLine(line);
    return line;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    singleton = this;

    ui->setupUi(this);

    createActions();
    createStatusBar();
    readSettings();
    connect(ui->codeEditor->document(), &QTextDocument::contentsChanged, this, &MainWindow::documentWasModified);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest, this, &MainWindow::commitData);
#endif

    // set up console layout
    QWidget* multiWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    consoleTextEdit = new QPlainTextEdit();
    layout->addWidget(consoleTextEdit);
    multiWidget->setLayout(layout);
    ui->dockWidgetConsole->setWidget(multiWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    // set point size in console pane
    QFont font = consoleTextEdit->document()->defaultFont();
    font.setPointSize(11);
    consoleTextEdit->document()->setDefaultFont(font);

    // set up register widget
    QWidget *containerWidget = new QWidget();
    containerWidget->setLayout(new QVBoxLayout);

    registerWidget = new RegisterWidget();
    registerWidget->setLayout(new QVBoxLayout);
    registerWidget->layout()->setAlignment(Qt::AlignTop);

    QScrollArea *registerScrollArea = new QScrollArea();
    registerScrollArea->setWidget(registerWidget);
    registerScrollArea->setFrameShape(QFrame::NoFrame);

    containerWidget->layout()->addWidget(registerScrollArea);

    // set up registers layout
    registerContainerWidget = new QWidget();
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->addItem(new QSpacerItem(0, 10), 0, 1, 1, 2);
    gridLayout->addWidget(ui->toolButtonRegDecimal, 1, 1, Qt::AlignRight);
    gridLayout->addWidget(ui->toolButtonRegHex, 1, 2, Qt::AlignLeft);
    gridLayout->addWidget(containerWidget, 2, 1, 1, 2);
    registerContainerWidget->setLayout(gridLayout);
    ui->dockWidgetRegisters->setWidget(registerContainerWidget);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    //registerContainerWidget->setStyleSheet("border: 1px solid red");
    registerContainerWidget->installEventFilter(this);

    // set up memory widget
    containerWidget = new QWidget();
    containerWidget->setLayout(new QVBoxLayout);

    memoryWidget = new MemoryWidget();
    memoryWidget->setLayout(new QVBoxLayout);
    memoryWidget->layout()->setAlignment(Qt::AlignTop);

    QScrollArea *memoryScrollArea = new QScrollArea();
    memoryScrollArea->setWidget(memoryWidget);
    memoryScrollArea->setFrameShape(QFrame::NoFrame);

    containerWidget->layout()->addWidget(memoryScrollArea);

    // set up memory layout
    memoryContainerWidget = new QWidget();
    gridLayout = new QGridLayout();
    gridLayout->addWidget(containerWidget, 2, 1, 1, 2);
    //    gridLayout->setColumnStretch(1, 0);
    memoryContainerWidget->setLayout(gridLayout);
    ui->dockWidgetMemory->setWidget(memoryContainerWidget);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    //memoryContainerWidget->setStyleSheet("border: 1px solid red");
    memoryContainerWidget->installEventFilter(this);

    connect(ui->toolButtonRegDecimal, &QToolButton::clicked, this, &MainWindow::showRegistersDecimal);
    connect(ui->toolButtonRegHex, &QToolButton::clicked, this, &MainWindow::showRegistersHex);

    // set up disassembler widget
    containerWidget = new QWidget();
    containerWidget->setLayout(new QVBoxLayout);

    disassemblerWidget = new DisassemblerWidget();
    disassemblerWidget->setLayout(new QVBoxLayout);
    disassemblerWidget->layout()->setAlignment(Qt::AlignTop);

    QScrollArea *disassemblerScrollArea = new QScrollArea();
    disassemblerScrollArea->setWidget(disassemblerWidget);
    disassemblerScrollArea->setFrameShape(QFrame::NoFrame);

    containerWidget->layout()->addWidget(disassemblerScrollArea);

    // set up disassembler layout
    disassemblerContainerWidget = new QWidget();
    gridLayout = new QGridLayout();
    gridLayout->addWidget(containerWidget, 2, 1, 1, 2);
    //    gridLayout->setColumnStretch(1, 0);
    disassemblerContainerWidget->setLayout(gridLayout);
    ui->dockWidgetDisassembler->setWidget(disassemblerContainerWidget);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    //disassemblerContainerWidget->setStyleSheet("border: 1px solid red");
    disassemblerContainerWidget->installEventFilter(this);

    // set up disassembler widget 2
    containerWidget = new QWidget();
    containerWidget->setLayout(new QVBoxLayout);

    disassemblerWidget2 = new DisassemblerWidget();
    disassemblerWidget2->setLayout(new QVBoxLayout);
    disassemblerWidget2->layout()->setAlignment(Qt::AlignTop);

    QScrollArea *disassemblerScrollArea2 = new QScrollArea();
    disassemblerScrollArea2->setWidget(disassemblerWidget2);
    disassemblerScrollArea2->setFrameShape(QFrame::NoFrame);

    containerWidget->layout()->addWidget(disassemblerScrollArea2);

    // set up disassembler layout
    disassemblerContainerWidget2 = new QWidget();
    gridLayout = new QGridLayout();
    gridLayout->addWidget(containerWidget, 2, 1, 1, 2);
    //    gridLayout->setColumnStretch(1, 0);
    disassemblerContainerWidget2->setLayout(gridLayout);
    ui->dockWidgetDisassembler2->setWidget(disassemblerContainerWidget2);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    //disassemblerContainerWidget->setStyleSheet("border: 1px solid red");
    disassemblerContainerWidget2->installEventFilter(this);

    setUnifiedTitleAndToolBarOnMac(true);

    setFont();

    resetSimulator();

    resetTimers();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTimes()));
    timer -> start(1000);
}

void MainWindow::resetTimers()
{
    time_sim = time_sim2 = 0;
    showTimes();
}

void MainWindow::doResetSimulator()
{
    if (maybeSave())
        resetSimulator();
}

void MainWindow::resetSimulator()
{
    setCurrentFile(QString());
    sim = create_simulator(output_string, input_string);
    sim2 = create_simulator2(output_string, input_string);
    instructions = 0;
    deltas_used = 0;
    ui->codeEditor->initialize(this);
    registerWidget->Initialize(sim, this);
    memoryWidget->Initialize(sim, this);
    disassemblerWidget->Initialize(sim, this);

    disassemblerWidget2->Initialize(sim2, this);
    instructions_sim2 = 0;

    doBuild();
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == registerContainerWidget && event->type() == QEvent::Resize)
        registerWidget->containerSizeChanged(registerContainerWidget->width());
    if (object == memoryContainerWidget && event->type() == QEvent::Resize)
        memoryWidget->containerSizeChanged(memoryContainerWidget->width());
    if (object == disassemblerContainerWidget && event->type() == QEvent::Resize)
        disassemblerWidget->containerSizeChanged(disassemblerContainerWidget->width());
    if (object == disassemblerContainerWidget2 && event->type() == QEvent::Resize)
        disassemblerWidget2->containerSizeChanged(disassemblerContainerWidget2->width());
    return false;
}

void MainWindow::showRegistersDecimal()
{
    registerWidget->hex = false;
    ui->toolButtonRegDecimal->setChecked(true);
    ui->toolButtonRegHex->setChecked(false);
    registerWidget->update();
    registerWidget->repaint();
}

void MainWindow::showRegistersHex()
{
    registerWidget->hex = true;
    ui->toolButtonRegDecimal->setChecked(false);
    ui->toolButtonRegHex->setChecked(true);
    registerWidget->update();
    registerWidget->repaint();
}

MainWindow::~MainWindow()
{
    if (animating)
        killTimer(timerId);

    if (instructions != 0) {
        deallocate_assemble_results(instructions, num_of_instructions);
        instructions = 0;
    }
    delete_simulator(sim);

    if (instructions_sim2 != 0) {
        deallocate_assemble_results(instructions_sim2, num_of_instructions_sim2);
        instructions_sim2 = 0;
    }
    delete_simulator2(sim2);

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        resetSimulator();
        ui->codeEditor->clear();
        setCurrentFile(QString());
        doBuild();
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty()) {
            resetSimulator();
            loadFile(fileName);
        }
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    const QString &fileName = dialog.getSaveFileName(this, "Save As");
    if (fileName.length() == 0)
        return false;
    return saveFile(fileName);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About RISC-V Simulator"),
            tr("RISC-V Simulator v 1.1<br/>&nbsp;<br/>"
               "The <b>RISC-V Simulator</b> assembles RISC-V assembly language programs and "
               "runs the programs on a simulated RISC-V processor.<br/>&nbsp;<br/>Copyright (c) 2018-20 "
               "Seminole State College. All rights reserved"));
}

void MainWindow::documentWasModified()
{
    setWindowModified(ui->codeEditor->document()->isModified());
}

void MainWindow::updateDisplay()
{
    ui->codeEditor->updateInstructions();
    registerWidget->update();
    memoryWidget->update();
    disassemblerWidget->update();
    disassemblerWidget2->update();
    compareDisassemblies();
}

void MainWindow::doBuild()
{
    if (instructions != 0) {
        deallocate_assemble_results(instructions, num_of_instructions);
        instructions = 0;
    }

    QString programStr = ui->codeEditor->toPlainText();
    QByteArray byteArray = programStr.toLocal8Bit();
    char *str = byteArray.data();

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    instructions = assemble(sim, str, 1000, &num_of_instructions);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    time_sim += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();

    disassemblerWidget -> refreshDisassembly();
    updateDisplay();
}

void MainWindow::doBuildSim2()
{
    if (instructions_sim2 != 0) {
        deallocate_assemble_results(instructions_sim2, num_of_instructions_sim2);
        instructions_sim2 = 0;
    }

    QString programStr = ui->codeEditor->toPlainText();
    QByteArray byteArray = programStr.toLocal8Bit();
    char *str = byteArray.data();

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    instructions_sim2 = assemble2(sim2, str, 1000, &num_of_instructions_sim2);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    time_sim2 += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();

    disassemblerWidget2 -> refreshDisassembly();
    updateDisplay();
}

// returns 0 for success, 1 otherwise
int MainWindow::doPartialBuildSim2(int lineNumber, const char *line) {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int result = do_partial_assembly (sim2, lineNumber, line);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    time_sim2 += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();

    disassemblerWidget2 -> refreshDisassembly();
    compareDisassemblies();

    return result;
}

// returns 0 for success, 1 otherwise
int MainWindow::enterKeyHit(int lineNumber) {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    int result = enter_key_hit (sim2, lineNumber, &instructions_sim2, &num_of_instructions_sim2);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    time_sim2 += std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();

    disassemblerWidget2 -> refreshDisassembly();
    compareDisassemblies();

    return result;
}

void MainWindow::compareDisassemblies() {
    disassemblerWidget->compareTo(disassemblerWidget2);
    disassemblerWidget2->compareTo(disassemblerWidget);
}

void MainWindow::doRun()
{
    reset_stop(sim);
    startRun();
}

void MainWindow::startRun()
{
    char *error_message = 0;
    deltas_used = 0;
    do_run(sim, &error_message, deltas, num_deltas, &deltas_used);
    if (error_message != 0)
        consoleTextEdit -> appendPlainText(QString(error_message));
    updateDisplay();
    ensurePCVisible();
}

void MainWindow::doStep()
{
    char *error_message = 0;
    deltas_used = 0;
    do_step(sim, &error_message, deltas, num_deltas, &deltas_used);
    if (error_message != 0)
        consoleTextEdit -> appendPlainText(QString(error_message));
    updateDisplay();
    ensurePCVisible ();

    // if 'stop' executed, kill the timer
    if (is_stopped(sim))
        animating = false;
}

void MainWindow::ensurePCVisible()
{
    ui->codeEditor->ensurePCVisible();
}

void MainWindow::doOptions()
{
    OptionsDialog *dlg = new OptionsDialog();
    dlg->Initialize(memoryWidget->address);
    int dialogCode = dlg->exec();
    if (dialogCode == QDialog::Accepted)
        memoryWidget->address = dlg->GetStartAddress();
    updateDisplay();
}

void MainWindow::doDecreaseFontSize()
{
    fontSize --;
    if (fontSize < 8)
        fontSize = 8;
    setFont();
}

void MainWindow::doIncreaseFontSize()
{
    fontSize ++;
    if (fontSize > 48)
        fontSize = 48;
    setFont();
}

void MainWindow::setFont()
{
    QFont newFont("Courier", fontSize);
    ui->codeEditor->setFont(newFont);
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    QAction *resetAct = fileMenu->addAction(tr("Reset Simulator"), this, &MainWindow::doResetSimulator);
    resetAct->setStatusTip(tr("Reset the simulator"));

    fileMenu->addSeparator();

    QAction *resetTimersAct = fileMenu->addAction(tr("Reset Timers"), this, &MainWindow::resetTimers);
    resetTimersAct->setStatusTip(tr("Reset the timers"));

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));
#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, ui->codeEditor, &QPlainTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, ui->codeEditor, &QPlainTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, ui->codeEditor, &QPlainTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();

#endif // !QT_NO_CLIPBOARD

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(ui->codeEditor, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(ui->codeEditor, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(ui->dockWidgetRegisters->toggleViewAction());
    viewMenu->addAction(ui->dockWidgetMemory->toggleViewAction());
    viewMenu->addAction(ui->dockWidgetConsole->toggleViewAction());
    viewMenu->addAction(ui->dockWidgetDisassembler->toggleViewAction());
    viewMenu->addAction(ui->dockWidgetDisassembler2->toggleViewAction());
    viewMenu->addSeparator();

    QAction *decreaseFontSizeAction = new QAction(tr("Decrease Font Size"), this);
    connect(decreaseFontSizeAction, &QAction::triggered, this, &MainWindow::doDecreaseFontSize);
    viewMenu->addAction(decreaseFontSizeAction);

    QAction *increaseFontSizeAction = new QAction(tr("Increase Font Size"), this);
    connect(increaseFontSizeAction, &QAction::triggered, this, &MainWindow::doIncreaseFontSize);
    viewMenu->addAction(increaseFontSizeAction);

    viewMenu->addSeparator();
    QAction *optionsAct = new QAction(tr("&Options"), this);
    connect(optionsAct, &QAction::triggered, this, &MainWindow::doOptions);
    viewMenu->addAction(optionsAct);

    QToolBar *runToolBar = addToolBar(tr("Run"));
    QMenu *runMenu = menuBar()->addMenu(tr("&Run"));

    const QIcon *runIcon = new QIcon(":/images/run.png");
    QAction *runAct = new QAction(*runIcon, tr("&Run"), this);
    runAct->setShortcut(Qt::CTRL + Qt::Key_R);
    runAct->setStatusTip(tr("Run"));
    connect(runAct, &QAction::triggered, this, &MainWindow::doRun);
    runMenu->addAction(runAct);
    runToolBar->addAction(runAct);

    const QIcon *stepIcon = new QIcon(":/images/step.png");
    QAction *stepAct = new QAction(*stepIcon, tr("&Step"), this);
    stepAct->setShortcut(Qt::CTRL + Qt::Key_T);
    stepAct->setStatusTip(tr("Step"));
    connect(stepAct, &QAction::triggered, this, &MainWindow::doStep);
    runMenu->addAction(stepAct);
    runToolBar->addAction(stepAct);

    const QIcon *animateIcon = new QIcon(":/images/animate.png");
    animateAct = new QAction(*animateIcon, tr("Start &Animation"), this);
    animateAct->setShortcut(Qt::CTRL + Qt::Key_M);
    animateAct->setStatusTip(tr("Animate"));
    connect(animateAct, &QAction::triggered, this, &MainWindow::doAnimate);
    runMenu->addAction(animateAct);
    runToolBar->addAction(animateAct);
    animateAct->setText(QString("Start/Stop &Animation"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show the application's About box"));
}

void MainWindow::doAnimate()
{
    reset_stop(sim);
    if (animating) {
        animating = false;
    }
    else {
        animating = true;
        timerId = startTimer(animateMilliseconds);
    }
}

void MainWindow::timerEvent(QTimerEvent *)
{
    if (!animating) {
        killTimer(timerId);
        return;
    }

    doStep();
}

void MainWindow::createStatusBar()
{
    statusText = new QLabel();
    time1Text = new QLabel();
    time2Text = new QLabel();
    timeRatioText = new QLabel();

    statusBar() -> addWidget(statusText);
    statusBar() -> addPermanentWidget(time1Text);
    statusBar() -> addPermanentWidget(time2Text);
    statusBar() -> addPermanentWidget(timeRatioText);

    setStatusBarText("Ready");
}

void MainWindow::setStatusBarText(const char text[]) {
    statusText->setText(QString(text));
}

void MainWindow::showTimes() {

    char time1[100];
    char time2[100];
    char ratio[100];

    sprintf(time1, "%.2f ms", (double) time_sim / 1000);
    sprintf(time2, "%.2f ms", (double) time_sim2 / 1000);
    sprintf(ratio, "Ratio: %.2f", (double) time_sim / time_sim2);

    time1Text->setText(QString(time1));
    time2Text->setText(QString(time2));
    timeRatioText->setText(QString(ratio));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool MainWindow::maybeSave()
{
    if (!ui->codeEditor->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    ui->codeEditor->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);

    doBuild();

    resetTimers();
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << ui->codeEditor->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    ui->codeEditor->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.asm";
    setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (ui->codeEditor->document()->isModified())
            save();
    }
}
#endif

void MainWindow::writeLine(QString line)
{
    consoleTextEdit -> appendPlainText(line);
}

void MainWindow::doUpdateDisplay()
{
    updateDisplay();
}

void MainWindow::readLine(char *line)
{
    line[0] = 0;
    ReadLineDialog *dlg = new ReadLineDialog();
    dlg->exec();
    QString userLine = dlg->GetUserInputLine();
    QByteArray ba = userLine.toLatin1();
    strcpy(line, ba.data());
}
