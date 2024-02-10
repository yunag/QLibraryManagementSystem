cmake_minimum_required(VERSION 3.5)

project(QLibraryManagementSystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Concurrent)

add_subdirectory(deps)

set(PROJECT_SOURCES
        main.cpp
        loginform.cpp
        loginform.h
        loginform.ui
        resource.qrc
        librarymainwindow.h librarymainwindow.cpp librarymainwindow.ui
        librarydatabase.h librarydatabase.cpp
        widgetutils.h widgetutils.cpp
        book.h book.cpp
        qlabeldragdrop.h qlabeldragdrop.cpp
        booksection.h booksection.cpp booksection.ui
        bookcard.h bookcard.cpp bookcard.ui
        bookadddialog.h bookadddialog.cpp bookadddialog.ui
        booksectiondao.h booksectiondao.cpp
        previewlineedit.h previewlineedit.cpp
        imagepickerlabel.h imagepickerlabel.cpp
        twowaylist.h twowaylist.cpp twowaylist.ui
        dragdroplistview.h dragdroplistview.cpp
        fadebutton.h fadebutton.cpp
        bookcategory.h bookcategory.cpp
        bookauthor.h bookauthor.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Concurrent
        kwidgets
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    -fno-omit-frame-pointer
    -fno-common
    -Wall
    -Wextra
    -pedantic
)

set_target_properties(QLibraryManagementSystem PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS QLibraryManagementSystem
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QLibraryManagementSystem)
endif()
