#include "translation.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
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
        translations.append(Translation(name, file, extra));
      }
    }
  }

  content.close();
  translations.squeeze();
}

Translation::Translation(QString display, QString filename, bool isExtra)
  : Content(display, filename, isExtra)
{
}

bool
Translation::operator==(const Translation& v2) const
{
  return this->filename() == v2.filename();
}

bool
Translation::operator!=(const Translation& v2) const
{
  return this->filename() != v2.filename();
}

bool
Translation::isAvailable() const
{
  const QDir& baseDir = isExtra() ? DirManager::getInstance().downloadsDir()
                                  : DirManager::getInstance().assetsDir();
  return baseDir.exists("translations/" + filename());
}
