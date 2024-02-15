#include "fontmanager.h"
#include "dirmanager.h"
#include <QApplication>
#include <QFontDatabase>

QString FontManager::qcfFontPrefix = "QCF_P";

void
FontManager::loadQcf()
{
  QFontDatabase::addApplicationFont(
    DirManager::fontsDir->filePath("QCFV1/QCF_BSML.ttf"));
  switch (Settings::qcfVersion) {
    case 1:
      DirManager::fontsDir->cd("QCFV1");
      break;
    case 2:
      DirManager::fontsDir->setPath(DirManager::downloadsDir->absolutePath() +
                                    "/QCFV2");
      qcfFontPrefix = "QCF2";
      break;
  }

  // add required fonts
  for (int i = 1; i < 605; i++) {
    QString fontName = pageFontname(i) + ".ttf";

    if (Settings::qcfVersion == 2 && !DirManager::fontsDir->exists(fontName)) {
      Settings::settings->setValue("Reader/QCF", 1);
      Settings::settings->sync();
      qFatal() << DirManager::fontsDir->filePath(fontName)
               << " font file not found, fallback to QCF v1";
    } else
      QFontDatabase::addApplicationFont(
        DirManager::fontsDir->filePath(fontName));
  }
}

void
FontManager::loadUiFonts()
{
  // ui fonts
  foreach (const QFileInfo& font,
           DirManager::fontsDir->entryInfoList(QDir::Files))
    QFontDatabase::addApplicationFont(font.absoluteFilePath());
  // set default UI fonts to use
  QStringList uiFonts;
  uiFonts << "Noto Sans Display"
          << "Expo Arabic";
  qApp->setFont(QFont(uiFonts, qApp->font().pointSize()));
}

QString
FontManager::pageFontname(int page)
{
  return qcfFontPrefix + QString::number(page).rightJustified(3, '0');
}

QString
FontManager::verseFontname(Settings::VerseType type, int page)
{
  QString fontname;
  switch (type) {
    case Settings::qcf:
      fontname = pageFontname(page);
      break;
    case Settings::uthmanic:
      fontname = "kfgqpc_hafs_uthmanic _script";
      break;
    case Settings::annotated:
      fontname = "Emine";
      break;
  }
  return fontname;
}
