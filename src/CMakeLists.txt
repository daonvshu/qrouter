cmake_minimum_required(VERSION 3.12)

project(qrouter VERSION 0.0.8)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_DEBUG_POSTFIX "d")

add_library(${PROJECT_NAME} 
    abstractrouterwidget.cpp
    qrouter.cpp
    routerstackmanager.cpp

    container/layoutroutercontainer.cpp
    container/stackroutercontainer.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)

install(TARGETS ${PROJECT_NAME}
    EXPORT QRouterTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/QRouterConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/QRouterConfig.cmake"
    INSTALL_DESTINATION lib/cmake/QRouter
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/QRouterConfig.cmake"
    DESTINATION lib/cmake/QRouter
)

install(EXPORT QRouterTargets
    FILE QRouterTargets.cmake
    DESTINATION lib/cmake/QRouter
)