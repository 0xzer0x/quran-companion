#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QLocale>
#include <QSettings>
#include <QSharedPointer>

class Configuration
{
public:
  enum VerseType
  {
    Qcf,
    Hafs,
    HafsAnnotated,
    Warsh
  };
  /**
   * @brief ReaderMode enum represents the available modes for the Quran reader
   * in MainWindow
   */
  enum ReaderMode
  {
    SinglePage, ///< Single Quran page, side panel is used for displaying verses
                ///< with translation
    DoublePage  ///< Two Quran pages, both panels are used to display Quran
                ///< pages, no translation
  };

  static Configuration& getInstance();
  void checkConfGroup(int gId);
  void loadUiTranslation();
  void checkGroups();

  QSettings& settings();
  int themeId() const;
  bool darkMode() const;
  int qcfVersion() const;
  QLocale::Language language() const;
  ReaderMode readerMode() const;
  VerseType verseType() const;
  void setVerseType(VerseType newVerseType);

private:
  Configuration();
  void setCustomPaths();
  int m_themeId;
  bool m_darkMode;
  int m_qcfVersion;
  QLocale::Language m_language;
  QSettings m_settings;
  ReaderMode m_readerMode;
  VerseType m_verseType;
};

#endif // CONFIGURATION_H
