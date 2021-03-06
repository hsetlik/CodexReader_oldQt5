QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    codexcontent.cpp \
    codexdatabase.cpp \
    contentreaderwidget.cpp \
    dictionarywidget.cpp \
    flashcardview.cpp \
    importview.cpp \
    lessonlist.cpp \
    loginform.cpp \
    main.cpp \
    mainwindow.cpp \
    masterstackedwidget.cpp \
    plainflashcardaddition.cpp \
    qtutil.cpp \
    srsviewers.cpp \
    statsview.cpp \
    userdashboard.cpp \
    vocabview.cpp \
    wordlabel.cpp

HEADERS += \
    codexcontent.h \
    codexdatabase.h \
    contentreaderwidget.h \
    dictionarywidget.h \
    flashcardview.h \
    importview.h \
    lessonlist.h \
    loginform.h \
    mainwindow.h \
    masterstackedwidget.h \
    plainflashcardaddition.h \
    qtutil.h \
    srsviewers.h \
    statsview.h \
    userdashboard.h \
    vocabview.h \
    wordlabel.h

FORMS += \
    dictionarywidget.ui \
    flashcardview.ui \
    importview.ui \
    lessonlist.ui \
    loginform.ui \
    mainwindow.ui \
    plainflashcardaddition.ui \
    statsview.ui \
    textreader.ui \
    userdashboard.ui \
    vocabview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
