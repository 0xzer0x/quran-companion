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

set(TS_FILES src/translations/ara.ts)

set(PROJECT_SOURCES
        src/main.cpp
        src/core/mainwindow.cpp
        src/core/mainwindow.h
        src/core/mainwindow.ui
        src/widgets/searchdialog.h
        src/widgets/searchdialog.cpp
        src/widgets/searchdialog.ui
        src/widgets/clickablelabel.cpp
        src/widgets/clickablelabel.h
        src/widgets/settingsdialog.cpp
        src/widgets/settingsdialog.h
        src/widgets/settingsdialog.ui
        src/widgets/downloaderdialog.cpp
        src/widgets/downloaderdialog.h
        src/widgets/downloaderdialog.ui
        src/widgets/downloadprogressbar.cpp
        src/widgets/downloadprogressbar.h
        src/widgets/verseframe.cpp
        src/widgets/verseframe.h
        src/utils/dbmanager.h
        src/utils/dbmanager.cpp
        src/utils/verseplayer.h
        src/utils/verseplayer.cpp
        src/utils/pageconstructor.h
        src/utils/pageconstructor.cpp
        src/utils/downloadmanager.h
        src/utils/downloadmanager.cpp
        resources.qrc
        qurancompanion.rc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(quran-companion
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_add_translation(qmFiles ${TS_FILES})
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET quran-companion APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
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
file(COPY ./audio/bismillah DESTINATION ${PROJECT_BINARY_DIR}/audio)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(quran-companion)
endif()


