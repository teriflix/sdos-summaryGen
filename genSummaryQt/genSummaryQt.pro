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
    fountain.h \
    ollamaclient.h \
    scenesummarizer.h

SOURCES += \
    fountain.cpp \
    main.cpp \
    ollamaclient.cpp \
    scenesummarizer.cpp

RESOURCES += \
    prompts.qrc
