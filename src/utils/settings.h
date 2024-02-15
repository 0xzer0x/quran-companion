#ifndef SETTINGS_H
#define SETTINGS_H

#include <QLocale>
#include <QSettings>
#include <QSharedPointer>

class Settings
{
public:
  enum VerseType
  {
    qcf,
    uthmanic,
    annotated
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

  static int themeId;
  static bool darkMode;
  static int qcfVersion;
  static QLocale::Language language;
  static QSharedPointer<QSettings> settings;
  static ReaderMode readerMode;
  static void setup();
  static void checkGroups();
  static void checkConfGroup(int gId);
  static void loadQmFiles();
};

#endif // SETTINGS_H
