TEMPLATE = app
TARGET = simpleEDA
INCLUDEPATH += $$PWD/src

QT += widgets printsupport

CONFIG += debug

MOC_DIR     = build
OBJECTS_DIR = build
RCC_DIR     = build
UI_DIR      = build

include($$PWD/src/main.pri)