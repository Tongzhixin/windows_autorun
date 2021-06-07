QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

LIBS += \
        -ladvapi32 \
        -lkernel32 \
        -luser32 \
        -lgdi32 \
        -lcrypt32 \
        -lwintrust \
        -ltaskschd \
        -lcomsupp \
        -lshlwapi
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_DEPRECATED_WARNINGS
SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    convert.h \
    description.h \
    mainwindow.h \
    regread.h \
    signature.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    autorun_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
