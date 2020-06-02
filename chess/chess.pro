QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    board.cpp \
    button.cpp \
    game.cpp \
    main.cpp \
    piece.cpp \
    tile.cpp

HEADERS += \
    board.h \
    button.h \
    game.h \
    piece.h \
    tile.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    images/black_bishop.png \
    images/black_king.png \
    images/black_knight.png \
    images/black_pawn.png \
    images/black_queen.png \
    images/black_rook.png \
    images/white_bishop.png \
    images/white_king.png \
    images/white_knight.png \
    images/white_pawn.png \
    images/white_queen.png \
    images/white_rook.png

RESOURCES += \
    res.qrc