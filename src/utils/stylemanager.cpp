#include "stylemanager.h"
#include "settings.h"
#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include <QXmlStreamReader>

QDir StyleManager::themeResources;
QSharedPointer<fa::QtAwesome> StyleManager::awesome;

void
StyleManager::loadTheme()
{
  qApp->setStyle(QStyleFactory::create("Fusion"));

  QPalette themeColors;
  QFile styles, palette;
  switch (Settings::themeId) {
    case 0:
      themeResources.setPath(":/resources/light/");
      styles.setFileName(themeResources.filePath("light.qss"));
      palette.setFileName(themeResources.filePath("light.xml"));
      Settings::darkMode = false;
      break;

    case 1:
      themeResources.setPath(":/resources/light/");
      styles.setFileName(themeResources.filePath("sepia.qss"));
      palette.setFileName(themeResources.filePath("sepia.xml"));
      Settings::darkMode = false;
      break;

    case 2:
      themeResources.setPath(":/resources/dark/");
      styles.setFileName(themeResources.filePath("dark.qss"));
      palette.setFileName(themeResources.filePath("dark.xml"));
      Settings::darkMode = true;
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

  awesome = QSharedPointer<fa::QtAwesome>::create();
  awesome->initFontAwesome();
}
