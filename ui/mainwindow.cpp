
#include <QtWidgets>
#include <QFormLayout>

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

    setUnifiedTitleAndToolBarOnMac(true);

    resetSimulator();
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
    instructions = 0;
    deltas_used = 0;
    ui->codeEditor->initialize(this);
    registerWidget->Initialize(sim, this);
    memoryWidget->Initialize(sim, this);
    doBuild();
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == registerContainerWidget && event->type() == QEvent::Resize)
        registerWidget->containerSizeChanged(registerContainerWidget->width());
    if (object == memoryContainerWidget && event->type() == QEvent::Resize)
        memoryWidget->containerSizeChanged(memoryContainerWidget->width());
    return false;
}

void MainWindow::showRegistersDecimal()
{
    registerWidget->hex = false;
    registerWidget->update();
    ui->toolButtonRegDecimal->setChecked(true);
    ui->toolButtonRegHex->setChecked(false);
}

void MainWindow::showRegistersHex()
{
    registerWidget->hex = true;
    registerWidget->update();
    ui->toolButtonRegDecimal->setChecked(false);
    ui->toolButtonRegHex->setChecked(true);
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
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About RISC-V Simulator"),
            tr("RISC-V Simulator v 1.1<br/>&nbsp;<br/>"
               "The <b>RISC-V Simulator</b> assembles RISC-V assembly language programs and "
               "runs the programs on a simulated RISC-V processor.<br/>&nbsp;<br/>Copyright (c) 2018 Seminole State College. All rights reserved"));
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
    instructions = assemble(sim, str, 1000, &num_of_instructions);
    updateDisplay();
}

void MainWindow::doRun()
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
    if (animating)
        doStep();
    else
        killTimer(timerId);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
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
