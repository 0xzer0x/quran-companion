cmake_minimum_required(VERSION 3.22)

project(
  quran-companion
  LANGUAGES CXX
  VERSION 1.2.6
  DESCRIPTION "Quran reader and player"
  HOMEPAGE_URL "https://0xzer0x.github.io/projects/quran-companion/")

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
    src/dialogs/fileselector.h
    src/dialogs/fileselector.cpp
    src/dialogs/importexportdialog.h
    src/dialogs/importexportdialog.cpp
    src/dialogs/importexportdialog.ui
    src/interfaces/userdataimporter.h
    src/interfaces/userdataexporter.h
    src/interfaces/dbconnection.h
    src/interfaces/notificationsender.h
    src/service/betaqatservice.h
    src/service/bookmarkservice.h
    src/service/quranservice.h
    src/service/glyphservice.h
    src/service/tafsirservice.h
    src/service/translationservice.h
    src/service/khatmahservice.h
    src/service/thoughtsservice.h
    src/service/impl/betaqatservicesqlimpl.h
    src/service/impl/betaqatservicesqlimpl.cpp
    src/service/impl/quranservicesqlimpl.h
    src/service/impl/quranservicesqlimpl.cpp
    src/service/impl/glyphservicesqlimpl.h
    src/service/impl/glyphservicesqlimpl.cpp
    src/service/impl/bookmarkservicesqlimpl.h
    src/service/impl/bookmarkservicesqlimpl.cpp
    src/service/impl/tafsirservicesqlimpl.h
    src/service/impl/tafsirservicesqlimpl.cpp
    src/service/impl/translationservicesqlimpl.h
    src/service/impl/translationservicesqlimpl.cpp
    src/service/impl/khatmahservicesqlimpl.h
    src/service/impl/khatmahservicesqlimpl.cpp
    src/service/impl/thoughtsservicesqlimpl.h
    src/service/impl/thoughtsservicesqlimpl.cpp
    src/notifiers/bookmarksnotifier.h
    src/notifiers/bookmarksnotifier.cpp
    src/notifiers/updatenotifier.h
    src/notifiers/updatenotifier.cpp
    src/notifiers/copynotifier.h
    src/notifiers/copynotifier.cpp
    src/notifiers/jobnotifier.h
    src/notifiers/jobnotifier.cpp
    src/utils/configuration.h
    src/utils/configuration.cpp
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
    src/utils/jsondataimporter.h
    src/utils/jsondataimporter.cpp
    src/utils/servicefactory.h
    src/utils/servicefactory.cpp
    src/utils/stringconverter.h
    src/utils/stringconverter.cpp
    src/downloader/downloadjob.h
    src/downloader/downloadtask.h
    src/downloader/impl/surahjob.h
    src/downloader/impl/surahjob.cpp
    src/downloader/impl/recitationtask.h
    src/downloader/impl/recitationtask.cpp
    src/downloader/impl/taskdownloader.h
    src/downloader/impl/taskdownloader.cpp
    src/downloader/impl/tafsirtask.h
    src/downloader/impl/tafsirtask.cpp
    src/downloader/impl/translationtask.h
    src/downloader/impl/translationtask.cpp
    src/downloader/impl/qcftask.h
    src/downloader/impl/qcftask.cpp
    src/downloader/impl/contentjob.h
    src/downloader/impl/contentjob.cpp
    src/downloader/impl/qcfjob.h
    src/downloader/impl/qcfjob.cpp
    src/downloader/impl/jobmanager.h
    src/downloader/impl/jobmanager.cpp
    src/repository/quranrepository.h
    src/repository/quranrepository.cpp
    src/repository/glyphsrepository.h
    src/repository/glyphsrepository.cpp
    src/repository/betaqatrepository.h
    src/repository/betaqatrepository.cpp
    src/repository/tafsirrepository.h
    src/repository/tafsirrepository.cpp
    src/repository/translationrepository.h
    src/repository/translationrepository.cpp
    src/repository/bookmarksrepository.h
    src/repository/bookmarksrepository.cpp
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
    resources/logo.icns
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
    PROPERTIES
      MACOSX_BUNDLE_ICON_FILE "logo.icns"
      MACOSX_BUNDLE_BUNDLE_NAME "Quran Companion"
      MACOSX_BUNDLE_GUI_IDENTIFIER "io.github._0xzer0x.qurancompanion"
      MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
      MACOSX_BUNDLE_SHORT_VERSION_STRING
      ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
      MACOSX_BUNDLE_LONG_VERSION_STRING
      ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
      MACOSX_BUNDLE TRUE)
  set(QURAN_COMPANION_ICON "resources/logo.icns")
  set_source_files_properties(${QURAN_COMPANION_ICON}
                              PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

install(
  TARGETS quran-companion
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY assets TYPE BIN)
install(DIRECTORY bismillah TYPE BIN)

message(STATUS "Copying application assets to build tree")
if(APPLE)
  file(INSTALL assets DESTINATION "quran-companion.app/Contents/MacOS")
  file(INSTALL bismillah DESTINATION "quran-companion.app/Contents/MacOS")
else()
  file(INSTALL assets DESTINATION ${CMAKE_BINARY_DIR})
  file(INSTALL bismillah DESTINATION ${CMAKE_BINARY_DIR})
endif()

list(APPEND SUPPORTED_LANGUAGES ar tr ru)
foreach(lang IN LISTS SUPPORTED_LANGUAGES)
  message(STATUS "Adding ${lang} translation file to QC_TS")
  list(APPEND QC_TS "dist/translations/out/qc_${lang}.ts")

  message(STATUS "Copying ${lang} qt translations to build tree")
  configure_file("dist/translations/qtbase/${lang}.qm"
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
  "/qtbase"
  BASE
  ${CMAKE_BINARY_DIR}
  FILES
  ${QTBASE_QM})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(quran-companion)
endif()
