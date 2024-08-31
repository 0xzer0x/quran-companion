#include "tafsir.h"
#include "content.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <utils/dirmanager.h>

QList<Tafsir> Tafsir::tafasir;

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
        QString id = file.first(file.size() - 3);

        tafasir.append(Tafsir(id, name, file, isText, isExtra));
      }
    }
  }

  content.close();
  tafasir.squeeze();
}

std::optional<Tafsir>
Tafsir::findById(QString id)
{
  std::optional<Tafsir> result = std::nullopt;
  foreach (const Tafsir& tafsir, tafasir) {
    if (tafsir.id() == id) {
      result = tafsir;
      break;
    }
  }
  return result;
}

Tafsir::Tafsir(QString id,
               QString display,
               QString filename,
               bool isText,
               bool isExtra)
  : Content(id, display, filename, isExtra)
  , m_isText(isText)
{
}

Tafsir&
Tafsir::operator=(const Tafsir& other)
{
  m_id = other.m_id;
  m_displayName = other.m_displayName;
  m_filename = other.m_filename;
  m_isExtra = other.m_isExtra;
  m_isText = other.m_isText;

  return *this;
}

bool
Tafsir::operator==(const Tafsir& v2) const
{
  return this->id() == v2.id();
}

bool
Tafsir::operator!=(const Tafsir& v2) const
{
  return this->id() != v2.id();
}

bool
Tafsir::isAvailable() const
{
  const QDir& baseDir = isExtra() ? DirManager::getInstance().downloadsDir()
                                  : DirManager::getInstance().assetsDir();
  return baseDir.exists("tafasir/" + filename());
}

const bool
Tafsir::isText() const
{
  return m_isText;
}
