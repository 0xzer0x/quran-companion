#include "configuration.h"
#include <QApplication>
#include <QFont>
#include <QLocale>
#include <QStyleHints>
#include <QTranslator>
#include <utils/dirmanager.h>

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
  setCustomPaths();
  m_themeId = m_settings.value("Theme").toInt();
  m_qcfVersion = m_settings.value("Reader/QCF").toInt();
  m_language = qvariant_cast<QLocale::Language>(m_settings.value("Language"));
  m_readerMode = qvariant_cast<ConfigurationSchema::ReaderMode>(
    m_settings.value("Reader/Mode"));
  m_darkMode = m_themeId == 2;
}

void
Configuration::setCustomPaths()
{
  QString path = m_settings.value("DownloadsDir").toString();
  if (!path.isEmpty()) {
    QDir dir = QDir(path);
    DirManager::getInstance().setDownloadsDir(dir);
  }
  DirManager::getInstance().createDirSkeleton();
}

void
Configuration::checkGroups()
{
  const ConfigurationSchema& schema = ConfigurationSchema::getInstance();
  QList<QString> allKeys = schema.getKeys().value();
  for (const QString& fullKey : allKeys) {
    QString settingsKey = fullKey;
    if (settingsKey.startsWith("General/")) {
      settingsKey.remove(0, 8);
    }
    m_settings.setValue(
      settingsKey,
      m_settings.value(settingsKey, schema.getDefault(fullKey).value()));
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
    qInfo() << "base translation:" << qtBase->load(":/qtbase/" + code + ".qm");
    qApp->installTranslator(translation);
    qApp->installTranslator(qtBase);
  } else {
    qWarning() << code + " translation not loaded!";
    delete translation;
    delete qtBase;
  }

  if (m_language == QLocale::Arabic) {
    qApp->setLayoutDirection(Qt::RightToLeft);
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

ConfigurationSchema::ReaderMode
Configuration::readerMode() const
{
  return m_readerMode;
}

ConfigurationSchema::VerseType
Configuration::verseType() const
{
  return m_verseType;
}

const Reciter&
Configuration::reciter() const
{
  const QString reciterId = m_settings.value("Reciter").toString();
  // NOTE: Get reciter with the configured id and fallback if ID is not valid
  const Reciter* reciterPtr = Reciter::reciterById(reciterId);
  if (reciterPtr == nullptr) {
    reciterPtr = Reciter::reciterById("husary");
  }

  return *reciterPtr;
}

void
Configuration::setReciter(const Reciter* reciter)
{
  if (!reciter) {
    qWarning()
      << "Invalid reciter pointer value passed to configuration: nullptr";
    return;
  }

  m_settings.setValue("Reciter", reciter->id());
}

void
Configuration::setVerseType(ConfigurationSchema::VerseType newVerseType)
{
  m_verseType = newVerseType;
}
