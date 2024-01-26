project(kwidgets)

add_library(${PROJECT_NAME}
  kratingpainter.cpp kratingpainter.h
  kratingwidget.cpp kratingwidget.h
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
