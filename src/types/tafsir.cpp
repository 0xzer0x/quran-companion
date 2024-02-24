#include "tafsir.h"
#include "content.h"
#include "utils/dirmanager.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>

QList<QSharedPointer<Tafsir>> Tafsir::tafasir;

void
Tafsir::populateTafasir()
{
  QFile content(":/resources/files.xml");
  if (!content.open(QIODevice::ReadOnly))
    qCritical("Couldn't Open Files XML");

  QXmlStreamReader reader(&content);
  while (!reader.atEnd() && !reader.hasError()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (reader.name().toString() == "tafsir") {
        QString name = qApp->translate(
          "SettingsDialog", reader.attributes().value("name").toLatin1());
        QString file = reader.attributes().value("file").toString();
        bool isText = reader.attributes().value("text").toInt();
        bool isExtra = reader.attributes().value("extra").toInt();
        tafasir.append(
          QSharedPointer<Tafsir>::create(name, file, isText, isExtra));
      }
    }
  }

  content.close();
  tafasir.squeeze();
}

Tafsir::Tafsir(QString display, QString filename, bool isText, bool isExtra)
  : Content(display, filename, isExtra)
  , m_isText(isText)
{
}

bool
Tafsir::isAvailable() const
{
  const QDir& baseDir =
    isExtra() ? *DirManager::downloadsDir : *DirManager::assetsDir;
  return baseDir.exists("tafasir/" + filename());
}

const bool
Tafsir::isText() const
{
  return m_isText;
}
