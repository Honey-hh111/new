QT += core gui widgets multimedia multimediawidgets
CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    sokoban.cpp \
    solitaire.cpp \
    sudoku.cpp

HEADERS += \
    mainwindow.h \
    sokoban.h \
    solitaire.h \
    sudoku.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
DEFINES +=QT_NO_DEBUG_OUTPUT