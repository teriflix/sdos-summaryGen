QT += gui network

TARGET = genSummaryQt
DESTDIR = $$PWD

macx {
    QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
    QMAKE_LIBS_OPENGL = -framework OpenGL
    LIBS -= -framework AGL
    LIBS += -framework Carbon -framework OpenGL
    CONFIG += sdk_no_version_check
    CONFIG -= app_bundle
}

HEADERS += \
    abstracttask.h \
    charactersummarizer.h \
    fountain.h \
    ollamaclient.h \
    scenesummarizer.h \
    scenesummarizer2.h \
    storysummarizer.h \
    taskqueue.h

SOURCES += \
    abstracttask.cpp \
    charactersummarizer.cpp \
    fountain.cpp \
    main.cpp \
    ollamaclient.cpp \
    scenesummarizer.cpp \
    scenesummarizer2.cpp \
    storysummarizer.cpp \
    taskqueue.cpp

RESOURCES += \
    prompts.qrc
