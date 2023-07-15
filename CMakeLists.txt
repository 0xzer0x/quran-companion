cmake_minimum_required(VERSION 3.22)

project(
  quran-companion
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Sql Multimedia Network
                                              LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Multimedia
                                                       Network LinguistTools)

if(WIN32)
  set(Vulkan_INCLUDE_DIR "$ENV{VULKAN_SDK}\\Include\\vulkan")
endif()

if(DEFINED BUILD_FLATPAK)
  message(STATUS "Adding flatpak specific files")
  add_subdirectory(dist/flatpak)
endif()

set(PROJECT_SOURCES
    src/main.cpp
    src/globals.h
    src/globals.cpp
    src/core/mainwindow.cpp
    src/core/mainwindow.h
    src/core/mainwindow.ui
    src/core/searchdialog.h
    src/core/searchdialog.cpp
    src/core/searchdialog.ui
    src/core/settingsdialog.cpp
    src/core/settingsdialog.h
    src/core/settingsdialog.ui
    src/core/downloaderdialog.cpp
    src/core/downloaderdialog.h
    src/core/downloaderdialog.ui
    src/core/bookmarksdialog.h
    src/core/bookmarksdialog.cpp
    src/core/bookmarksdialog.ui
    src/core/tafsirdialog.h
    src/core/tafsirdialog.cpp
    src/core/tafsirdialog.ui
    src/widgets/quranpagebrowser.h
    src/widgets/quranpagebrowser.cpp
    src/widgets/clickablelabel.cpp
    src/widgets/clickablelabel.h
    src/widgets/downloadprogressbar.cpp
    src/widgets/downloadprogressbar.h
    src/widgets/verseframe.cpp
    src/widgets/verseframe.h
    src/widgets/notificationpopup.h
    src/widgets/notificationpopup.cpp
    src/utils/dbmanager.h
    src/utils/dbmanager.cpp
    src/utils/verseplayer.h
    src/utils/verseplayer.cpp
    src/utils/downloadmanager.h
    src/utils/downloadmanager.cpp
    src/utils/notificationmanager.h
    src/utils/notificationmanager.cpp
    src/utils/logger.h
    src/utils/logger.cpp
    resources.qrc
    qurancompanion.rc)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(quran-companion MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
  add_executable(quran-companion ${PROJECT_SOURCES})
endif()

target_link_libraries(
  quran-companion
  PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql
          Qt${QT_VERSION_MAJOR}::Multimedia Qt${QT_VERSION_MAJOR}::Network)

if(WIN32)
  set_target_properties(quran-companion PROPERTIES WIN32_EXECUTABLE TRUE)
elseif(APPLE)
  set_target_properties(
    quran-companion
    PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER com.zer0x.qurancompanion
               MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING
               ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
               MACOSX_BUNDLE TRUE)
endif()

install(
  TARGETS quran-companion
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY ./assets TYPE BIN)
install(DIRECTORY ./bismillah TYPE BIN)

message(STATUS "Copying application assets to build tree")
file(INSTALL assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY bismillah DESTINATION ${CMAKE_BINARY_DIR})

message(STATUS "Adding translations to executable")
qt_add_translations(quran-companion TS_FILES src/translations/qc_ar.ts)

message(STATUS "Copying qt translations")
configure_file("dist/translations/qtbase_ar.qm" "${CMAKE_BINARY_DIR}/ar.qm"
               COPYONLY)

message(STATUS "Creating qt translations resource file")
qt_add_resources(
  quran-companion
  "qtTranslations"
  PREFIX
  "/base"
  BASE
  ${CMAKE_BINARY_DIR}
  FILES
  "${CMAKE_BINARY_DIR}/ar.qm")

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(quran-companion)
endif()
