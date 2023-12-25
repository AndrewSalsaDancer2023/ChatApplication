#include "mainwindow.h"
#include <iostream>
#include <QApplication>
#include "coordinator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    coordinator coordinator{w};
    w.setCoordinator(&coordinator);
    w.show();
    return a.exec();
}

/*
conanbuildinfo.cmake qt cmake
https://doc.qt.io/qtcreator/creator-build-settings-cmake.html#using-cmake-with-conan
https://doc.qt.io/qtcreator/creator-build-settings-conan.html#conan-build-steps
*/
