QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Render/render_gl_realtime_raster_renderer.cpp \
    Render/render_gl_shadermanager.cpp \
    Render/render_mesh.cpp \
    Render/render_model.cpp \
    main.cpp \
    mainwindow.cpp \
    Render/render_camera.cpp \
    Render/render_renderimage2d.cpp \
    Render/render_rendertexture.cpp \
    Render/Light/render_light.cpp \
    Render/Light/render_parallellight.cpp \
    Global/globalfunc.cpp \
    Global/globalgl.cpp \
    Global/globalinfo.cpp \
    Global/globalrender.cpp \
    Global/globalui.cpp


HEADERS += \
    Render/render_gl_realtime_raster_renderer.h \
    Render/render_gl_shadermanager.h \
    Render/render_mesh.h \
    Render/render_model.h \
    mainwindow.h \
    Render/render_camera.h \
    Render/render_renderimage2d.h \
    Render/render_rendertexture.h \
    Render/Light/render_light.h \
    Render/Light/render_parallellight.h \
    Global/globalfunc.h \
    Global/globalgl.h \
    Global/globalinfo.h \
    Global/globalrender.h \
    Global/globalui.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/./ -llibassimp.dll

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

RESOURCES += \
    Shader/RealTimeRasterizeRenderPipline/RealTimeRasterizeRenderShaders.qrc
