cmake_minimum_required(VERSION 3.14)
project(kwidgets LANGUAGES CXX)

add_library(kwidgets kratingpainter.cpp kratingpainter.h kratingwidget.cpp kratingwidget.h)

set_target_properties(kwidgets PROPERTIES AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(kwidgets PRIVATE Qt6::Widgets)

target_include_directories(kwidgets PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
