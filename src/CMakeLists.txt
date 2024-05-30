set(PROJECT_FORMS
    forms/loginform.ui
    forms/bookadddialog.ui
    forms/bookcard.ui
    forms/booksection.ui
    forms/librarymainwindow.ui
    forms/librarysidemenu.ui
    forms/searchfilterdialog.ui
    forms/twowaylist.ui
    forms/bookdetailsdialog.ui
    forms/authordetailsdialog.ui
    forms/authorsection.ui
    forms/pagination.ui)

set(PROJECT_DAO_SOURCES dao/authordetailsdao.h dao/authordetailsdao.cpp)

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
    searchfilterdialog.h
    searchfilterdialog.cpp
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
    elidedlabel.h
    elidedlabel.cpp
    ${PROJECT_FORMS}
    ${PROJECT_DAO_SOURCES})

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Concurrent Network Svg)

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

add_subdirectory(model)
add_subdirectory(network)
add_subdirectory(delegate)
add_subdirectory(schema)
add_subdirectory(common)
add_subdirectory(controllers)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Sql Qt6::Network Qt6::Concurrent Qt6::Svg kwidgets)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

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

set_property(
  TARGET ${PROJECT_NAME}
  APPEND
  PROPERTY AUTOUIC_SEARCH_PATHS forms)

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
