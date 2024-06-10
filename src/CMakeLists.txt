set(PROJECT_FORMS
    forms/loginform.ui
    forms/bookadddialog.ui
    forms/booksection.ui
    forms/librarymainwindow.ui
    forms/librarysidemenu.ui
    forms/booksearchfilterdialog.ui
    forms/twowaylist.ui
    forms/bookdetailsdialog.ui
    forms/authordetailsdialog.ui
    forms/authoradddialog.ui
    forms/authorsection.ui
    forms/pagination.ui
    forms/authorsearchfilterdialog.ui)

set(PROJECT_SOURCES
    main.cpp
    ../resources/resource.qrc
    bookdetailsdialog.h
    bookdetailsdialog.cpp
    libraryapplication.h
    libraryapplication.cpp
    librarymainwindow.h
    librarymainwindow.cpp
    aspectratiolabel.h
    aspectratiolabel.cpp
    bookadddialog.h
    bookadddialog.cpp
    bookcard.h
    bookcard.cpp
    booksection.h
    booksection.cpp
    qlabeldragdrop.h
    qlabeldragdrop.cpp
    clickableframe.h
    clickableframe.cpp
    dragdroplistview.h
    dragdroplistview.cpp
    fadebutton.h
    fadebutton.cpp
    passwordlineedit.h
    passwordlineedit.cpp
    imagepickerlabel.h
    imagepickerlabel.cpp
    previewlineedit.h
    previewlineedit.cpp
    librarysidemenu.h
    librarysidemenu.cpp
    twowaylist.cpp
    twowaylist.h
    loginform.h
    loginform.cpp
    booksearchfilterdialog.h
    booksearchfilterdialog.cpp
    smoothscrollarea.h
    smoothscrollarea.cpp
    smoothscrollbar.h
    smoothscrollbar.cpp
    authordetailsdialog.h
    authordetailsdialog.cpp
    authorsection.h
    authorsection.cpp
    pagination.h
    pagination.cpp
    paginationwidget.h
    paginationwidget.cpp
    gridview.h
    gridview.cpp
    elidedlabel.h
    elidedlabel.cpp
    resourcemanager.h
    resourcemanager.cpp
    authoradddialog.h
    authoradddialog.cpp
    authorsearchfilterdialog.h
    authorsearchfilterdialog.cpp
    ${PROJECT_FORMS})

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent Network Svg StateMachine)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# BUG: Do not use Ninja (https://gitlab.kitware.com/cmake/cmake/-/issues/16776)
set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES AUTOMOC ON
             AUTOUIC ON
             AUTORCC ON
             AUTOUIC_SEARCH_PATHS "forms")

add_subdirectory(model)
add_subdirectory(network)
add_subdirectory(delegate)
add_subdirectory(schema)
add_subdirectory(common)
add_subdirectory(controllers)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Network Qt6::Concurrent Qt6::Svg Qt6::StateMachine
                                              kwidgets)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE -fno-omit-frame-pointer
          -fno-common
          -Wall
          -Wextra
          -Wpedantic
          -Wcast-qual
          -Wconversion
          -Wdouble-promotion
          -Wduplicated-branches
          -Wduplicated-cond
          -Wfloat-equal
          -Wformat=2
          -Wformat-signedness
          -Wlogical-op
          -Wmissing-declarations
          # -Wpadded
          -Wshadow
          -Wswitch-default
          -Wswitch-enum
          -Wundef
          -Wunused-macros
          -Wwrite-strings)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

install(
  TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
