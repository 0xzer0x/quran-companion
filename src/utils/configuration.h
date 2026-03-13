#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QFont>
#include <QKeySequence>
#include <QLocale>
#include <QSettings>
#include <QSharedPointer>
#include <types/reciter.h>
#include <types/tafsir.h>
#include <types/translation.h>
#include <utils/configurationschema.h>

class Configuration
{
public:
  static Configuration& getInstance();
  void loadUiTranslation();
  void checkGroups();

  QSettings& settings();
  const int themeId() const;
  const bool darkMode() const;
  const int qcfVersion() const;
  const int qcfFontSize() const;
  const int verseFontSize() const;
  const bool menuBarVisible() const;
  const bool foregroundHighlighting() const;
  const int khatmahId() const;
  const bool adaptiveReaderFont() const;
  const bool missingFileWarning() const;
  const bool showVerseOfTheDay() const;
  const Reciter reciter() const;
  const Tafsir tafsir() const;
  const Translation translation() const;
  const QByteArray windowState() const;
  const QKeySequence shortcutKeySequence(const QString name) const;
  const QFont sideContentFont() const;
  const QLocale::Language language() const;
  const ConfigurationSchema::ReaderMode readerMode() const;
  const ConfigurationSchema::VerseType verseType() const;
  void setVerseType(ConfigurationSchema::VerseType newVerseType);
  void setReciter(const Reciter reciter);
  void setTafsir(const Tafsir tafsir);
  void setTranslation(Translation translation);
  void setWindowState(const QByteArray windowState);
  void setMenuBarVisible(const bool visible);
  void setKhatmahId(const int id);
  void setAdaptiveReaderFont(const bool enabled);
  void setShortcutIfNotExists(const QString name, const QString value);
  void setQcfVersion(const int version);
  void setQcfFontSize(const int size);
  void setForegroundHighlighting(const bool enabled);
  void setSideContentFont(const QFont font);
  void setVerseFontSize(const int size);
  void setTheme(const int themeId);
  void setLanguage(const QLocale::Language lang);
  void setDownloadsDir(const QString path);
  void setShowVerseOfTheDay(const bool enabled);
  void setMissingFileWarning(const bool enabled);
  void setReaderMode(const ConfigurationSchema::ReaderMode mode);
  void setShortcutKeySequence(const QString name, const QKeySequence sequence);
  void sync();

private:
  Configuration();
  void setCustomPaths();
  int m_themeId;
  bool m_darkMode;
  int m_qcfVersion;
  QLocale::Language m_language;
  QSettings m_settings;
  ConfigurationSchema::ReaderMode m_readerMode;
};

#endif // CONFIGURATION_H
