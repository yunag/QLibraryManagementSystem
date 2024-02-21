set(PROJECT_SOURCES
    forms/loginform.ui
    forms/bookadddialog.ui
    forms/bookcard.ui
    forms/booksection.ui
    forms/librarymainwindow.ui
    forms/librarysidemenu.ui
    forms/searchfilterdialog.ui
    forms/twowaylist.ui
    main.cpp
    ../resource.qrc
    database/librarydatabase.h
    database/schema.h
    database/book.h
    database/book.cpp
    database/librarydatabase.cpp
    database/bookauthor.h
    database/bookauthor.cpp
    database/author.cpp
    database/author.h
    database/category.cpp
    database/category.h
    database/bookcategory.h
    libraryapplication.cpp
    librarymainwindow.cpp
    bookadddialog.h
    widgetutils.cpp
    bookcard.h
    user.cpp
    booksection.h
    qlabeldragdrop.cpp
    booksectiondao.h
    booksection.cpp
    clickableframe.h
    bookcard.cpp
    dragdroplistview.h
    bookadddialog.cpp
    fadebutton.h
    passwordlineedit.cpp
    imagepickerlabel.h
    booksectiondao.cpp
    libraryapplication.h
    previewlineedit.cpp
    librarymainwindow.h
    imagepickerlabel.cpp
    librarysidemenu.h
    twowaylist.cpp
    loginform.h
    dragdroplistview.cpp
    passwordlineedit.h
    fadebutton.cpp
    previewlineedit.h
    qlabeldragdrop.h
    searchfilterdialog.h
    searchfilterdialog.cpp
    twowaylist.h
    librarysidemenu.cpp
    user.h
    loginform.cpp
    widgetutils.h
    clickableframe.cpp)

# list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
  add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql
                                              Qt${QT_VERSION_MAJOR}::Concurrent kwidgets)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(${PROJECT_NAME} PRIVATE -fno-omit-frame-pointer -fno-common -Wall -Wextra -pedantic)

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(${PROJECT_NAME})
endif()
