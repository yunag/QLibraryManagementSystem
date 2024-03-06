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
    forms/authorsection.ui)

set(PROJECT_DB_SOURCES
    database/bookdetailsdao.h
    database/bookdetailsdao.cpp
    database/librarydatabase.h
    database/librarydatabase.cpp
    database/schema.h
    database/book.h
    database/book.cpp
    database/bookauthor.h
    database/bookauthor.cpp
    database/author.h
    database/author.cpp
    database/category.h
    database/category.cpp
    database/bookcategory.h
    database/bookcategory.cpp
    database/booksectiondao.h
    database/booksectiondao.cpp
    database/authordetailsdao.h
    database/authordetailsdao.cpp)

set(PROJECT_SOURCES
    main.cpp
    ../resource.qrc
    bookdetailsdialog.h
    bookdetailsdialog.cpp
    libraryapplication.h
    libraryapplication.cpp
    librarymainwindow.h
    librarymainwindow.cpp
    bookadddialog.h
    bookadddialog.cpp
    widgetutils.h
    widgetutils.cpp
    bookcard.cpp
    bookcard.h
    user.h
    user.cpp
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
    imageloader.h
    imageloader.cpp
    authorsection.h
    authorsection.cpp
    ${PROJECT_FORMS}
    ${PROJECT_DB_SOURCES})

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
