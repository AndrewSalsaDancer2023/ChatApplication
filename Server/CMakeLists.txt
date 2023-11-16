cmake_minimum_required(VERSION 2.8.12)

project(ChatServer CXX)
# Исходый код будет компилироваться с поддержкой стандарта С++ 20
set(CONAN_DISABLE_CHECK_COMPILER true)

# Подключаем сгенерированный скрипт conanbuildinfo.cmake, созданный Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# Выполняем макрос из conanbuildinfo.cmake, который настроит СMake на работу с библиотеками, установленными Conan
conan_basic_setup()
# Ищем Boost версии 1.78

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  # Boost найден, добавляем к каталогам заголовочных файлов проекта путь к
  # заголовочным файлам Boost
  include_directories(${Boost_INCLUDE_DIRS})
endif()

# Платформы вроде linux требуют подключения библиотеки pthread для
# поддержки стандартных потоков.
# Следующие две строки подключат эту библиотеку на таких платформах
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Protobuf REQUIRED)
#set(Protobuf_PROTOC_EXECUTABLE /usr/bin/protoc)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ./protobuf/messages.proto ./protobuf/authentication.proto)

add_executable(ChatServer 
  src/main.cpp 
  src/beast.hpp
  src/http_session.cpp
  src/http_session.hpp
  src/listener.cpp
  src/listener.hpp
  src/utilities.cpp
  src/messageserialization.cpp
  src/parametersserialization.cpp
  src/request_handlers.h
  src/request_handlers.cpp
  src/main.cpp
  src/net.hpp
  src/shared_state.cpp
  src/shared_state.hpp
  src/websocket_session.cpp
  src/websocket_session.hpp 
  src/security.cpp
  src/database/MessageDecoder.cpp
  src/database/MongoRoutines.cpp
  ${PROTO_SRCS} ${PROTO_HDRS})

# Просим компоновщик подключить библиотеку для поддержки потоков
target_link_libraries(ChatServer PRIVATE ${CONAN_LIBS} ${Protobuf_LIBRARIES} Threads::Threads)
