#include "core/mainwindow.h"
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
checkSettings(QSettings& settings);

int
main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  QApplication::setApplicationName("Quran Companion");
  QApplication::setOrganizationName("0xzer0x");
  QApplication::setApplicationVersion("1.1");

  QSplashScreen splash(QPixmap(":/resources/splash.png"));
  splash.show();

  QDir::setCurrent(QApplication::applicationDirPath());
  Logger::startLogger(QDir::currentPath());
  Logger::attach();

  QSettings appSettings("qc-config.ini", QSettings::Format::IniFormat, &a);
  checkSettings(appSettings);

  setTheme(appSettings.value("Theme").toInt());
  addFonts(appSettings.value("Reader/QCF").toInt());
  addTranslation((QLocale::Language)appSettings.value("Language").toInt());

  MainWindow w(nullptr, &appSettings);
  splash.finish(&w);
  w.show();

  int ret = a.exec();
  Logger::stopLogger();
  return ret;
}

void
checkSettings(QSettings& settings)
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

  if (settings.allKeys() != defaultKeys) {
    settings.setValue("Language",
                      settings.value("Language", (int)QLocale::English));
    settings.setValue("Theme", settings.value("Theme", 0));
    settings.setValue("VOTD", settings.value("VOTD", true));
    settings.setValue("MissingFileWarning",
                      settings.value("MissingFileWarning", true));

    settings.beginGroup("Reader");
    // all keys now have "Reader" prefix
    settings.setValue("Page", settings.value("Page", 1));
    settings.setValue("Surah", settings.value("Surah", 1));
    settings.setValue("Verse", settings.value("Verse", 1));
    settings.setValue("AdaptiveFont", settings.value("AdaptiveFont", true));
    settings.setValue("QCF1Size", settings.value("QCF1Size", 22));
    settings.setValue("QCF2Size", settings.value("QCF2Size", 20));
    settings.setValue("QCF", settings.value("QCF", 1));
    settings.setValue("Tafsir", settings.value("Tafsir", 1));
    settings.setValue("Translation", settings.value("Translation", 5));
    settings.setValue(
      "SideContentFont",
      settings.value("SideContentFont", QFont("Droid Sans Arabic", 14)));

    settings.endGroup();
  }
}

void
addFonts(int qcfVersion)
{
  QDir fontsDir;
  QString fontBase;
  fontsDir = QApplication::applicationDirPath() + QDir::separator() + "assets" +
             QDir::separator() + "fonts";

  QFontDatabase::addApplicationFont(fontsDir.filePath("PakTypeNaskhBasic.ttf"));
  QFontDatabase::addApplicationFont(fontsDir.filePath("noto-symbols.ttf"));
  QFontDatabase::addApplicationFont(fontsDir.filePath("noto-display.ttf"));
  QFontDatabase::addApplicationFont(fontsDir.filePath("droid-sans-org.ttf"));

  switch (qcfVersion) {
    case 1:
      fontsDir.cd("QCFV1");
      fontBase = "QCF_P";
      QFontDatabase::addApplicationFont(fontsDir.filePath("QCF_BSML.ttf"));
      break;

    case 2:
      fontsDir.cd("QCFV2");
      fontBase = "QCF2";
      QFontDatabase::addApplicationFont(fontsDir.filePath("QCF2BSML.ttf"));
      break;
  }

  // add required fonts
  for (int i = 1; i < 605; i++) {
    QString fontName = fontBase;
    fontName.append(QString::number(i).rightJustified(3, '0'));
    fontName.append(".ttf");

    QFontDatabase::addApplicationFont(fontsDir.filePath(fontName));
  }

  // set default UI fonts to use
  QStringList uiFonts;
  uiFonts << "Noto Sans Display"
          << "Droid Sans Arabic";
  qApp->setFont(QFont(uiFonts, qApp->font().pointSize()));
}

void
setTheme(int themeIdx)
{
  qApp->setStyle(QStyleFactory::create("Fusion"));

  QPalette themeColors;
  QFile ss;
  switch (themeIdx) {
    case 0: // light
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
      // load light stylesheet
      ss.setFileName(":/resources/light/styles.qss");
      if (ss.open(QIODevice::ReadOnly)) {
        qApp->setStyleSheet(ss.readAll());
        ss.close();
      }
      break;

    case 1: // dark
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
      // load dark stylesheet
      ss.setFileName(":/resources/dark/styles.qss");
      if (ss.open(QIODevice::ReadOnly)) {
        qApp->setStyleSheet(ss.readAll());
        ss.close();
      }
      break;

    default:
      break;
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
