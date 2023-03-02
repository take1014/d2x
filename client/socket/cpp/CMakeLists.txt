cmake_minimum_required(VERSION 3.5)

project(socket)

# include_directories(./)
file(GLOB SRC_LIST ./*.cpp ./*.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14" )
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(socket PUBLIC "-lpthread" "-ljsoncpp" "-lpaho-mqttpp3" "-lpaho-mqtt3as")
