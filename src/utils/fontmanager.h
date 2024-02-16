#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "settings.h"
#include <QString>

class FontManager
{
public:
  static QString qcfFontPrefix;
  static QString pageFontname(int page);
  static QString verseFontname(Settings::VerseType type, int page);
  static void loadQcf();
  static void loadUiFonts();
  /**
   * @brief check if QCF2 font files exist
   *
   * @return true - all 604 QCF2 files are found
   * @return false - one of the files is missing
   */
  static bool qcfExists();
};

#endif // FONTMANAGER_H
