cmake_minimum_required(VERSION 3.5)

project(AdminTool LANGUAGES CXX)
#execute_process(COMMAND conan install --update --build=missing ${CMAKE_SOURCE_DIR})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup(TARGETS)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS WebSockets REQUIRED)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

if(ANDROID)
  add_library(AdminTool SHARED
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
  )
else()
  add_executable(AdminTool
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    authenticatedialog.cpp
    authenticatedialog.h
    authenticatedialog.ui
    ../../Server/build/authentication.pb.h
    ../../Server/build/authentication.pb.cc    
    ../../Server/build/messages.pb.h
    ../../Server/build/messages.pb.cc
    ../../Server/src/common_types.h
    ../../Server/src/database/DatabaseTypes.h
    ../../Server/src/database/DatabaseTypes.cpp
    ../../Server/src/security.h
    ../../Server/src/security.cpp
    parametersserialization.h
    parametersserialization.cpp
    messageserialization.h
    messageserialization.cpp
    ${PROTO_SRCS} ${PROTO_HDRS}
  )
endif()

target_link_libraries(AdminTool PRIVATE Qt5::Widgets Qt5::WebSockets Qt5::Core  ${Protobuf_LIBRARIES})
