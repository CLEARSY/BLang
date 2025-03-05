cmake_minimum_required(VERSION 3.14)
project(btype 
    VERSION 1.0.0
    DESCRIPTION "B Type System Library"
    LANGUAGES CXX
)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)

# Add compile options
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

find_package(fmt REQUIRED)
find_package(Threads REQUIRED)
find_package(tinyxml2 REQUIRED)

add_subdirectory(src)

# Enable testing
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Installation configuration
include(GNUInstallDirs)

install(TARGETS BLang DESTINATION ${CMAKE_INSTALL_LIBDIR})
