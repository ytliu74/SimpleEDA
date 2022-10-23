TEMPLATE = app
TARGET = simpleEDA
INCLUDEPATH += $$PWD/src

QT += widgets printsupport

QMAKE_CC = clang
QMAKE_CXX = clang++
QMAKE_CXXFLAGS += -std=gnu++17

QMAKE_LINK = clang++

CONFIG += debug

MOC_DIR     = build
OBJECTS_DIR = build
RCC_DIR     = build
UI_DIR      = build

include($$PWD/src/main.pri)