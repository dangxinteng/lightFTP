TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lpthread -lboost_system -lboost_filesystem

SOURCES += main.cpp \
    FTPServer.cpp \
    Protocal.cpp \
    Socket.cpp

HEADERS += \
    Exception.h \
    FTPServer.h \
    Protocal.h \
    Socket.h \
    Utility.h
