cmake_minimum_required(VERSION 3.5)

project(quran-companion VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("./include/")

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Multimedia Network LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Multimedia Network LinguistTools)

set(TS_FILES)

set(PROJECT_SOURCES
        main.cpp
        sources/interfaces/mainwindow.cpp
        headers/interfaces/mainwindow.h
        sources/interfaces/clickablelabel.cpp
        headers/interfaces/clickablelabel.h
        sources/interfaces/settingsdialog.cpp
        headers/interfaces/settingsdialog.h
        sources/interfaces/downloaderdialog.cpp
        headers/interfaces/downloaderdialog.h
        sources/interfaces/downloadprogressbar.cpp
        headers/interfaces/downloadprogressbar.h
        sources/interfaces/verseframe.cpp
        headers/interfaces/verseframe.h
        headers/dbmanager.h
        sources/dbmanager.cpp
        headers/verseplayer.h
        sources/verseplayer.cpp
        headers/pageconstructor.h
        sources/pageconstructor.cpp
        headers/downloadmanager.h
        sources/downloadmanager.cpp
        sources/mainwindow.ui
        sources/settingsdialog.ui
        sources/downloaderdialog.ui
        resources.qrc
        images/qurancompanion.rc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(quran-companion
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET quran-companion APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(quran-companion SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(quran-companion
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(quran-companion PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::Network)

set_target_properties(quran-companion PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS quran-companion
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

file(COPY ./assets DESTINATION ${PROJECT_BINARY_DIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(quran-companion)
endif()


