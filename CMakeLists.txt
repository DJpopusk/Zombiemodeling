cmake_minimum_required(VERSION 3.16)

project(ZombieQtModel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(zombie_model
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/world.cpp
    src/world.h
    src/worldobject.cpp
    src/worldobject.h
    src/zombie.cpp
    src/zombie.h
    src/human.cpp
    src/human.h
    src/qcustomplot.cpp
    src/qcustomplot.h
)

target_link_libraries(zombie_model PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(zombie_model PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
