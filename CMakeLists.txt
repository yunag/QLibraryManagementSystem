cmake_minimum_required(VERSION 3.16...3.22)

project(
  QLibraryManagementSystem
  VERSION 0.1
  LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(deps)
add_subdirectory(src)
