cmake_minimum_required(VERSION 3.22)
project(SecStorageLib VERSION 1.0.0 DESCRIPTION "Lib for access to Security Storage")

set(CMAKE_CXX_STANDARD 14)

# Find source files
file(GLOB SOURCES src/*.cpp)

# Include header files
include_directories(include)

# Create shared library
add_library(${PROJECT_NAME} SHARED)

target_sources(${PROJECT_NAME}
    PRIVATE
    ${SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)


file(GLOB_RECURSE EDV_INCLUDE_FILES "include/*.h*")


set_target_properties(${PROJECT_NAME}
    PROPERTIES PUBLIC_HEADER "${EDV_INCLUDE_FILES}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)


configure_file(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

#https://www.pragmaticlinux.com/2022/02/create-a-shared-library-in-c-with-cmake/
#https://stackoverflow.com/questions/17511496/how-to-create-a-shared-library-with-cmake