#include "reciter.h"
#include <QApplication>
#include <QCollator>
#include <QFile>
#include <QXmlStreamReader>
#include <algorithm>
#include <utils/dirmanager.h>

QList<Reciter> Reciter::reciters;

void
Reciter::populateReciters()
{
  QFile recitersFile(":/resources/reciters.xml");
  if (!recitersFile.open(QIODevice::ReadOnly))
    qFatal("Couldn't Open Reciters XML, Exiting");

  QXmlStreamReader reader(&recitersFile);
  while (!reader.atEnd() && !reader.hasError()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (reader.name().toString() == "reciter") {
        QString baseDirName = reader.attributes().value("dirname").toString();
        QString displayName = qApp->translate(
          "MainWindow", reader.attributes().value("display").toLatin1());
        QString baseUrl = reader.attributes().value("url").toString();
        QString basmallahPath =
          DirManager::getInstance().basmallahDir().absoluteFilePath(
            reader.attributes().value("basmallah").toString());
        bool useId = reader.attributes().value("useid").toInt();

        reciters.append(
          Reciter(baseDirName, displayName, basmallahPath, baseUrl, useId));
      }
    }
  }

  recitersFile.close();
  reciters.squeeze();

  // sort reciters alphabetically by display name (locale-aware)
  QCollator collator;
  collator.setCaseSensitivity(Qt::CaseInsensitive);
  collator.setNumericMode(true);
  std::sort(reciters.begin(),
            reciters.end(),
            [&collator](const Reciter& a, const Reciter& b) {
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

Reciter::Reciter(QString dir,
                 QString display,
                 QString basmallah,
                 QString url,
                 bool useId)
  : m_baseDirName(dir)
  , m_displayName(display)
  , m_basmallahPath(basmallah)
  , m_baseUrl(url)
  , m_useId(useId)
{
}

QString
Reciter::baseUrl() const
{
  return m_baseUrl;
}

QString
Reciter::basmallahPath() const
{
  return m_basmallahPath;
}

QString
Reciter::displayName() const
{
  return m_displayName;
}

QString
Reciter::baseDirName() const
{
  return m_baseDirName;
}

bool
Reciter::useId() const
{
  return m_useId;
}

int
Reciter::indexByDirName(const QString& dirName)
{
  auto it = std::find_if(
    reciters.cbegin(), reciters.cend(), [&dirName](const Reciter& r) {
      return r.baseDirName() == dirName;
    });
  return it != reciters.cend() ? std::distance(reciters.cbegin(), it) : 0;
}

int
Reciter::resolveSettingsIndex(const QVariant& savedReciter)
{
  if (savedReciter.typeId() == QMetaType::QString)
    return indexByDirName(savedReciter.toString());

  return qBound(0, savedReciter.toInt(), reciters.size() - 1);
}
