QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    behavioranalyzer.cpp \
    databasemanager.cpp \
    datamodels.cpp \
    main.cpp \
    mainwindow.cpp \
    metricscalculator.cpp \
    playerprofilepage.cpp \
    recommendersystem.cpp \
    replayanalyzerpage.cpp \
    replayparser.cpp \
    toptoolbar.cpp

HEADERS += \
    behavioranalyzer.h \
    databasemanager.h \
    datamodels.h \
    mainwindow.h \
    metricscalculator.h \
    playerprofilepage.h \
    recommendersystem.h \
    replayanalyzerpage.h \
    replayparser.h \
    toptoolbar.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
