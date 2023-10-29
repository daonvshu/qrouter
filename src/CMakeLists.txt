cmake_minimum_required(VERSION 3.2)
project(qrouterlib)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

add_library(${PROJECT_NAME} 
    abstractrouterwidget.cpp
    qrouter.cpp
    routerstackmanager.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ../include)

add_library(qrouter::lib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)