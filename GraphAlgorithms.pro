QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = GraphAlgorithms
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/graph.cpp \
    src/graphalgorithms.cpp \
    src/graphvisualizer.cpp \
    src/grapheditor.cpp

HEADERS += \
    src/mainwindow.h \
    src/graph.h \
    src/graphalgorithms.h \
    src/graphvisualizer.h \
    src/grapheditor.h

FORMS += \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc
