#include "configuration.h"
#include "dirmanager.h"
#include <QApplication>
#include <QFont>
#include <QLocale>
#include <QTranslator>

Configuration&
Configuration::getInstance()
{
  static Configuration config;
  return config;
}

Configuration::Configuration()
  : m_settings(DirManager::getInstance().configDir().absoluteFilePath(
                 "qurancompanion.conf"),
               QSettings::IniFormat)
{
  checkGroups();
  m_themeId = m_settings.value("Theme").toInt();
  m_qcfVersion = m_settings.value("Reader/QCF").toInt();
  m_language = qvariant_cast<QLocale::Language>(m_settings.value("Language"));
  m_readerMode = qvariant_cast<ReaderMode>(m_settings.value("Reader/Mode"));
  m_darkMode = m_themeId == 2;
}

void
Configuration::checkGroups()
{
  for (int i = 0; i < 2; i++)
    checkConfGroup(i);
}

void
Configuration::checkConfGroup(int gId)
{
  switch (gId) {
    case 0:
      m_settings.setValue("Language",
                          m_settings.value("Language", (int)QLocale::English));
      m_settings.setValue("Theme", m_settings.value("Theme", 0));
      m_settings.setValue("VOTD", m_settings.value("VOTD", true));
      m_settings.setValue("MissingFileWarning",
                          m_settings.value("MissingFileWarning", true));
      break;
    case 1:
      m_settings.beginGroup("Reader");
      m_settings.setValue("Mode", m_settings.value("Mode", 0));
      m_settings.setValue("FGHighlight", m_settings.value("FGHighlight", 1));
      m_settings.setValue("Khatmah", m_settings.value("Khatmah", 0));
      m_settings.setValue("AdaptiveFont",
                          m_settings.value("AdaptiveFont", true));
      m_settings.setValue("QCF1Size", m_settings.value("QCF1Size", 22));
      m_settings.setValue("QCF2Size", m_settings.value("QCF2Size", 20));
      m_settings.setValue("QCF", m_settings.value("QCF", 1));
      m_settings.setValue("VerseType", m_settings.value("VerseType", 0));
      m_settings.setValue("VerseFontSize",
                          m_settings.value("VerseFontSize", 20));
      m_settings.setValue("Tafsir", m_settings.value("Tafsir", 6));
      m_settings.setValue("Translation", m_settings.value("Translation", 5));
      m_settings.setValue(
        "SideContentFont",
        m_settings.value("SideContentFont", QFont("Expo Arabic", 14)));
      m_settings.endGroup();
      break;
  }
}

void
Configuration::loadUiTranslation()
{
  if (m_language == QLocale::English)
    return;

  QString code = QLocale::languageToCode(m_language);
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

QSettings&
Configuration::settings()
{
  return m_settings;
}

int
Configuration::themeId() const
{
  return m_themeId;
}

bool
Configuration::darkMode() const
{
  return m_darkMode;
}

int
Configuration::qcfVersion() const
{
  return m_qcfVersion;
}

QLocale::Language
Configuration::language() const
{
  return m_language;
}

Configuration::ReaderMode
Configuration::readerMode() const
{
  return m_readerMode;
}

Configuration::VerseType
Configuration::verseType() const
{
  return m_verseType;
}

void
Configuration::setVerseType(VerseType newVerseType)
{
  m_verseType = newVerseType;
}
