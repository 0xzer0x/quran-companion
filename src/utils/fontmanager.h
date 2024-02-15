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
};

#endif // FONTMANAGER_H
