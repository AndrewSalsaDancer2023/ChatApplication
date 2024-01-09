cmake_minimum_required(VERSION 3.5)

project(Chat LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
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

find_package(Qt5 COMPONENTS Core Quick WebSockets REQUIRED)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

if(ANDROID)
    add_library(Chat SHARED
      main.cpp
      qml.qrc
    )
else()
    add_executable(Chat
      main.cpp
      authenticationform.h
      authenticationform.cpp
      stringlistmodel.h
      stringlistmodel.cpp
      conversationmodel.h
      conversationmodel.cpp
      participantmodel.h
      participantmodel.cpp
      coordinator.h
      coordinator.cpp
      commonutils.h
      commonutils.cpp
      ../../Server/build/authentication.pb.h
      ../../Server/build/authentication.pb.cc
      ../../Server/build/messages.pb.h
      ../../Server/build/messages.pb.cc
      ../../Server/src/common_types.h
      ../../Server/src/database/DatabaseTypes.h
      ../../Server/src/database/DatabaseTypes.cpp
      ../../Server/src/security.h
      ../../Server/src/security.cpp
      ../../DatabaseAdmin/AdminTool/ServerCommunication/servercommunicator.h
      ../../DatabaseAdmin/AdminTool/ServerCommunication/servercommunicator.cpp
      ../../DatabaseAdmin/AdminTool/ServerCommunication/authutils.h
      ../../DatabaseAdmin/AdminTool/ServerCommunication/authutils.cpp
      ../../DatabaseAdmin/AdminTool/ServerCommunication/messageserialization.h
      ../../DatabaseAdmin/AdminTool/ServerCommunication/messageserialization.cpp
      ../../DatabaseAdmin/AdminTool/ServerCommunication/coordinatorinterface.h
      ../../DatabaseAdmin/AdminTool/ServerCommunication/parametersserialization.h
      ../../DatabaseAdmin/AdminTool/ServerCommunication/parametersserialization.cpp
      qml.qrc
      ${PROTO_SRCS} ${PROTO_HDRS}
    )
endif()

target_compile_definitions(Chat
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(Chat
  PRIVATE Qt5::Core Qt5::Quick Qt5::WebSockets ${Protobuf_LIBRARIES})
