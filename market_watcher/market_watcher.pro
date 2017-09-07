QT += core dbus xml
QT -= gui

TARGET = market_watcher
CONFIG += console c++14
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../common/market.cpp \
    ../common/common_utility.cpp \
    ../common/multiple_timer.cpp \
    ../common/trading_calendar.cpp \
    market_watcher.cpp \
    tick_receiver.cpp

HEADERS += ../config.h \
    ../common/market.h \
    ../common/common_utility.h \
    ../common/multiple_timer.h \
    ../common/trading_calendar.h \
    market_watcher.h \
    tick_receiver.h

INCLUDEPATH += ../ ../common/
DBUS_ADAPTORS += ../interface/market_watcher.xml

unix:CTP_FOLDER_PREFIX = linux
win32:CTP_FOLDER_PREFIX = win

contains(QT_ARCH, i386) {
    CTP_FOLDER_SUFFIX = 32
} else {
    CTP_FOLDER_SUFFIX = 64
}

HEADERS += \
    ../ctp/$$CTP_FOLDER_PREFIX$$CTP_FOLDER_SUFFIX/ThostFtdcMdApi.h \
    ../ctp/$$CTP_FOLDER_PREFIX$$CTP_FOLDER_SUFFIX/ThostFtdcUserApiDataType.h \
    ../ctp/$$CTP_FOLDER_PREFIX$$CTP_FOLDER_SUFFIX/ThostFtdcUserApiStruct.h

INCLUDEPATH += ../ctp/$$CTP_FOLDER_PREFIX$$CTP_FOLDER_SUFFIX/

unix:LIBS += $$PWD/../ctp/$$CTP_FOLDER_PREFIX$$CTP_FOLDER_SUFFIX/thostmduserapi.so
win32:LIBS += $$PWD/../ctp/$$CTP_FOLDER_PREFIX$$CTP_FOLDER_SUFFIX/thostmduserapi.lib

# Copies the given files to the destination directory
defineTest(copyToDestdir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$DESTDIR

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

win32 {
    copyToDestdir($$PWD/../ctp/$$CTP_FOLDER_PREFIX$$CTP_FOLDER_SUFFIX/thostmduserapi.dll)
    copyToDestdir($$PWD/../ctp/$$CTP_FOLDER_PREFIX$$CTP_FOLDER_SUFFIX/thosttraderapi.dll)
}
