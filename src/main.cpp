/**
 * @file main.cpp
 * @brief Application entry point.
 */

#include "core/mainwindow.h"
#include "types/reciter.h"
#include "types/tafsir.h"
#include "types/translation.h"
#include "utils/dirmanager.h"
#include "utils/fontmanager.h"
#include "utils/logger.h"
#include "utils/settings.h"
#include "utils/shortcuthandler.h"
#include "utils/stylemanager.h"
#include <QApplication>
#include <QSplashScreen>

/**
 * @brief application entry point
 * @param argc - the number of arguments passed to the application
 * @param argv - command line arguments passed to the application
 * @return application exit code.
 */
int
main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  QApplication::setApplicationName("Quran Companion");
  QApplication::setOrganizationName("0xzer0x");
  QApplication::setApplicationVersion("1.2.3");

  QSplashScreen splash(QPixmap(":/resources/splash.png"));
  splash.show();

  DirManager::setup();
  Logger::startLogger(DirManager::configDir->absolutePath());
  Logger::attach();

  Settings::setup();
  ShortcutHandler::populateDescriptionMap();
  StyleManager::loadTheme();
  FontManager::loadUiFonts();
  FontManager::loadQcf();
  Settings::loadQmFiles();

  Tafsir::populateTafasir();
  Translation::populateTranslations();
  Reciter::populateReciters();

  MainWindow w(nullptr);
  splash.finish(&w);
  w.show();

  int exitcode = a.exec();
  Logger::stopLogger();
  return exitcode;
}
