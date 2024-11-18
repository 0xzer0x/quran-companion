#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QLocale>
#include <QSettings>
#include <QSharedPointer>
#include <utils/configurationschema.h>

class Configuration
{
public:
  static Configuration& getInstance();
  void loadUiTranslation();
  void checkGroups();

  QSettings& settings();
  int themeId() const;
  bool darkMode() const;
  int qcfVersion() const;
  QLocale::Language language() const;
  ConfigurationSchema::ReaderMode readerMode() const;
  ConfigurationSchema::VerseType verseType() const;
  void setVerseType(ConfigurationSchema::VerseType newVerseType);

private:
  Configuration();
  void setCustomPaths();
  int m_themeId;
  bool m_darkMode;
  int m_qcfVersion;
  QLocale::Language m_language;
  QSettings m_settings;
  ConfigurationSchema::ReaderMode m_readerMode;
  ConfigurationSchema::VerseType m_verseType;
};

#endif // CONFIGURATION_H
