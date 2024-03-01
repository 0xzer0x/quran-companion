#include "fontmanager.h"
#include "configuration.h"
#include <QApplication>
#include <QFontDatabase>

FontManager&
FontManager::getInstance()
{
  static FontManager fontmanager;
  return fontmanager;
}

FontManager::FontManager()
  : m_dirMgr(DirManager::getInstance())
  , m_config(Configuration::getInstance())
{
}

void
FontManager::loadFonts()
{
  loadUiFonts();
  loadQcf();
}

void
FontManager::loadQcf()
{
  QFontDatabase::addApplicationFont(
    m_dirMgr.fontsDir().filePath("QCFV1/QCF_BSML.ttf"));
  switch (m_config.qcfVersion()) {
    case 1:
      m_dirMgr.setFontsDir(m_dirMgr.fontsDir().absoluteFilePath("QCFV1"));
      m_qcfFontPrefix = "QCF_P";
      break;
    case 2:
      m_dirMgr.setFontsDir(m_dirMgr.downloadsDir().absolutePath() + "/QCFV2");
      m_qcfFontPrefix = "QCF2";
      break;
  }

  // add required fonts
  for (int i = 1; i < 605; i++) {
    QString fontName = pageFontname(i) + ".ttf";

    if (m_config.qcfVersion() == 2 && !m_dirMgr.fontsDir().exists(fontName)) {
      m_config.settings().setValue("Reader/QCF", 1);
      m_config.settings().sync();
      qFatal() << m_dirMgr.fontsDir().filePath(fontName)
               << " font file not found, fallback to QCF v1";
    } else
      QFontDatabase::addApplicationFont(m_dirMgr.fontsDir().filePath(fontName));
  }
}

void
FontManager::loadUiFonts()
{
  // ui fonts
  foreach (const QFileInfo& font,
           m_dirMgr.fontsDir().entryInfoList(QDir::Files))
    QFontDatabase::addApplicationFont(font.absoluteFilePath());
  // set default UI fonts to use
  QStringList uiFonts;
  uiFonts << "Noto Sans Display"
          << "Expo Arabic";
  qApp->setFont(QFont(uiFonts, qApp->font().pointSize()));
}

bool
FontManager::qcfExists()
{
  QString filename = "QCFV2/QCF2%0.ttf";
  for (int i = 1; i <= 604; i++) {
    if (!m_dirMgr.downloadsDir().exists(
          filename.arg(QString::number(i).rightJustified(3, '0'))))
      return false;
  }

  return true;
}

QString
FontManager::pageFontname(int page)
{
  return m_qcfFontPrefix + QString::number(page).rightJustified(3, '0');
}

QString
FontManager::verseFontname(Configuration::VerseType type, int page)
{
  QString fontname;
  switch (type) {
    case Configuration::Qcf:
      fontname = pageFontname(page);
      break;
    case Configuration::Uthmanic:
      fontname = "kfgqpc_hafs_uthmanic _script";
      break;
    case Configuration::Annotated:
      fontname = "Emine";
      break;
  }
  return fontname;
}
