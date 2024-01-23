/**
 * @file main.cpp
 * @brief Application entry point.
 */

#include "core/mainwindow.h"
#include "globals.h"
#include "utils/logger.h"
#include <QApplication>
#include <QFontDatabase>
#include <QLibraryInfo>
#include <QLocale>
#include <QSettings>
#include <QSplashScreen>
#include <QStyleFactory>
#include <QTranslator>
#include <QXmlStreamReader>
#include <QtAwesome.h>
#include <qforeach.h>
using namespace Globals;

/*!
 * @brief sets the theme for the application.
 * @details Application theme consists of a QPalette for the application and a
 * custom stylesheet for different UI components. works through passing the
 * theme index (0 = light, 1 = sepia , 2 = dark).
 *
 * @param themeIdx - index for theme as it appears in the application settings.
 */
void
setTheme(int themeIdx);
/**
 * @brief loads the QCF fonts for Quran pages before starting the
 * GUI.
 * @details The application depends mainly on QCF fonts to display Quranic
 * verses and pages, the QCF fonts are a set of fonts designed specifically for
 * Madani Mushaf printing. As each page font includes unicode glyphs for each
 * word in that page. The fonts are loaded at application startup by loading 604
 * truetype files (ttf). The available QCF versions are 1 and 2.
 *
 * @param qcfVersion - font version to use.
 */
void
addFonts(int qcfVersion);
/**
 * @brief loads UI translation.
 * @details Application UI translation is done through compiled Qt translation
 * files (.qm). addTranslation() loads the correct qm files for the language
 * given. The loaded translation files consist of a Qt base translation and a
 * custom application translation file.
 *
 * @param localeCode - instance from QLocale::Language that defines a
 * specific language
 */
void
addTranslation(QLocale::Language localeCode);
/**
 * @brief checks the default settings groups and sets them with default
 * values if not found.
 * @details application settings are stored as ini-formatted files in order to
 * provide the same functionality on different platforms. The configuration file
 * is stored in Globals::configDir
 * directory.
 *
 * @param settings - pointer to QSettings instance to access app settings
 */
void
checkSettings(QSettings* settings);
/**
 * @brief set the default values for non-existing keys in a certain settings
 * group (0 - general, 1 - Reader).
 * @param settings - pointer to QSettings instance to access app settings
 * @param group - integer refering to group to check
 */
void
checkSettingsGroup(QSettings* settings, int group);
/**
 * @brief fills the global reciters list and creates their corresponding
 * directories.
 * @details Fills the QList that contains ::Reciter instances
 * that represent the reciters supported by the application. The reciters list
 * is used in many parts of the application by creating a constant reference to
 * the global QList. It also creates the reciters directories in the application
 * recitations directory as needed.
 */
void
populateRecitersList();
/**
 *
 * MODIFIED
 */
void
populateContentLists();
/**
 * @brief populates the Globals::shortcutDescription QMap with key-value pairs
 * of (name - description) and set the default value if shortcut is not found in
 * settings, any new shortcuts should be added to shortcuts.xml along with the
 * default keybinding for it.
 */
void
populateShortcutsMap();
/**
 * @brief set application-wide variables that represents used paths.
 */
void
setGlobalPaths();

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
  QApplication::setApplicationVersion("1.2.2");

  QSplashScreen splash(QPixmap(":/resources/splash.png"));
  splash.show();

  setGlobalPaths();
  Logger::startLogger(configDir.absolutePath());
  Logger::attach();

  settings = new QSettings(
    configDir.filePath("qurancompanion.conf"), QSettings::IniFormat, &a);
  checkSettings(settings);

  themeId = settings->value("Theme").toInt();
  qcfVersion = settings->value("Reader/QCF").toInt();
  language = qvariant_cast<QLocale::Language>(settings->value("Language"));
  readerMode = qvariant_cast<ReaderMode>(settings->value("Reader/Mode"));
  setTheme(themeId);
  addFonts(qcfVersion);
  addTranslation(language);
  populateRecitersList();
  populateShortcutsMap();
  populateContentLists();

  databaseManager = new DBManager(&a);
  MainWindow w(nullptr);
  splash.finish(&w);
  w.show();

  int exitcode = a.exec();
  Logger::stopLogger();
  return exitcode;
}

void
setGlobalPaths()
{
  // data
  assetsDir = QApplication::applicationDirPath() + QDir::separator() + "assets";
  bismillahDir =
    QApplication::applicationDirPath() + QDir::separator() + "bismillah";

  // config & downloads
  if (!configDir.exists("QuranCompanion"))
    configDir.mkpath("QuranCompanion");
  configDir.cd("QuranCompanion");

  if (!downloadsDir.exists("QuranCompanion"))
    downloadsDir.mkpath("QuranCompanion");
  downloadsDir.cd("QuranCompanion");

  if (!downloadsDir.exists("recitations"))
    downloadsDir.mkpath("recitations");
  if (!downloadsDir.exists("QCFV2"))
    downloadsDir.mkpath("QCFV2");
  if (!downloadsDir.exists("tafasir"))
    downloadsDir.mkpath("tafasir");
  if (!downloadsDir.exists("translations"))
    downloadsDir.mkpath("translations");

#ifdef Q_OS_WIN
  updateToolPath = QApplication::applicationDirPath() + QDir::separator() +
                   "QCMaintenanceTool.exe";
#endif
}

void
checkSettings(QSettings* settings)
{
  for (int i = 0; i < 2; i++)
    checkSettingsGroup(settings, i);
}

void
checkSettingsGroup(QSettings* settings, int group)
{
  switch (group) {
    case 0:
      settings->setValue("Language",
                         settings->value("Language", (int)QLocale::English));
      settings->setValue("Theme", settings->value("Theme", 0));
      settings->setValue("VOTD", settings->value("VOTD", true));
      settings->setValue("MissingFileWarning",
                         settings->value("MissingFileWarning", true));
      break;
    case 1:
      settings->beginGroup("Reader");
      settings->setValue("Mode", settings->value("Mode", 0));
      settings->setValue("FGHighlight", settings->value("FGHighlight", 1));
      settings->setValue("Khatmah", settings->value("Khatmah", 0));
      settings->setValue("AdaptiveFont", settings->value("AdaptiveFont", true));
      settings->setValue("QCF1Size", settings->value("QCF1Size", 22));
      settings->setValue("QCF2Size", settings->value("QCF2Size", 20));
      settings->setValue("QCF", settings->value("QCF", 1));
      settings->setValue("VerseType", settings->value("VerseType", 0));
      settings->setValue("VerseFontSize", settings->value("VerseFontSize", 20));
      settings->setValue("Tafsir", settings->value("Tafsir", 6));
      settings->setValue("Translation", settings->value("Translation", 5));
      settings->setValue(
        "SideContentFont",
        settings->value("SideContentFont", QFont("Expo Arabic", 14)));
      settings->endGroup();
      break;
  }
}

void
addFonts(int qcfVersion)
{
  QDir fontsDir;
  fontsDir = QApplication::applicationDirPath() + QDir::separator() + "assets" +
             QDir::separator() + "fonts";

  // ui fonts
  foreach (const QFileInfo& font, fontsDir.entryInfoList(QDir::Files))
    QFontDatabase::addApplicationFont(font.absoluteFilePath());

  // font for surah frames
  QFontDatabase::addApplicationFont(fontsDir.filePath("QCFV1/QCF_BSML.ttf"));
  switch (qcfVersion) {
    case 1:
      fontsDir.cd("QCFV1");
      break;
    case 2:
      fontsDir.setPath(downloadsDir.absolutePath() + "/QCFV2");
      qcfFontPrefix = "QCF2";
      break;
  }

  // add required fonts
  for (int i = 1; i < 605; i++) {
    QString fontName = pageFontname(i) + ".ttf";

    if (qcfVersion == 2 && !fontsDir.exists(fontName)) {
      settings->setValue("Reader/QCF", 1);
      settings->sync();
      qFatal() << fontsDir.filePath(fontName)
               << " font file not found, fallback to QCF v1";
    } else
      QFontDatabase::addApplicationFont(fontsDir.filePath(fontName));
  }

  // set default UI fonts to use
  QStringList uiFonts;
  uiFonts << "Noto Sans Display"
          << "Expo Arabic";
  qApp->setFont(QFont(uiFonts, qApp->font().pointSize()));
}

void
setTheme(int themeIdx)
{
  qApp->setStyle(QStyleFactory::create("Fusion"));

  QPalette themeColors;
  QFile styles, palette;
  switch (themeIdx) {
    case 0:
      themeResources.setPath(":/resources/light/");
      styles.setFileName(themeResources.filePath("light.qss"));
      palette.setFileName(themeResources.filePath("light.xml"));
      darkMode = false;
      break;

    case 1:
      themeResources.setPath(":/resources/light/");
      styles.setFileName(themeResources.filePath("sepia.qss"));
      palette.setFileName(themeResources.filePath("sepia.xml"));
      darkMode = false;
      break;

    case 2:
      themeResources.setPath(":/resources/dark/");
      styles.setFileName(themeResources.filePath("dark.qss"));
      palette.setFileName(themeResources.filePath("dark.xml"));
      darkMode = true;
      break;
  }

  if (!palette.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qCritical() << "Couldn't Read Color palette xml";
  }

  QXmlStreamReader paletteReader(&palette);
  QPalette::ColorGroup group = QPalette::All;
  while (!paletteReader.atEnd() && !paletteReader.hasError()) {
    QXmlStreamReader::TokenType token = paletteReader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (paletteReader.name().toString() == "enabled")
        group = QPalette::All;
      else if (paletteReader.name().toString() == "disabled")
        group = QPalette::Disabled;
      // color element
      else {
        QPalette::ColorRole role;
        int red, green, blue;
        role = static_cast<QPalette::ColorRole>(
          paletteReader.attributes().value("role").toInt());
        red = paletteReader.attributes().value("red").toInt();
        green = paletteReader.attributes().value("green").toInt();
        blue = paletteReader.attributes().value("blue").toInt();

        themeColors.setColor(group, role, QColor(red, green, blue));
      }
    }
  }

  palette.close();
  qApp->setPalette(themeColors);

  // load stylesheet
  if (styles.open(QIODevice::ReadOnly)) {
    qApp->setStyleSheet(styles.readAll());
    styles.close();
  }

  awesome = new fa::QtAwesome(qApp);
  awesome->initFontAwesome();
}

void
addTranslation(QLocale::Language localeCode)
{
  if (localeCode == QLocale::English)
    return;

  QString code = QLocale::languageToCode(localeCode);
  QTranslator *translation = new QTranslator(qApp),
              *qtBase = new QTranslator(qApp);

  if (translation->load(":/i18n/qc_" + code + ".qm")) {
    qInfo() << translation->language() << "translation loaded";
    qInfo() << "base translation:" << qtBase->load(":/base/" + code + ".qm");
    qApp->installTranslator(translation);
    qApp->installTranslator(qtBase);

  } else {
    qWarning() << code + " translation not loaded!";
    delete translation;
    delete qtBase;
  }
}

void
populateRecitersList()
{
  QFile reciters(":/resources/reciters.xml");
  if (!reciters.open(QIODevice::ReadOnly))
    qFatal("Couldn't Open Reciters XML, Exiting");

  QXmlStreamReader reader(&reciters);
  while (!reader.atEnd() && !reader.hasError()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (reader.name().toString() == "reciter") {
        Reciter reciter;
        reciter.baseDirName = reader.attributes().value("dirname").toString();
        reciter.displayName = qApp->translate(
          "MainWindow", reader.attributes().value("display").toLatin1());
        reciter.baseUrl = reader.attributes().value("url").toString();
        reciter.basmallahPath = bismillahDir.absoluteFilePath(
          reader.attributes().value("basmallah").toString());
        reciter.useId = reader.attributes().value("useid").toInt();

        recitersList.append(reciter);
      }
    }
  }

  reciters.close();
  recitersList.squeeze();

  // create reciters directories
  downloadsDir.cd("recitations");
  foreach (const Reciter& r, recitersList) {
    if (!downloadsDir.exists(r.baseDirName))
      downloadsDir.mkdir(r.baseDirName);
  }
  downloadsDir.cdUp();
}

void
populateShortcutsMap()
{
  QFile shortcuts(":/resources/shortcuts.xml");
  if (!shortcuts.open(QIODevice::ReadOnly))
    qCritical("Couldn't Open Shortcuts XML");

  settings->beginGroup("Shortcuts");
  QXmlStreamReader reader(&shortcuts);
  while (!reader.atEnd() && !reader.hasError()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (reader.name().toString() == "shortcut") {
        QString key = reader.attributes().value("key").toString();
        QString defBind = reader.attributes().value("default").toString();
        QString desc =
          qApp->translate("SettingsDialog",
                          reader.attributes().value("description").toLatin1());

        shortcutDescription.insert(key, desc);
        if (!settings->contains(key))
          settings->setValue(key, defBind);
      }
    }
  }

  settings->endGroup();
  shortcuts.close();
}

void
populateContentLists()
{
  QFile content(":/resources/files.xml");
  if (!content.open(QIODevice::ReadOnly))
    qCritical("Couldn't Open Files XML");

  QXmlStreamReader reader(&content);
  while (!reader.atEnd() && !reader.hasError()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (reader.name().toString() == "tafsir") {
        QString name = qApp->translate(
          "SettingsDialog", reader.attributes().value("name").toLatin1());
        QString file = reader.attributes().value("file").toString();
        bool text = reader.attributes().value("text").toInt();
        bool extra = reader.attributes().value("extra").toInt();
        tafasirList.append(Tafsir{ name, file, text, extra });
      }
      // translations
      else if (reader.name().toString() == "translation") {
        QString name = reader.attributes().value("name").toString();
        QString file = reader.attributes().value("file").toString();
        bool extra = reader.attributes().value("extra").toInt();
        translationsList.append(Translation{ name, file, extra });
      }
    }
  }

  content.close();
  tafasirList.squeeze();
  translationsList.squeeze();
}
