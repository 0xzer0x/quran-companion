#include "reciter.h"
#include "../utils/dirmanager.h"
#include <QApplication>
#include <QFile>
#include <QXmlStreamReader>

QList<QSharedPointer<Reciter>> Reciter::reciters;

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
        QString basmallahPath = DirManager::basmallahDir->absoluteFilePath(
          reader.attributes().value("basmallah").toString());
        bool useId = reader.attributes().value("useid").toInt();

        reciters.append(QSharedPointer<Reciter>::create(
          baseDirName, displayName, basmallahPath, baseUrl, useId));
      }
    }
  }

  recitersFile.close();
  reciters.squeeze();

  // create reciters directories
  DirManager::downloadsDir->cd("recitations");
  foreach (const QSharedPointer<Reciter>& r, reciters) {
    if (!DirManager::downloadsDir->exists(r->baseDirName()))
      DirManager::downloadsDir->mkdir(r->baseDirName());
  }
  DirManager::downloadsDir->cdUp();
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
