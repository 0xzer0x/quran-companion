#include "translation.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <utils/configurationschema.h>
#include <utils/dirmanager.h>

QList<Translation> Translation::translations;

void
Translation::populateTranslations()
{
  QFile content(":/resources/files.xml");
  if (!content.open(QIODevice::ReadOnly))
    qCritical("Couldn't Open Files XML");

  QXmlStreamReader reader(&content);
  while (!reader.atEnd() && !reader.hasError()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (reader.name().toString() == "translation") {
        QString name = reader.attributes().value("name").toString();
        QString file = reader.attributes().value("file").toString();
        bool extra = reader.attributes().value("extra").toInt();
        QString id = file.first(file.size() - 3);

        translations.append(Translation(id, name, file, extra));
      }
    }
  }

  content.close();
  translations.squeeze();
}

std::optional<Translation>
Translation::findById(QString id)
{
  std::optional<Translation> result = std::nullopt;
  foreach (const Translation& translation, translations) {
    if (translation.id() == id) {
      result = translation;
      break;
    }
  }
  return result;
}

const int
Translation::indexForTranslation(const Translation& translation)
{
  return Translation::translations.indexOf(translation);
}

const Translation
Translation::defaultTranslation()
{
  const QString defaultId = ConfigurationSchema::getInstance()
                              .getDefault("Reader/Translation")
                              .value()
                              .toString();
  ;
  const std::optional<const Translation> translation =
    Translation::findById(defaultId);
  if (!translation.has_value()) {
    qFatal() << "Failed to find translation using default ID:" << defaultId;
  }
  return translation.value();
}

Translation::Translation(QString id,
                         QString display,
                         QString filename,
                         bool isExtra)
  : Content(id, display, filename, isExtra)
{
}

bool
Translation::operator==(const Translation& v2) const
{
  return this->id() == v2.id();
}

bool
Translation::operator!=(const Translation& v2) const
{
  return this->id() != v2.id();
}

bool
Translation::isAvailable() const
{
  const QDir& baseDir = isExtra() ? DirManager::getInstance().downloadsDir()
                                  : DirManager::getInstance().assetsDir();
  return baseDir.exists("translations/" + filename());
}
