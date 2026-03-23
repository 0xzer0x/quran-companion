#include "reciter.h"
#include <QApplication>
#include <QCollator>
#include <QFile>
#include <QXmlStreamReader>
#include <algorithm>
#include <optional>
#include <utils/configurationschema.h>
#include <utils/dirmanager.h>

QList<Reciter> Reciter::reciters;

void
Reciter::populateReciters()
{
  QFile recitersFile(":/resources/reciters.xml");
  if (!recitersFile.open(QIODevice::ReadOnly))
    qFatal("Faile to open reciters XML file");

  QXmlStreamReader reader(&recitersFile);
  while (!reader.atEnd() && !reader.hasError()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (reader.name().toString() == "reciter") {
        QString id = reader.attributes().value("id").toString();
        QString baseDirName = reader.attributes().value("dirname").toString();
        QString displayName =
          QCoreApplication::translate("MainWindow", reader.attributes().value("display").toLatin1());
        QString baseUrl = reader.attributes().value("url").toString();
        QString basmallahPath =
          DirManager::getInstance().basmallahDir().absoluteFilePath(reader.attributes().value("basmallah").toString());
        bool downloadById = reader.attributes().value("downloadById").toInt();

        reciters.append(Reciter(id, baseDirName, displayName, basmallahPath, baseUrl, downloadById));
      }
    }
  }

  recitersFile.close();
  reciters.squeeze();

  // sort reciters alphabetically by display name (locale-aware)
  QCollator collator;
  collator.setCaseSensitivity(Qt::CaseInsensitive);
  collator.setNumericMode(true);
  std::sort(reciters.begin(), reciters.end(), [&collator](const Reciter& a, const Reciter& b) {
    return collator.compare(a.displayName(), b.displayName()) < 0;
  });

  // create reciters directories
  QString temp = "recitations/%0";
  foreach (const Reciter& r, reciters) {
    QString path = temp.arg(r.baseDirName());
    if (!DirManager::getInstance().downloadsDir().exists(path))
      DirManager::getInstance().downloadsDir().mkdir(path);
  }
}

std::optional<Reciter>
Reciter::findById(const QString id)
{
  std::optional<Reciter> reciter = std::nullopt;

  const auto iterator = std::find_if(Reciter::reciters.constBegin(),
                                     Reciter::reciters.constEnd(),
                                     [&id](const Reciter& reciter) { return reciter.id() == id; });

  // NOTE: Dereference iterator if found
  if (iterator != Reciter::reciters.constEnd()) {
    reciter = *iterator;
  }

  return reciter;
}

const int
Reciter::indexForReciter(const Reciter& reciter)
{
  return Reciter::reciters.indexOf(reciter);
}

const Reciter
Reciter::defaultReciter()
{
  const QString defaultId = ConfigurationSchema::getInstance().getDefault("Reciter").value().toString();

  const std::optional<const Reciter> reciter = Reciter::findById(defaultId);
  if (!reciter.has_value()) {
    qFatal("Default reciter not found, ID: %s", qPrintable(defaultId));
  }
  return reciter.value();
}

Reciter::Reciter(QString id, QString dir, QString display, QString basmallah, QString url, bool downloadById)
  : m_id(id)
  , m_baseDirName(dir)
  , m_displayName(display)
  , m_basmallahPath(basmallah)
  , m_baseUrl(url)
  , m_downloadVerseById(downloadById)
{
}

Reciter&
Reciter::operator=(const Reciter& r)
{
  m_id = r.m_id;
  m_displayName = r.m_displayName;
  m_baseDirName = r.m_baseDirName;
  m_basmallahPath = r.m_basmallahPath;
  m_baseUrl = r.m_baseUrl;
  m_downloadVerseById = r.m_downloadVerseById;

  return *this;
}

bool
Reciter::operator==(const Reciter& r) const
{
  return this->id() == r.id();
}

bool
Reciter::operator!=(const Reciter& r) const
{
  return this->id() != r.id();
}

const QString&
Reciter::id() const
{
  return m_id;
}

const QString&
Reciter::baseUrl() const
{
  return m_baseUrl;
}

const QString&
Reciter::basmallahPath() const
{
  return m_basmallahPath;
}

const QString&
Reciter::displayName() const
{
  return m_displayName;
}

const QString&
Reciter::baseDirName() const
{
  return m_baseDirName;
}

const bool
Reciter::downloadVerseById() const
{
  return m_downloadVerseById;
}
