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

set(PROJECT_SCHEMA_SOURCES
    schema/author.h
    schema/book.h
    schema/bookauthor.h
    schema/bookcategory.h
    schema/category.h
    schema/schema.h
    schema/schema.cpp)

set(PROJECT_COMMON_SOURCES
    common/json.h
    common/json.cpp
    common/widgetutils.h
    common/widgetutils.cpp
    common/error.h
    common/error.cpp
    common/algorithm.h)

set(PROJECT_CONTROLLERS_SOURCES
    controllers/bookcontroller.h
    controllers/bookcontroller.cpp
    controllers/bookratingcontroller.h
    controllers/bookratingcontroller.cpp
    controllers/authorbookcontroller.h
    controllers/authorbookcontroller.cpp
    controllers/bookcategorycontroller.h
    controllers/bookcategorycontroller.cpp
    controllers/categorycontroller.h
    controllers/categorycontroller.cpp
    controllers/authorcontroller.h
    controllers/authorcontroller.cpp)

set(PROJECT_NETWORK_SOURCES network/network.h network/network.cpp network/userauthentication.h
                            network/userauthentication.cpp network/imageloader.h network/imageloader.cpp)

set(PROJECT_SOURCES
    main.cpp
    ../resources/resource.qrc
    bookdetailsdialog.h
    bookdetailsdialog.cpp
    loadingmodel.h
    loadingmodel.cpp
    bookrestmodel.h
    bookrestmodel.cpp
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
    bookcarddelegate.h
    bookcarddelegate.cpp
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
    ${PROJECT_DAO_SOURCES}
    ${PROJECT_SCHEMA_SOURCES}
    ${PROJECT_NETWORK_SOURCES}
    ${PROJECT_COMMON_SOURCES}
    ${PROJECT_CONTROLLERS_SOURCES})

qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})

set_property(
  TARGET ${PROJECT_NAME}
  APPEND
  PROPERTY AUTOUIC_SEARCH_PATHS forms)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Sql Qt6::Network Qt6::Concurrent kwidgets)

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

qt_finalize_executable(${PROJECT_NAME})
