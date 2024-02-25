#include "settings.h"
#include <QApplication>
#include <QFont>
#include <QLocale>
#include <QTranslator>
#include <utils/dirmanager.h>

int Settings::themeId = 0;
bool Settings::darkMode = false;
int Settings::qcfVersion = 1;
Settings::ReaderMode Settings::readerMode;
QLocale::Language Settings::language;
QSharedPointer<QSettings> Settings::settings;

void
Settings::setup()
{
  settings = QSharedPointer<QSettings>::create(
    DirManager::configDir->filePath("qurancompanion.conf"),
    QSettings::IniFormat);

  themeId = settings->value("Theme").toInt();
  qcfVersion = settings->value("Reader/QCF").toInt();
  language = qvariant_cast<QLocale::Language>(settings->value("Language"));
  readerMode = qvariant_cast<ReaderMode>(settings->value("Reader/Mode"));

  checkGroups();
}

void
Settings::checkGroups()
{
  for (int i = 0; i < 2; i++)
    checkConfGroup(i);
}

void
Settings::checkConfGroup(int gId)
{
  switch (gId) {
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
Settings::loadQmFiles()
{
  if (language == QLocale::English)
    return;

  QString code = QLocale::languageToCode(language);
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
