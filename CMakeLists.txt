cmake_minimum_required(VERSION 3.5)

project(socket)


include_directories(./ ./sensord ./sensord/cpp)
file(GLOB SRC_LIST ./sensord/cpp/*.cpp ./sensord/cpp/*/*.cpp ./sensord/cpp/*/*.hpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17" )
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
# add_executable(${PROJECT_NAME} ${SRC_LIST})
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(socket PUBLIC "-lpthread" "-ljsoncpp" "-lpaho-mqttpp3" "-lpaho-mqtt3as")
