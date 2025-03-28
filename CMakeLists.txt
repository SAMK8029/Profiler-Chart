# Copyright (C) 2023 The Qt Company Ltd.
cmake_minimum_required(VERSION 3.16)
project(chartsgallery LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Charts Core Gui Widgets)

qt_standard_project_setup()

add_executable(Profiler main.cpp)

add_subdirectory(Modules/Chart/)

target_link_libraries(Profiler PUBLIC Chart)


