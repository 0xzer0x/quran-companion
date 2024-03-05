#include "stylemanager.h"
#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include <QXmlStreamReader>

StyleManager&
StyleManager::getInstance()
{
  static StyleManager stylemanager;
  return stylemanager;
}

StyleManager::StyleManager()
  : m_config(Configuration::getInstance())
{
}

void
StyleManager::loadTheme()
{
  qApp->setStyle(QStyleFactory::create("Fusion"));

  QPalette themeColors;
  QFile styles, palette;
  switch (m_config.themeId()) {
    case 0:
      m_themeResources.setPath(":/resources/light/");
      styles.setFileName(m_themeResources.filePath("light.qss"));
      palette.setFileName(m_themeResources.filePath("light.xml"));
      break;

    case 1:
      m_themeResources.setPath(":/resources/light/");
      styles.setFileName(m_themeResources.filePath("sepia.qss"));
      palette.setFileName(m_themeResources.filePath("sepia.xml"));
      break;

    case 2:
      m_themeResources.setPath(":/resources/dark/");
      styles.setFileName(m_themeResources.filePath("dark.qss"));
      palette.setFileName(m_themeResources.filePath("dark.xml"));
      break;
  }

  if (!palette.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qCritical() << "Couldn't Read Color palette xml";
  }

  QXmlStreamReader paletteReader(&palette);
  QPalette::ColorGroup group = QPalette::All;
  while (!paletteReader.atEnd() && !paletteReader.hasError()) {
    QXmlStreamReader::TokenType token = paletteReader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (paletteReader.name().toString() == "enabled")
        group = QPalette::All;
      else if (paletteReader.name().toString() == "disabled")
        group = QPalette::Disabled;
      // color element
      else {
        QPalette::ColorRole role;
        int red, green, blue;
        role = static_cast<QPalette::ColorRole>(
          paletteReader.attributes().value("role").toInt());
        red = paletteReader.attributes().value("red").toInt();
        green = paletteReader.attributes().value("green").toInt();
        blue = paletteReader.attributes().value("blue").toInt();

        themeColors.setColor(group, role, QColor(red, green, blue));
      }
    }
  }

  palette.close();
  qApp->setPalette(themeColors);

  // load stylesheet
  if (styles.open(QIODevice::ReadOnly)) {
    qApp->setStyleSheet(styles.readAll());
    styles.close();
  }

  m_awesome = new fa::QtAwesome(qApp);
  m_awesome->initFontAwesome();
}

fa::QtAwesome&
StyleManager::awesome()
{
  return *m_awesome;
}

const QDir&
StyleManager::themeResources() const
{
  return m_themeResources;
}
