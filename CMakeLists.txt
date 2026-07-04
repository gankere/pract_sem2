cmake_minimum_required(VERSION 3.16)
project(PodcastUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Test)
qt_standard_project_setup()

# === Основное приложение ===
qt_add_executable(PodcastUI
    main.cpp
    mainwindow.h mainwindow.cpp
)

target_link_libraries(PodcastUI PRIVATE Qt6::Widgets)