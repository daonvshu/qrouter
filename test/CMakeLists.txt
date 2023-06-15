cmake_minimum_required(VERSION 3.2)
project(qroutertest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

add_executable(${PROJECT_NAME} WIN32
    main.cpp
    qrouter.qrc
    qrouter.ui
    qroutertest.h
    qroutertest.cpp

    pages/customdata.h
    pages/page1.ui
    pages/testpage1.h
    pages/testpage1.cpp
    pages/page2.ui
    pages/testpage2.h
    pages/testpage2.cpp
)

target_link_libraries(${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    qrouter::lib
)