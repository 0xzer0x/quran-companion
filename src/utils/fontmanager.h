#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include "configuration.h"
#include "dirmanager.h"
#include <QString>
#include <utils/configurationschema.h>

class FontManager
{
public:
  static FontManager& getInstance();
  QString pageFontname(int page);
  QString verseFontname(ConfigurationSchema::VerseType type, int page);
  void loadFonts();
  bool qcfExists();

private:
  FontManager();
  void loadQcf();
  void loadUiFonts();
  Configuration& m_config;
  DirManager& m_dirMgr;
  QString m_qcfFontPrefix;
};

#endif // FONTMANAGER_H
