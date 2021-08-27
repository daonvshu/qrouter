cmake_minimum_required(VERSION 3.2)
project(qrouterlib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_library(${PROJECT_NAME} 
    abstractrouterwidget.cpp
    qrouter.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)

add_library(qrouter::lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)