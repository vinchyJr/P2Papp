cmake_minimum_required(VERSION 3.16)

project(P2PApp VERSION 0.1 LANGUAGES CXX)

# AutoMoc : Qt gère automatiquement les fichiers MOC (Meta-Object Compiler)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Utilisation de C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Trouver et inclure Qt6
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core WebSockets)

# Vérification de WebSockets
message(STATUS "Qt WebSockets found: ${Qt6WebSockets_FOUND}")

# Fichiers sources
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    createroom.cpp
    createroom.h
    joinroom.cpp
    joinroom.h
    peer.cpp  # 🔹 Assure-toi que peer.cpp est là mais PAS peer.h
)

# Créer l'exécutable
add_executable(P2PApp ${PROJECT_SOURCES})

# Lier les bibliothèques Qt nécessaires
target_link_libraries(P2PApp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::WebSockets)
