QT       += core gui svg xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 resources_big

DEFINES += QT_DEPRECATED_WARNINGS \
           QT_DISABLE_DEPRECATED_BEFORE=0x060000

include(dependencies/phantom/phantom.pri)
include(dependencies/qtgifimage/src/gifimage/qtgifimage.pri)
include(dependencies/qtcolorwidgets/color_widgets.pri)
include(dependencies/rsdk_reverse/rsdk_reverse.pri)

QMAKE_LFLAGS += -lz
LIBS += -lz

INCLUDEPATH += \
    utils/

SOURCES += \
    includes.cpp \
    mainwindow.cpp \
    tools/compiler/compilerv3.cpp \
    tools/compiler/compilerv4.cpp \
    tools/decompiler/decompilerv3.cpp \
    tools/decompiler/decompilerv4.cpp \
    tools/decompiler/decompilerv1.cpp \
    tools/gameconfigeditorv5.cpp \
    tools/paletteeditor/colourdialog.cpp \
    tools/rsvtool.cpp \
    tools/savefileeditorv3.cpp \
    tools/savefileeditorv4.cpp \
    tools/exportrsdkv5scene.cpp \
    tools/gameconfigeditorv2.cpp \
    tools/gameconfigeditorv3.cpp \
    tools/gameconfigeditorv4.cpp \
    tools/gfxtool.cpp \
    tools/paletteeditor.cpp \
    tools/rsdkunpacker.cpp \
    tools/sceneeditor.cpp \
    tools/sceneeditorv5.cpp \
    tools/sceneproperties/chunkeditor.cpp \
    tools/sceneproperties/sceneexportimgoptions.cpp \
    tools/sceneproperties/scenelayerproperties.cpp \
    tools/sceneproperties/scenelayerpropertiesv5.cpp \
    tools/sceneproperties/sceneobjectproperties.cpp \
    tools/sceneproperties/sceneobjectpropertiesv5.cpp \
    tools/sceneproperties/sceneproperties.cpp \
    tools/sceneproperties/scenepropertiesv5.cpp \
    tools/sceneproperties/scenescrollproperties.cpp \
    tools/sceneproperties/scenescrollpropertiesv5.cpp \
    tools/sceneproperties/scenetileproperties.cpp \
    tools/sceneproperties/scenetilepropertiesv5.cpp \
    tools/sceneproperties/stageconfigeditorv2.cpp \
    tools/sceneproperties/stageconfigeditorv3.cpp \
    tools/sceneproperties/stageconfigeditorv4.cpp \
    tools/sceneproperties/stageconfigeditorv1.cpp \
    tools/sceneproperties/stageconfigeditorv5.cpp \
    tools/sceneviewer.cpp \
    tools/sceneviewerv5.cpp \
    tools/scriptunpacker.cpp \
    tools/staticobjecteditor.cpp \
    utils/appconfig.cpp \
    utils/retroedutils.cpp \
    utils/shaders.cpp \
    main.cpp \
    utils/staticobjectinfo.cpp

HEADERS += \
    mainwindow.hpp \
    tools/compiler/compilerv3.hpp \
    tools/compiler/compilerv4.hpp \
    tools/decompiler/decompilerv3.hpp \
    tools/decompiler/decompilerv4.hpp \
    tools/decompiler/decompilerv1.hpp \
    tools/gameconfigeditorv5.hpp \
    tools/paletteeditor/colourdialog.hpp \
    tools/rsvtool.hpp \
    tools/savefileeditorv3.hpp \
    tools/savefileeditorv4.hpp \
    tools/exportrsdkv5scene.hpp \
    tools/gameconfigeditorv2.hpp \
    tools/gameconfigeditorv3.hpp \
    tools/gameconfigeditorv4.hpp \
    tools/gfxtool.hpp \
    tools/paletteeditor.hpp \
    tools/rsdkunpacker.hpp \
    tools/sceneeditor.hpp \
    tools/sceneeditorv5.hpp \
    tools/sceneproperties/chunkeditor.hpp \
    tools/sceneproperties/sceneexportimgoptions.hpp \
    tools/sceneproperties/scenelayerproperties.hpp \
    tools/sceneproperties/scenelayerpropertiesv5.hpp \
    tools/sceneproperties/sceneobjectproperties.hpp \
    tools/sceneproperties/sceneobjectpropertiesv5.hpp \
    tools/sceneproperties/sceneproperties.hpp \
    tools/sceneproperties/scenepropertiesv5.hpp \
    tools/sceneproperties/scenescrollproperties.hpp \
    tools/sceneproperties/scenescrollpropertiesv5.hpp \
    tools/sceneproperties/scenetileproperties.hpp \
    tools/sceneproperties/scenetilepropertiesv5.hpp \
    tools/sceneproperties/stageconfigeditorv2.hpp \
    tools/sceneproperties/stageconfigeditorv3.hpp \
    tools/sceneproperties/stageconfigeditorv4.hpp \
    tools/sceneproperties/stageconfigeditorv1.hpp \
    tools/sceneproperties/stageconfigeditorv5.hpp \
    tools/sceneviewer.hpp \
    tools/sceneviewerv5.hpp \
    tools/scriptunpacker.hpp \
    tools/staticobjecteditor.hpp \
    utils/appconfig.hpp \
    utils/retroedutils.hpp \
    utils/shaders.hpp \
    includes.hpp \
    utils/staticobjectinfo.hpp

FORMS += \
    mainwindow.ui \
    tools/gameconfigeditorv5.ui \
    tools/paletteeditor/colourdialog.ui \
    tools/rsvtool.ui \
    tools/savefileeditorv3.ui \
    tools/savefileeditorv4.ui \
    tools/exportrsdkv5scene.ui \
    tools/gameconfigeditorv2.ui \
    tools/gameconfigeditorv3.ui \
    tools/gameconfigeditorv4.ui \
    tools/gfxtool.ui \
    tools/paletteeditor.ui \
    tools/rsdkunpacker.ui \
    tools/sceneeditor.ui \
    tools/sceneeditorv5.ui \
    tools/sceneproperties/chunkeditor.ui \
    tools/sceneproperties/sceneexportimgoptions.ui \
    tools/sceneproperties/scenelayerproperties.ui \
    tools/sceneproperties/scenelayerpropertiesv5.ui \
    tools/sceneproperties/sceneobjectproperties.ui \
    tools/sceneproperties/sceneobjectpropertiesv5.ui \
    tools/sceneproperties/sceneproperties.ui \
    tools/sceneproperties/scenepropertiesv5.ui \
    tools/sceneproperties/scenescrollproperties.ui \
    tools/sceneproperties/scenescrollpropertiesv5.ui \
    tools/sceneproperties/scenetileproperties.ui \
    tools/sceneproperties/scenetilepropertiesv5.ui \
    tools/sceneproperties/stageconfigeditorv2.ui \
    tools/sceneproperties/stageconfigeditorv3.ui \
    tools/sceneproperties/stageconfigeditorv4.ui \
    tools/sceneproperties/stageconfigeditorv1.ui \
    tools/sceneproperties/stageconfigeditorv5.ui \
    tools/scriptunpacker.ui \
    tools/staticobjecteditor.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#windows
win32 {
    RC_ICONS = icons/win.ico
    VERSION = 2.0.0
    QMAKE_TARGET_COMPANY = Rubberduckycooly
    QMAKE_TARGET_DESCRIPTION = RetroED
    QMAKE_TARGET_COPYRIGHT = Rubberduckycooly
}

TARGET = RetroED

RESOURCES += \
    resources.qrc

DISTFILES +=
