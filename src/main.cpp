/**
 * @file main.cpp
 * @brief Application entry point.
 */

#include "core/mainwindow.h"
#include "globals.h"
#include "utils/logger.h"
#include <QApplication>
#include <QFontDatabase>
#include <QLocale>
#include <QSettings>
#include <QSplashScreen>
#include <QStyleFactory>
#include <QTranslator>
using namespace Globals;

/*!
 * @brief sets the theme for the application.
 * @details Application theme consists of a QPalette for the application and a
 * custom stylesheet for different UI components. works through passing the
 * theme index (0 = light, 1 = dark).
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
 * group (0 - general, 1 - Reader, 2 - Shortcuts).
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
 * @brief populates the Globals::shortcutDescription QMap with key-value pairs
 * of (name - description), any new shortcuts should be added here along with
 * the default keybinding for it in the shortcuts settings group.
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
  QApplication::setApplicationVersion("1.1.8");

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
  if (!recitationsDir.exists("QuranCompanion/recitations"))
    recitationsDir.mkpath("QuranCompanion/recitations");

  configDir.cd("QuranCompanion");
  recitationsDir.cd("QuranCompanion/recitations");

  updateToolPath = QApplication::applicationDirPath() + QDir::separator() +
                   "QCMaintenanceTool";
#ifdef Q_OS_WIN
  updateToolPath.append(".exe");
#endif
}

void
checkSettings(QSettings* settings)
{
  QStringList groups;
  groups << "Reader"
         << "Shortcuts";

  checkSettingsGroup(settings, 0);
  if (settings->childGroups() != groups) {
    for (int i = 0; i < groups.size(); i++) {
      if (i >= settings->childGroups().size() ||
          settings->childGroups().at(i) != groups.at(i))
        checkSettingsGroup(settings, i + 1);
    }
  }
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
      settings->setValue("Page", settings->value("Page", 1));
      settings->setValue("Surah", settings->value("Surah", 1));
      settings->setValue("Verse", settings->value("Verse", 1));
      settings->setValue("AdaptiveFont", settings->value("AdaptiveFont", true));
      settings->setValue("QCF1Size", settings->value("QCF1Size", 22));
      settings->setValue("QCF2Size", settings->value("QCF2Size", 20));
      settings->setValue("QCF", settings->value("QCF", 1));
      settings->setValue("Tafsir", settings->value("Tafsir", 1));
      settings->setValue("Translation", settings->value("Translation", 5));
      settings->setValue(
        "SideContentFont",
        settings->value("SideContentFont", QFont("Expo Arabic", 14)));
      settings->endGroup();
      break;
    case 2:
      settings->beginGroup("Shortcuts");
      settings->setValue("ToggleMenubar",
                         settings->value("ToggleMenubar", "Ctrl+M"));
      settings->setValue("ToggleNavDock",
                         settings->value("ToggleNavDock", "Ctrl+N"));
      settings->setValue("TogglePlayback",
                         settings->value("TogglePlayback", "Space"));
      settings->setValue("VolumeUp", settings->value("VolumeUp", "+"));
      settings->setValue("VolumeDown", settings->value("VolumeDown", "-"));
      settings->setValue("NextPage", settings->value("NextPage", "Left"));
      settings->setValue("PrevPage", settings->value("PrevPage", "Right"));
      settings->setValue("NextVerse", settings->value("NextVerse", "V"));
      settings->setValue("PrevVerse", settings->value("PrevVerse", "Shift+V"));
      settings->setValue("NextSurah", settings->value("NextSurah", "S"));
      settings->setValue("PrevSurah", settings->value("PrevSurah", "Shift+S"));
      settings->setValue("NextJuz", settings->value("NextJuz", "J"));
      settings->setValue("PrevJuz", settings->value("PrevJuz", "Shift+J"));
      settings->setValue("BookmarkCurrent",
                         settings->value("BookmarkCurrent", "Ctrl+Shift+B"));
      settings->setValue("BookmarksDialog",
                         settings->value("BookmarksDialog", "Ctrl+B"));
      settings->setValue("SearchDialog",
                         settings->value("SearchDialog", "Ctrl+F"));
      settings->setValue("SettingsDialog",
                         settings->value("SettingsDialog", "Ctrl+P"));
      settings->setValue("TafsirDialog",
                         settings->value("TafsirDialog", "Ctrl+T"));
      settings->setValue("DownloaderDialog",
                         settings->value("DownloaderDialog", "Ctrl+D"));
      settings->setValue("Quit", settings->value("Quit", "Ctrl+Q"));
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

  QFontDatabase::addApplicationFont(fontsDir.filePath("PakTypeNaskhBasic.ttf"));
  QFontDatabase::addApplicationFont(fontsDir.filePath("ExpoArabic.ttf"));
  QFontDatabase::addApplicationFont(fontsDir.filePath("noto-symbols.ttf"));
  QFontDatabase::addApplicationFont(fontsDir.filePath("noto-display.ttf"));

  switch (qcfVersion) {
    case 1:
      fontsDir.cd("QCFV1");
      qcfFontPrefix = "QCF_P";
      qcfBSMLFont = "QCF_BSML";
      QFontDatabase::addApplicationFont(fontsDir.filePath("QCF_BSML.ttf"));
      break;

    case 2:
      fontsDir.cd("QCFV2");
      qcfFontPrefix = "QCF2";
      qcfBSMLFont = "QCF2BSML";
      QFontDatabase::addApplicationFont(fontsDir.filePath("QCF2BSML.ttf"));
      break;
  }

  // add required fonts
  for (int i = 1; i < 605; i++) {
    QString fontName = qcfFontPrefix;
    fontName.append(QString::number(i).rightJustified(3, '0'));
    fontName.append(".ttf");

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
  QFile styles;
  switch (themeIdx) {
    case 0:
      // set global theme icons & styles path
      themeResources.setPath(":/resources/light/");
      darkMode = false;
      // light palette
      themeColors.setColor(QPalette::Window, QColor(240, 240, 240));
      themeColors.setColor(QPalette::WindowText, Qt::black);
      themeColors.setColor(
        QPalette::Disabled, QPalette::WindowText, QColor(120, 120, 120));
      themeColors.setColor(QPalette::Base, QColor(255, 255, 255));
      themeColors.setColor(QPalette::AlternateBase, QColor(233, 231, 227));
      themeColors.setColor(QPalette::ToolTipBase, Qt::white);
      themeColors.setColor(QPalette::ToolTipText, Qt::black);
      themeColors.setColor(QPalette::Text, Qt::black);
      themeColors.setColor(
        QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));
      themeColors.setColor(QPalette::Dark, QColor(160, 160, 160));
      themeColors.setColor(QPalette::Shadow, QColor(105, 105, 105));
      themeColors.setColor(QPalette::Button, QColor(240, 240, 240));
      themeColors.setColor(QPalette::ButtonText, Qt::black);
      themeColors.setColor(
        QPalette::Disabled, QPalette::ButtonText, QColor(120, 120, 120));
      themeColors.setColor(QPalette::BrightText, Qt::red);
      themeColors.setColor(QPalette::Link, QColor(0, 0, 255));
      themeColors.setColor(QPalette::Highlight, QColor(0, 120, 215));
      themeColors.setColor(
        QPalette::Disabled, QPalette::Highlight, QColor(0, 120, 215));
      themeColors.setColor(QPalette::HighlightedText, Qt::black);
      themeColors.setColor(
        QPalette::Disabled, QPalette::HighlightedText, QColor(255, 255, 255));
      qApp->setPalette(themeColors);
      break;

    case 1:
      // set global theme icons & styles path
      themeResources.setPath(":/resources/dark/");
      darkMode = true;
      // dark palette
      themeColors.setColor(QPalette::Window, QColor(53, 53, 53));
      themeColors.setColor(QPalette::WindowText, Qt::white);
      themeColors.setColor(
        QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
      themeColors.setColor(QPalette::Base, QColor(42, 42, 42));
      themeColors.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
      themeColors.setColor(QPalette::ToolTipBase, QColor(22, 22, 22));
      themeColors.setColor(QPalette::ToolTipText, Qt::white);
      themeColors.setColor(QPalette::Text, Qt::white);
      themeColors.setColor(
        QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
      themeColors.setColor(QPalette::Dark, QColor(35, 35, 35));
      themeColors.setColor(QPalette::Shadow, QColor(20, 20, 20));
      themeColors.setColor(QPalette::Button, QColor(53, 53, 53));
      themeColors.setColor(QPalette::ButtonText, Qt::white);
      themeColors.setColor(
        QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
      themeColors.setColor(QPalette::BrightText, Qt::red);
      themeColors.setColor(QPalette::Link, QColor(42, 130, 218));
      themeColors.setColor(QPalette::Highlight, QColor(42, 130, 218));
      themeColors.setColor(
        QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
      themeColors.setColor(QPalette::HighlightedText, Qt::white);
      themeColors.setColor(
        QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
      qApp->setPalette(themeColors);
      break;
  }

  // load stylesheet
  styles.setFileName(themeResources.filePath("styles.qss"));
  if (styles.open(QIODevice::ReadOnly)) {
    qApp->setStyleSheet(styles.readAll());
    styles.close();
  }
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
    qInfo() << "tr" << translation->language() << "loaded";
    qInfo() << "base tr loaded:" << qtBase->load(":/base/" + code + ".qm");
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
  Reciter husary{ "Al-Husary",
                  qApp->translate("MainWindow", "Al-Husary"),
                  bismillahDir.absoluteFilePath("husary.mp3"),
                  "https://cdn.islamic.network/quran/audio/64/ar.husary/",
                  true };

  Reciter husaryQasr{
    "Al-Husary_(Qasr)",
    qApp->translate("MainWindow", "Al-Husary (Qasr)"),
    husary.basmallahPath,
    "https://gitlab.com/0xzer0x/qc-audio/-/raw/main/husary_qasr_64kbps/"
  };

  Reciter husaryMujawwad{
    "Al-Husary_(Mujawwad)",
    qApp->translate("MainWindow", "Al-Husary (Mujawwad)"),
    husary.basmallahPath,
    "https://cdn.islamic.network/quran/audio/64/ar.husarymujawwad/",
    true
  };

  Reciter abdulbasit{
    "Abdul-Basit",
    qApp->translate("MainWindow", "Abdul-Basit"),
    bismillahDir.filePath("abdul-basit.mp3"),
    "https://cdn.islamic.network/quran/audio/64/ar.abdulbasitmurattal/",
    true
  };

  Reciter abdulbaitMujawwad{
    "Abdul-Basit_(Mujawwad)",
    qApp->translate("MainWindow", "Abdul-Basit (Mujawwad)"),
    abdulbasit.basmallahPath,
    "https://cdn.islamic.network/quran/audio/64/ar.abdulsamad/",
    true
  };

  Reciter menshawi{ "Menshawi",
                    qApp->translate("MainWindow", "Menshawi"),
                    bismillahDir.absoluteFilePath("menshawi.mp3"),
                    "https://cdn.islamic.network/quran/audio/128/ar.minshawi/",
                    true };

  Reciter menshawiMujawwad{
    "Menshawi_(Mujawwad)",
    qApp->translate("MainWindow", "Menshawi (Mujawwad)"),
    menshawi.basmallahPath,
    "https://cdn.islamic.network/quran/audio/64/ar.minshawimujawwad/",
    true
  };

  Reciter alafasy{ "Mishary_Alafasy",
                   qApp->translate("MainWindow", "Mishary Alafasy"),
                   bismillahDir.absoluteFilePath("alafasy.mp3"),
                   "https://cdn.islamic.network/quran/audio/64/ar.alafasy/",
                   true };

  Reciter tunaiji{ "Khalefa_Al-Tunaiji",
                   qApp->translate("MainWindow", "Khalefa Al-Tunaiji"),
                   bismillahDir.absoluteFilePath("tunaiji.mp3"),
                   "https://everyayah.com/data/khalefa_al_tunaiji_64kbps/" };

  Reciter dussary{ "Yasser_Ad-Dussary",
                   qApp->translate("MainWindow", "Yasser Ad-Dussary"),
                   bismillahDir.absoluteFilePath("ad-dussary.mp3"),
                   "https://everyayah.com/data/Yasser_Ad-Dussary_128kbps/" };

  Reciter banna{ "Mahmoud_Al-Banna",
                 qApp->translate("MainWindow", "Mahmoud Al-Banna"),
                 bismillahDir.absoluteFilePath("al-banna.mp3"),
                 "https://everyayah.com/data/mahmoud_ali_al_banna_32kbps/" };

  Reciter basfar{
    "Abdullah_Basfar",
    qApp->translate("MainWindow", "Abdullah Basfar"),
    bismillahDir.absoluteFilePath("basfar.mp3"),
    "https://cdn.islamic.network/quran/audio/64/ar.abdullahbasfar/",
    true
  };

  Reciter shatree{ "Ash-Shaatree",
                   qApp->translate("MainWindow", "Abu Bakr Ash-Shaatree"),
                   bismillahDir.absoluteFilePath("shatree.mp3"),
                   "https://cdn.islamic.network/quran/audio/64/ar.shaatree/",
                   true };

  Reciter ajamy{ "Al-Ajamy",
                 qApp->translate("MainWindow", "Ahmed Al-Ajamy"),
                 bismillahDir.absoluteFilePath("ajamy.mp3"),
                 "https://cdn.islamic.network/quran/audio/64/ar.ahmedajamy/",
                 true };

  Reciter aliJaber{ "Ali_Jaber",
                    qApp->translate("MainWindow", "Ali Jaber"),
                    bismillahDir.absoluteFilePath("ajaber.mp3"),
                    "https://everyayah.com/data/Ali_Jaber_64kbps/" };

  Reciter fAbbad{ "Fares_Abbad",
                  qApp->translate("MainWindow", "Fares Abbad"),
                  bismillahDir.absoluteFilePath("fabbad.mp3"),
                  "https://everyayah.com/data/Fares_Abbad_64kbps/" };

  Reciter ghamadi{ "Ghamadi",
                   qApp->translate("MainWindow", "Saad Al-Ghamadi"),
                   bismillahDir.absoluteFilePath("ghamadi.mp3"),
                   "https://everyayah.com/data/Ghamadi_40kbps/" };

  Reciter hRifai{ "Hani_Rifai",
                  qApp->translate("MainWindow", "Hani Rifai"),
                  bismillahDir.absoluteFilePath("rifai.mp3"),
                  "https://cdn.islamic.network/quran/audio/64/ar.hanirifai/",
                  true };

  Reciter hudhaify{ "Hudhaify",
                    qApp->translate("MainWindow", "Hudhaify"),
                    bismillahDir.absoluteFilePath("hudhaify.mp3"),
                    "https://cdn.islamic.network/quran/audio/64/ar.hudhaify/",
                    true };

  Reciter shuraym{
    "Saood_Ash-Shuraym",
    qApp->translate("MainWindow", "Saood Ash-Shuraym"),
    bismillahDir.absoluteFilePath("shuraym.mp3"),
    "https://cdn.islamic.network/quran/audio/64/ar.saoodshuraym/",
    true
  };

  Reciter alqatami{ "Nasser_Alqatami",
                    qApp->translate("MainWindow", "Nasser Alqatami"),
                    bismillahDir.absoluteFilePath("qatami.mp3"),
                    "https://everyayah.com/data/Nasser_Alqatami_128kbps/" };

  Reciter muaiqly{
    "Maher_AlMuaiqly",
    qApp->translate("MainWindow", "Maher Al-Muaiqly"),
    bismillahDir.absoluteFilePath("muaiqly.mp3"),
    "https://cdn.islamic.network/quran/audio/64/ar.mahermuaiqly/",
    true
  };

  Reciter mIsmail{
    "Mostafa_Ismail",
    qApp->translate("MainWindow", "Mostafa Ismail"),
    bismillahDir.absoluteFilePath("mismail.mp3"),
    "https://quran.ksu.edu.sa/ayat/mp3/Mostafa_Ismail_128kbps/"
  };

  Reciter mJibreel{
    "Muhammad_Jibreel",
    qApp->translate("MainWindow", "Muhammad Jibreel"),
    bismillahDir.absoluteFilePath("mjibreel.mp3"),
    "https://quran.ksu.edu.sa/ayat/mp3/Muhammad_Jibreel_64kbps/"
  };

  recitersList.append(husary);
  recitersList.append(husaryQasr);
  recitersList.append(husaryMujawwad);
  recitersList.append(abdulbasit);
  recitersList.append(abdulbaitMujawwad);
  recitersList.append(menshawi);
  recitersList.append(menshawiMujawwad);
  recitersList.append(alafasy);
  recitersList.append(tunaiji);
  recitersList.append(dussary);
  recitersList.append(banna);
  recitersList.append(basfar);
  recitersList.append(shatree);
  recitersList.append(ajamy);
  recitersList.append(aliJaber);
  recitersList.append(fAbbad);
  recitersList.append(ghamadi);
  recitersList.append(hRifai);
  recitersList.append(hudhaify);
  recitersList.append(shuraym);
  recitersList.append(alqatami);
  recitersList.append(muaiqly);
  recitersList.append(mIsmail);
  recitersList.append(mJibreel);

  // create reciters directories
  recitationsDir.setPath(configDir.absolutePath());
  if (!recitationsDir.exists("recitations"))
    recitationsDir.mkdir("recitations");

  recitationsDir.cd("recitations");
  foreach (const Reciter& r, recitersList) {
    if (!recitationsDir.exists(r.baseDirName)) {
      recitationsDir.mkdir(r.baseDirName);
    }
  }
}

void
populateShortcutsMap()
{
  // shortcut descriptions
  shortcutDescription.insert(
    "ToggleMenubar",
    qApp->translate("SettingsDialog", "Toggle visibility of the menubar"));
  shortcutDescription.insert(
    "ToggleNavDock",
    qApp->translate("SettingsDialog",
                    "Toggle visibility of the navigation dock"));
  shortcutDescription.insert(
    "TogglePlayback",
    qApp->translate("SettingsDialog", "Toggle playback state of recitation"));
  shortcutDescription.insert(
    "VolumeUp",
    qApp->translate("SettingsDialog", "Increase the playback volume"));
  shortcutDescription.insert(
    "VolumeDown",
    qApp->translate("SettingsDialog", "Decrease the playback volume"));

  shortcutDescription.insert(
    "NextPage", qApp->translate("SettingsDialog", "Move to the next page"));
  shortcutDescription.insert(
    "PrevPage", qApp->translate("SettingsDialog", "Move to the previous page"));

  shortcutDescription.insert(
    "NextVerse", qApp->translate("SettingsDialog", "Move to the next verse"));
  shortcutDescription.insert(
    "PrevVerse",
    qApp->translate("SettingsDialog", "Move to the previous verse"));

  shortcutDescription.insert(
    "NextSurah", qApp->translate("SettingsDialog", "Move to the next surah"));
  shortcutDescription.insert(
    "PrevSurah",
    qApp->translate("SettingsDialog", "Move to the previous surah"));

  shortcutDescription.insert(
    "NextJuz", qApp->translate("SettingsDialog", "Move to the next juz"));
  shortcutDescription.insert(
    "PrevJuz", qApp->translate("SettingsDialog", "Move to the previous juz"));

  shortcutDescription.insert(
    "BookmarkCurrent",
    qApp->translate("SettingsDialog", "Bookmark the current active verse"));

  shortcutDescription.insert(
    "BookmarksDialog",
    qApp->translate("SettingsDialog", "Open the bookmarks dialog"));
  shortcutDescription.insert(
    "SearchDialog",
    qApp->translate("SettingsDialog", "Open the search dialog"));
  shortcutDescription.insert(
    "SettingsDialog",
    qApp->translate("SettingsDialog", "Open the preferences dialog"));
  shortcutDescription.insert(
    "TafsirDialog",
    qApp->translate("SettingsDialog",
                    "Open the tafsir for the current active verse"));
  shortcutDescription.insert(
    "DownloaderDialog",
    qApp->translate("SettingsDialog", "Open the recitations download dialog"));

  shortcutDescription.insert("Quit", qApp->translate("SettingsDialog", "Quit"));
}
