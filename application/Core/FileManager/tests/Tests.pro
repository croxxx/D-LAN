# -------------------------------------------------
# Project created by QtCreator 2009-10-04T02:24:09
# -------------------------------------------------
QT += testlib
QT -= gui
TARGET = Tests
CONFIG += link_prl console
CONFIG -= app_bundle

DESTDIR = "output/debug"
MOC_DIR = ".tmp/debug"
OBJECTS_DIR = ".tmp/debug"

LIBS += -L../output/debug -lFileManager
POST_TARGETDEPS += ../output/debug/libFileManager.a

LIBS += -L../../../Common/output/debug -lCommon
POST_TARGETDEPS += ../../../Common/output/debug/libCommon.a

# FIXME : Should not be here, all dependies are read from the prl file (see link_prl):
LIBS += -L../../../Common/LogManager/output/debug -lLogManager
POST_TARGETDEPS += ../../../Common/LogManager/output/debug/libLogManager.a

LIBS += -L${PROTOBUF}/src/.libs \
    -lprotobuf

INCLUDEPATH += . \
    .. \
    ../../.. \ # For the 'Common' component.
    ${PROTOBUF}/src

TEMPLATE = app
SOURCES += main.cpp \
    Tests.cpp \
    ../../../Protos/common.pb.cc
HEADERS += Tests.h \
    ../../../Protos/common.pb.h
