cmake_minimum_required(VERSION 3.14)
project(kwidgets LANGUAGES CXX)

add_library(${PROJECT_NAME} kratingpainter.cpp kratingpainter.h kratingwidget.cpp kratingwidget.h)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
