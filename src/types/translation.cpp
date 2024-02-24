#include "translation.h"
#include "utils/dirmanager.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>

QList<QSharedPointer<Translation>> Translation::translations;

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
        translations.append(
          QSharedPointer<Translation>::create(name, file, extra));
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
Translation::isAvailable() const
{
  const QDir& baseDir =
    isExtra() ? *DirManager::downloadsDir : *DirManager::assetsDir;
  return baseDir.exists("translations/" + filename());
}
