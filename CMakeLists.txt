cmake_minimum_required(VERSION 3.5)

project(Lab_work VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    methodologydialog.cpp
    methodologydialog.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Lab_work
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        experimentwindow.h experimentwindow.cpp
        digitaldisplay.h digitaldisplay.cpp
        calorimeterscene.h calorimeterscene.cpp
        material.h material.cpp
    )
else()
    if(ANDROID)
        add_library(Lab_work SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Lab_work
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Lab_work PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(Lab_work PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Lab_work)
endif()
