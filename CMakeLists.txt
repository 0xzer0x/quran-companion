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

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Multimedia Network
                                     LinguistTools)

if(WIN32)
  set(Vulkan_INCLUDE_DIR "$ENV{VULKAN_SDK}\\Include\\vulkan")
endif()

if(DEFINED BUILD_FLATPAK)
  message(STATUS "Adding flatpak specific files")
  add_subdirectory(dist/flatpak)
endif()

add_subdirectory(third_party/QtAwesome)

include_directories(src)

set(PROJECT_SOURCES
    src/main.cpp
    src/types/verse.h
    src/types/verse.cpp
    src/types/reciter.h
    src/types/reciter.cpp
    src/types/content.h
    src/types/content.cpp
    src/types/tafsir.h
    src/types/tafsir.cpp
    src/types/translation.h
    src/types/translation.cpp
    src/core/mainwindow.cpp
    src/core/mainwindow.h
    src/core/mainwindow.ui
    src/core/quranreader.h
    src/core/quranreader.cpp
    src/core/quranreader.ui
    src/core/playercontrols.h
    src/core/playercontrols.cpp
    src/core/playercontrols.ui
    src/dialogs/searchdialog.h
    src/dialogs/searchdialog.cpp
    src/dialogs/searchdialog.ui
    src/dialogs/settingsdialog.cpp
    src/dialogs/settingsdialog.h
    src/dialogs/settingsdialog.ui
    src/dialogs/downloaderdialog.cpp
    src/dialogs/downloaderdialog.h
    src/dialogs/downloaderdialog.ui
    src/dialogs/bookmarksdialog.h
    src/dialogs/bookmarksdialog.cpp
    src/dialogs/bookmarksdialog.ui
    src/dialogs/contentdialog.h
    src/dialogs/contentdialog.cpp
    src/dialogs/contentdialog.ui
    src/dialogs/khatmahdialog.h
    src/dialogs/khatmahdialog.cpp
    src/dialogs/khatmahdialog.ui
    src/dialogs/copydialog.h
    src/dialogs/copydialog.cpp
    src/dialogs/copydialog.ui
    src/dialogs/aboutdialog.h
    src/dialogs/aboutdialog.h
    src/dialogs/aboutdialog.cpp
    src/dialogs/aboutdialog.ui
    src/dialogs/versedialog.h
    src/dialogs/versedialog.cpp
    src/dialogs/versedialog.ui
    src/dialogs/aboutdialog.cpp
    src/dialogs/aboutdialog.ui
    src/dialogs/versedialog.h
    src/dialogs/versedialog.cpp
    src/dialogs/versedialog.ui
    src/interfaces/userdataimporter.h
    src/interfaces/userdataexporter.h
    src/interfaces/downloadjob.h
    src/interfaces/downloadtask.h
    src/interfaces/dbconnection.h
    src/utils/settings.h
    src/utils/settings.cpp
    src/utils/shortcuthandler.h
    src/utils/shortcuthandler.cpp
    src/utils/verseplayer.h
    src/utils/verseplayer.cpp
    src/utils/systemtray.h
    src/utils/systemtray.cpp
    src/utils/logger.h
    src/utils/logger.cpp
    src/utils/dirmanager.h
    src/utils/dirmanager.cpp
    src/utils/stylemanager.h
    src/utils/stylemanager.cpp
    src/utils/fontmanager.h
    src/utils/fontmanager.cpp
    src/utils/versionchecker.h
    src/utils/versionchecker.cpp
    src/utils/jsondataexporter.h
    src/utils/jsondataexporter.cpp
    src/downloader/surahjob.h
    src/downloader/surahjob.cpp
    src/downloader/recitationtask.h
    src/downloader/recitationtask.cpp
    src/downloader/taskdownloader.h
    src/downloader/taskdownloader.cpp
    src/downloader/tafsirtask.h
    src/downloader/tafsirtask.cpp
    src/downloader/translationtask.h
    src/downloader/translationtask.cpp
    src/downloader/qcftask.h
    src/downloader/qcftask.cpp
    src/downloader/contentjob.h
    src/downloader/contentjob.cpp
    src/downloader/qcfjob.h
    src/downloader/qcfjob.cpp
    src/downloader/jobmanager.h
    src/downloader/jobmanager.cpp
    src/database/qurandb.h
    src/database/qurandb.cpp
    src/database/glyphsdb.h
    src/database/glyphsdb.cpp
    src/database/betaqatdb.h
    src/database/betaqatdb.cpp
    src/database/tafsirdb.h
    src/database/tafsirdb.cpp
    src/database/translationdb.h
    src/database/translationdb.cpp
    src/database/bookmarksdb.h
    src/database/bookmarksdb.cpp
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
    src/widgets/inputfield.h
    src/widgets/inputfield.cpp
    src/widgets/shortcutdelegate.h
    src/widgets/shortcutdelegate.cpp
    src/widgets/betaqaviewer.h
    src/widgets/betaqaviewer.cpp
    src/widgets/betaqaviewer.ui
    resources.qrc
    qurancompanion.rc)

qt_add_executable(quran-companion MANUAL_FINALIZATION ${PROJECT_SOURCES})

target_link_libraries(
  quran-companion PRIVATE Qt6::Widgets Qt6::Sql Qt6::Multimedia Qt6::Network
                          QtAwesome)

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
install(DIRECTORY assets TYPE BIN)
install(DIRECTORY bismillah TYPE BIN)

message(STATUS "Copying application assets to build tree")
file(INSTALL assets DESTINATION ${CMAKE_BINARY_DIR})
file(INSTALL bismillah DESTINATION ${CMAKE_BINARY_DIR})

list(APPEND SUPPORTED_LANGUAGES ar tr)
foreach(lang IN LISTS SUPPORTED_LANGUAGES)
  message(STATUS "Adding ${lang} translation file to QC_TS")
  list(APPEND QC_TS "dist/translations/qc_${lang}.ts")

  message(STATUS "Copying ${lang} qt translations to build tree")
  configure_file("dist/translations/qtbase_${lang}.qm"
                 "${CMAKE_BINARY_DIR}/${lang}.qm" COPYONLY)

  message(STATUS "Adding ${lang} qtbase qm file to QTBASE_QM")
  list(APPEND QTBASE_QM "${CMAKE_BINARY_DIR}/${lang}.qm")
endforeach()

message(STATUS "Creating qt translations resource file")
qt_add_translations(quran-companion TS_FILES ${QC_TS})
qt_add_resources(
  quran-companion
  "qttranslations"
  PREFIX
  "/base"
  BASE
  ${CMAKE_BINARY_DIR}
  FILES
  ${QTBASE_QM})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(quran-companion)
endif()
