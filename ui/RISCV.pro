#-------------------------------------------------
#
# Project created by QtCreator 2017-12-16T10:14:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RISCV
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    codeeditor.cpp \
    main.cpp \
    mainwindow.cpp \
    registerwidget.cpp \
    memorywidget.cpp \
    optionsdialog.cpp \
    readlinedialog.cpp \
    ../engine/instructions.c \
    ../engine/sim.c \
    ../engine/utils.c \
    explaindialog.cpp \
    explainintdialog.cpp \
    explainfloatdialog.cpp \
    changeregisterdialog.cpp

HEADERS += \
        mainwindow.h \
    codeeditor.h \
    registerwidget.h \
    memorywidget.h \
    optionsdialog.h \
    readlinedialog.h \
    ../engine/instructions.h \
    ../engine/internal.h \
    ../engine/sim.h \
    ../engine/utils.h \
    explaindialog.h \
    explainintdialog.h \
    explainfloatdialog.h \
    changeregisterdialog.h

FORMS += \
        mainwindow.ui \
    optionsdialog.ui \
    readlinedialog.ui \
    explaindialog.ui \
    explainintdialog.ui \
    explainfloatdialog.ui \
    changeregisterdialog.ui

RESOURCES += \
    application.qrc

DISTFILES += \
    engine/status.txt \
    engine/tests/arithmetic_and_logical.asm \
    engine/tests/branch.asm \
    engine/tests/compare.asm \
    engine/tests/data_input_output.asm \
    engine/tests/float.asm \
    engine/tests/float_compare.asm \
    engine/tests/jal.asm \
    engine/tests/load_and_store.asm \
    engine/tests/mul_div.asm \
    engine/tests/pseudoinstructions.asm \
    engine/tests/shift.asm \
    engine/tests/arithmetic_and_logical.asm \
    engine/tests/branch.asm \
    engine/tests/compare.asm \
    engine/tests/data_input_output.asm \
    engine/tests/float.asm \
    engine/tests/float_compare.asm \
    engine/tests/jal.asm \
    engine/tests/load_and_store.asm \
    engine/tests/mul_div.asm \
    engine/tests/pseudoinstructions.asm \
    engine/tests/shift.asm \
    engine/tests/demo.asm

CONFIG     += debug

CONFIG += static
# QMAKE_LFLAGS += -static-libgcc -static-libstdc++
QMAKE_LFLAGS += -static-libstdc++

# marwan 12-31-18 - had to add this, for it to find the latest sdk
QMAKE_MAC_SDK = macosx10.14
