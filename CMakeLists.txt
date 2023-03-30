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

set(QT_TR_DIR "C:/Qt/6.4.3/mingw_64/translations")

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
        src/utils/logger.h
        src/utils/logger.cpp
        resources.qrc
        qurancompanion.rc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(quran-companion
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

else()
    if(ANDROID)
        add_library(quran-companion SHARED
            ${PROJECT_SOURCES}
        )

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

qt_add_translations(quran-companion TS_FILES ./src/translations/quran_companion_ar.ts)


install(TARGETS quran-companion
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

file(COPY ./assets DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ./audio/bismillah DESTINATION ${PROJECT_BINARY_DIR}/audio)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/translations)
file(COPY_FILE ${QT_TR_DIR}/qtbase_ar.qm ${PROJECT_BINARY_DIR}/translations/qt_ar.qm)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(quran-companion)
endif()


