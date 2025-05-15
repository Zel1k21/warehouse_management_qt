QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    employeeMenu.cpp \
    main.cpp \
    mainwindow.cpp \
    productUtils.cpp \
    productWindowUtils.cpp \
    receivingWindow.cpp \
    sendingWindow.cpp \
    usermenu.cpp \
    warehouseDialog.cpp \
    warehouseOwnerMenu.cpp

HEADERS += \
    accountType.h \
    employeeMenu.h \
    mainwindow.h \
    modules.h \
    productUtils.h \
    productWindowUtils.h \
    sendingWindow.h \
    receivingWindow.h \
    usermenu.h \
    warehouseDialog.h \
    warehouseOwnerMenu.h

FORMS += \
    employeeMenu.ui \
    mainwindow.ui \
    sendingWindow.ui \
    receivingWindow.ui \
    usermenu.ui \
    warehouseOwnerMenu.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
