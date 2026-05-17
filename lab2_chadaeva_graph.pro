QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    graph.cpp \
    graphvisuals.cpp

HEADERS += \
    mainwindow.h \
    graph.h \
    graphvisuals.h