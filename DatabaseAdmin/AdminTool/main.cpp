#include "mainwindow.h"
#include <iostream>
#include <QApplication>

template <typename T>
std::string str(T t) {
  if (std::is_same_v<T, std::string>) // строка или преобразуемый в строку
    return t;
  else
    return std::to_string(t);
}

int main(int argc, char *argv[])
{
//    auto t = str(std::string("10"));
//    std::cout << t;

    int mid = 5;
    auto f = [&mid]() mutable ->bool {
            mid++;
            return true;
};
    f();
    std::cout << mid << std::endl;;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

/*
conanbuildinfo.cmake qt cmake
https://doc.qt.io/qtcreator/creator-build-settings-cmake.html#using-cmake-with-conan
https://doc.qt.io/qtcreator/creator-build-settings-conan.html#conan-build-steps
*/
