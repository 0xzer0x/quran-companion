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

void
setTheme(int themeIdx);
void
addFonts(int qcfVersion);
void
addTranslation(QLocale::Language localeCode);
void
checkSettings(QSettings* settings);
void
fillRecitersList();
void
setGlobals();

int
main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  QApplication::setApplicationName("Quran Companion");
  QApplication::setOrganizationName("0xzer0x");
  QApplication::setApplicationVersion("1.1.5");

  QSplashScreen splash(QPixmap(":/resources/splash.png"));
  splash.show();

  setGlobals();
  Logger::startLogger(QDir::currentPath());
  Logger::attach();

  g_settings = new QSettings(
    g_configDir.filePath("qurancompanion.conf"), QSettings::IniFormat, &a);
  checkSettings(g_settings);

  g_themeId = g_settings->value("Theme").toInt();
  g_qcfVersion = g_settings->value("Reader/QCF").toInt();
  g_language = qvariant_cast<QLocale::Language>(g_settings->value("Language"));
  setTheme(g_themeId);
  addFonts(g_qcfVersion);
  addTranslation(g_language);
  fillRecitersList();

  MainWindow w(nullptr);
  splash.finish(&w);
  w.show();

  int exitcode = a.exec();
  Logger::stopLogger();
  return exitcode;
}

void
checkSettings(QSettings* settings)
{
  QStringList defaultKeys;
  defaultKeys << "Language"
              << "MissingFileWarning"
              << "Reader/AdaptiveFont"
              << "Reader/Page"
              << "Reader/QCF"
              << "Reader/QCF1Size"
              << "Reader/QCF2Size"
              << "Reader/SideContentFont"
              << "Reader/Surah"
              << "Reader/Tafsir"
              << "Reader/Translation"
              << "Reader/Verse"
              << "Reciter"
              << "Theme"
              << "VOTD"
              << "WindowState";

  if (settings->allKeys() != defaultKeys) {
    settings->setValue("Language",
                       settings->value("Language", (int)QLocale::English));
    settings->setValue("Theme", settings->value("Theme", 0));
    settings->setValue("VOTD", settings->value("VOTD", true));
    settings->setValue("MissingFileWarning",
                       settings->value("MissingFileWarning", true));

    settings->beginGroup("Reader");
    // all keys now have "Reader" prefix
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
      g_qcfFontPrefix = "QCF_P";
      g_qcfBSMLFont = "QCF_BSML";
      QFontDatabase::addApplicationFont(fontsDir.filePath("QCF_BSML.ttf"));
      break;

    case 2:
      fontsDir.cd("QCFV2");
      g_qcfFontPrefix = "QCF2";
      g_qcfBSMLFont = "QCF2BSML";
      QFontDatabase::addApplicationFont(fontsDir.filePath("QCF2BSML.ttf"));
      break;
  }

  // add required fonts
  for (int i = 1; i < 605; i++) {
    QString fontName = g_qcfFontPrefix;
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
      g_themeResources.setPath(":/resources/light/");
      g_darkMode = false;
      // light palette
      themeColors.setColor(QPalette::Window, QColor(240, 240, 240));
      themeColors.setColor(QPalette::WindowText, Qt::black);
      themeColors.setColor(
        QPalette::Disabled, QPalette::WindowText, QColor(120, 120, 120));
      themeColors.setColor(QPalette::Base, QColor(255, 255, 255));
      themeColors.setColor(QPalette::AlternateBase, QColor(233, 231, 227));
      themeColors.setColor(QPalette::ToolTipBase, Qt::black);
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
      g_themeResources.setPath(":/resources/dark/");
      g_darkMode = true;
      // dark palette
      themeColors.setColor(QPalette::Window, QColor(53, 53, 53));
      themeColors.setColor(QPalette::WindowText, Qt::white);
      themeColors.setColor(
        QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
      themeColors.setColor(QPalette::Base, QColor(42, 42, 42));
      themeColors.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
      themeColors.setColor(QPalette::ToolTipBase, Qt::white);
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
  styles.setFileName(g_themeResources.filePath("styles.qss"));
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
fillRecitersList()
{
  g_recitationsDir.setPath(QDir::currentPath());
  if (!g_recitationsDir.exists("recitations"))
    g_recitationsDir.mkdir("recitations");

  g_recitationsDir.cd("recitations");
  foreach (const Reciter& r, g_recitersList) {
    if (!g_recitationsDir.exists(r.baseDirName)) {
      g_recitationsDir.mkdir(r.baseDirName);
    }
  }

  Reciter husary{ "Al-Husary",
                  qApp->translate("MainWindow", "Al-Husary"),
                  g_bismillahDir.absoluteFilePath("husary.mp3"),
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
    g_bismillahDir.filePath("abdul-basit.mp3"),
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
                    g_bismillahDir.absoluteFilePath("menshawi.mp3"),
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
                   g_bismillahDir.absoluteFilePath("alafasy.mp3"),
                   "https://cdn.islamic.network/quran/audio/64/ar.alafasy/",
                   true };

  Reciter tunaiji{ "Khalefa_Al-Tunaiji",
                   qApp->translate("MainWindow", "Khalefa Al-Tunaiji"),
                   g_bismillahDir.absoluteFilePath("tunaiji.mp3"),
                   "https://everyayah.com/data/khalefa_al_tunaiji_64kbps/" };

  Reciter dussary{ "Yasser_Ad-Dussary",
                   qApp->translate("MainWindow", "Yasser Ad-Dussary"),
                   g_bismillahDir.absoluteFilePath("ad-dussary.mp3"),
                   "https://everyayah.com/data/Yasser_Ad-Dussary_128kbps/" };

  Reciter banna{ "Mahmoud_Al-Banna",
                 qApp->translate("MainWindow", "Mahmoud Al-Banna"),
                 g_bismillahDir.absoluteFilePath("al-banna.mp3"),
                 "https://everyayah.com/data/mahmoud_ali_al_banna_32kbps/" };

  Reciter basfar{
    "Abdullah_Basfar",
    qApp->translate("MainWindow", "Abdullah Basfar"),
    g_bismillahDir.absoluteFilePath("basfar.mp3"),
    "https://cdn.islamic.network/quran/audio/64/ar.abdullahbasfar/",
    true
  };

  Reciter shatree{ "Ash-Shaatree",
                   qApp->translate("MainWindow", "Abu Bakr Ash-Shaatree"),
                   g_bismillahDir.absoluteFilePath("shatree.mp3"),
                   "https://cdn.islamic.network/quran/audio/64/ar.shaatree/",
                   true };

  Reciter ajamy{ "Al-Ajamy",
                 qApp->translate("MainWindow", "Ahmed Al-Ajamy"),
                 g_bismillahDir.absoluteFilePath("ajamy.mp3"),
                 "https://cdn.islamic.network/quran/audio/64/ar.ahmedajamy/",
                 true };

  Reciter aliJaber{ "Ali_Jaber",
                    qApp->translate("MainWindow", "Ali Jaber"),
                    g_bismillahDir.absoluteFilePath("ajaber.mp3"),
                    "https://everyayah.com/data/Ali_Jaber_64kbps/" };

  Reciter fAbbad{ "Fares_Abbad",
                  qApp->translate("MainWindow", "Fares Abbad"),
                  g_bismillahDir.absoluteFilePath("fabbad.mp3"),
                  "https://everyayah.com/data/Fares_Abbad_64kbps/" };

  Reciter ghamadi{ "Ghamadi",
                   qApp->translate("MainWindow", "Saad Al-Ghamadi"),
                   g_bismillahDir.absoluteFilePath("ghamadi.mp3"),
                   "https://everyayah.com/data/Ghamadi_40kbps/" };

  Reciter hRifai{ "Hani_Rifai",
                  qApp->translate("MainWindow", "Hani Rifai"),
                  g_bismillahDir.absoluteFilePath("rifai.mp3"),
                  "https://cdn.islamic.network/quran/audio/64/ar.hanirifai/",
                  true };

  Reciter hudhaify{ "Hudhaify",
                    qApp->translate("MainWindow", "Hudhaify"),
                    g_bismillahDir.absoluteFilePath("hudhaify.mp3"),
                    "https://cdn.islamic.network/quran/audio/64/ar.hudhaify/",
                    true };

  Reciter shuraym{
    "Saood_Ash-Shuraym",
    qApp->translate("MainWindow", "Saood Ash-Shuraym"),
    g_bismillahDir.absoluteFilePath("shuraym.mp3"),
    "https://cdn.islamic.network/quran/audio/64/ar.saoodshuraym/",
    true
  };

  Reciter alqatami{ "Nasser_Alqatami",
                    qApp->translate("MainWindow", "Nasser Alqatami"),
                    g_bismillahDir.absoluteFilePath("qatami.mp3"),
                    "https://everyayah.com/data/Nasser_Alqatami_128kbps/" };

  Reciter muaiqly{
    "Maher_AlMuaiqly",
    qApp->translate("MainWindow", "Maher Al-Muaiqly"),
    g_bismillahDir.absoluteFilePath("muaiqly.mp3"),
    "https://cdn.islamic.network/quran/audio/64/ar.mahermuaiqly/",
    true
  };

  Reciter mIsmail{
    "Mostafa_Ismail",
    qApp->translate("MainWindow", "Mostafa Ismail"),
    g_bismillahDir.absoluteFilePath("mismail.mp3"),
    "https://quran.ksu.edu.sa/ayat/mp3/Mostafa_Ismail_128kbps/"
  };

  Reciter mJibreel{
    "Muhammad_Jibreel",
    qApp->translate("MainWindow", "Muhammad Jibreel"),
    g_bismillahDir.absoluteFilePath("mjibreel.mp3"),
    "https://quran.ksu.edu.sa/ayat/mp3/Muhammad_Jibreel_64kbps/"
  };

  g_recitersList.append(husary);
  g_recitersList.append(husaryQasr);
  g_recitersList.append(husaryMujawwad);
  g_recitersList.append(abdulbasit);
  g_recitersList.append(abdulbaitMujawwad);
  g_recitersList.append(menshawi);
  g_recitersList.append(menshawiMujawwad);
  g_recitersList.append(alafasy);
  g_recitersList.append(tunaiji);
  g_recitersList.append(dussary);
  g_recitersList.append(banna);
  g_recitersList.append(basfar);
  g_recitersList.append(shatree);
  g_recitersList.append(ajamy);
  g_recitersList.append(aliJaber);
  g_recitersList.append(fAbbad);
  g_recitersList.append(ghamadi);
  g_recitersList.append(hRifai);
  g_recitersList.append(hudhaify);
  g_recitersList.append(shuraym);
  g_recitersList.append(alqatami);
  g_recitersList.append(muaiqly);
  g_recitersList.append(mIsmail);
  g_recitersList.append(mJibreel);
}

void
setGlobals()
{
  // data
  g_assetsDir =
    QApplication::applicationDirPath() + QDir::separator() + "assets";
  g_bismillahDir =
    QApplication::applicationDirPath() + QDir::separator() + "bismillah";

  // config
  g_configDir.mkpath(".qurancompanion");
  g_configDir.cd(".qurancompanion");
  QDir::setCurrent(g_configDir.absolutePath());

  g_updateToolPath = QApplication::applicationDirPath() + QDir::separator() +
                     "QCMaintenanceTool";
#ifdef Q_OS_WIN
  m_updateToolPath.append(".exe");
#endif
}
