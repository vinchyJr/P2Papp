cmake_minimum_required(VERSION 3.16)

project(P2PApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    createroom.cpp
    createroom.h
    joinroom.cpp
    joinroom.h
)

add_executable(P2PApp ${PROJECT_SOURCES}
    p2pserver.h p2pserver.cpp
    p2pserver.h p2pserver.cpp)

target_link_libraries(P2PApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
