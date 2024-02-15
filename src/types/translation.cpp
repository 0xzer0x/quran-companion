#include "translation.h"
#include "../utils/dirmanager.h"
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
  : m_displayName(display)
  , m_filename(filename)
  , m_isExtra(isExtra)
{
}

bool
Translation::translationExists(int idx)
{
  const QSharedPointer<Translation>& tr = translations.at(idx);
  return translationExists(tr);
}

bool
Translation::translationExists(const QSharedPointer<Translation>& tr)
{
  const QDir& baseDir =
    tr->isExtra() ? *DirManager::downloadsDir : *DirManager::assetsDir;
  return baseDir.exists("translations/" + tr->filename());
}

const QString&
Translation::displayName() const
{
  return m_displayName;
}

const QString&
Translation::filename() const
{
  return m_filename;
}

const bool&
Translation::isExtra() const
{
  return m_isExtra;
}
