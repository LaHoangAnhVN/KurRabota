cmake_minimum_required(VERSION 3.22)
project(SecStorageDaemon)

find_package(SecStorageLib REQUIRED)

file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS src/*.h)
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

add_executable(Client client.cpp)

target_link_libraries(${PROJECT_NAME} SecStorageLib)
target_link_libraries(Client SecStorageLib)
