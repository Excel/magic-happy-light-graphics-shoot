QT += core gui opengl

TARGET = final
TEMPLATE = app

INCLUDEPATH += lab \
    lib \
    math \
    support \
    models
DEPENDPATH += lab \
    lib \
    math \
    support\
    models

HEADERS += lab/glwidget.h \
    lib/targa.h \
    lib/glm.h \
    math/vector.h \
    support/resourceloader.h \
    support/mainwindow.h \
    support/camera.h \
    game/World.h \
    game/Bullet.h \
    game/particleemitter.h \
    support/common.h \
    game/Entity.h \
    game/Target.h \
    game/PBCurve.h \
    game/canvas.h

SOURCES += \
    lab/glwidget.cpp \
    lib/targa.cpp \
    lib/glm.cpp \
    support/resourceloader.cpp \
    support/mainwindow.cpp \
    support/main.cpp \
    support/camera.cpp \
    game/World.cpp \
    game/Bullet.cpp \
    game/particleemitter.cpp \
    game/Entity.cpp \
    game/Target.cpp \
    game/PBCurve.cpp \
    game/canvas.cpp

FORMS += mainwindow.ui \
    support/mainwindow.ui

OTHER_FILES += \
    shaders/refract.vert \
    shaders/refract.frag \
    shaders/reflect.vert \
    shaders/reflect.frag \
    shaders/brightpass.frag \
    shaders/blur.frag \
    shaders/psycho.frag \
    shaders/psycho.vert \
    shaders/psycho.vars

RESOURCES += \
    resources.qrc
