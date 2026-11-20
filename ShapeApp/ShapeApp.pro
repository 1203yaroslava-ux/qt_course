QT += core gui widgets sql
CONFIG += c++11
TARGET = ShapeApp
TEMPLATE = app

INCLUDEPATH += $$PWD

SOURCES += main.cpp \
    db/database.cpp \
    scene/shapeitem.cpp \
    scene/linkitem.cpp \
    ui/mainwindow.cpp \
    delegates/shapedelegate.cpp

HEADERS += \
    db/database.h \
    scene/shapeitem.h \
    scene/linkitem.h \
    ui/mainwindow.h \
    delegates/shapedelegate.h
