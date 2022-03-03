cmake_minimum_required(VERSION 3.5)

project(QOrderedJsonModel
    VERSION 0.1
    LANGUAGES CXX)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

include_directories(json)

set(PROJECT_SOURCES
    main.cpp
    qorderedjsonmodel.h
    qorderedjsonmodel.cpp
    qjsontreeitem.h
    qjsontreeitem.cpp
    )

add_executable(QOrderedJsonModel
    ${PROJECT_SOURCES}
    )

target_link_libraries(QOrderedJsonModel PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(QOrderedJsonModel PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    )

