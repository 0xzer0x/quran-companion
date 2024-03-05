/**
 * @file main.cpp
 * @brief Application entry point.
 */

#include <QApplication>
#include <QSplashScreen>
#include <core/mainwindow.h>
#include <types/reciter.h>
#include <types/tafsir.h>
#include <types/translation.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>
#include <utils/fontmanager.h>
#include <utils/logger.h>
#include <utils/shortcuthandler.h>
#include <utils/stylemanager.h>

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

  Logger::startLogger(DirManager::getInstance().configDir().absolutePath());
  Logger::attach();

  Configuration::getInstance().loadUiTranslation();
  ShortcutHandler::getInstance().populateDescriptionMap();
  StyleManager::getInstance().loadTheme();
  FontManager::getInstance().loadFonts();

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
