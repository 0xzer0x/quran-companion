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

namespace {
// resource directory and file name (without extension) of each theme, indexed by theme id
const QList<QPair<QString, QString>> THEMES = {
  { ":/resources/light/", "light" },
  { ":/resources/light/", "sepia" },
  { ":/resources/dark/", "dark" },
};
}

void
StyleManager::loadTheme()
{
  qApp->setStyle(QStyleFactory::create("Fusion"));

  const auto [dir, name] = THEMES.value(m_config.themeId());
  m_themeResources.setPath(dir);
  qApp->setPalette(themePalette(m_config.themeId()));

  // load stylesheet
  QFile styles(m_themeResources.filePath(name + ".qss"));
  if (styles.open(QIODevice::ReadOnly)) {
    qApp->setStyleSheet(styles.readAll());
    styles.close();
  }

  m_awesome = new fa::QtAwesome(qApp);
  m_awesome->initFontAwesome();
}

QPalette
StyleManager::themePalette(int themeId)
{
  const auto [dir, name] = THEMES.value(themeId);
  QPalette themeColors;
  QFile palette(QDir(dir).filePath(name + ".xml"));
  if (!palette.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qCritical("Failed to read color palette XML, file: %s", qPrintable(palette.fileName()));
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
        role = static_cast<QPalette::ColorRole>(paletteReader.attributes().value("role").toInt());
        red = paletteReader.attributes().value("red").toInt();
        green = paletteReader.attributes().value("green").toInt();
        blue = paletteReader.attributes().value("blue").toInt();

        themeColors.setColor(group, role, QColor(red, green, blue));
      }
    }
  }

  palette.close();
  return themeColors;
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
