#include "jsondataexporter.h"
#include <QJsonArray>
#include <utils/servicefactory.h>

JsonDataExporter::JsonDataExporter()
  : m_bookmarkService(ServiceFactory::bookmarkService())
  , m_khatmahService(ServiceFactory::khatmahService())
  , m_thoughtsService(ServiceFactory::thoughtsService())
{
}

void
JsonDataExporter::exportBookmarks()
{
  QJsonArray bookmarks;
  QList<Verse> all = m_bookmarkService->bookmarkedVerses();
  foreach (const Verse& v, all) {
    bookmarks.append(verseJson(v));
  }

  m_fileObj["bookmarks"] = bookmarks;
}

void
JsonDataExporter::exportKhatmah()
{
  QJsonArray khatmah;
  QList<int> ids = m_khatmahService->getAllKhatmah();
  foreach (const int id, ids) {
    Verse v = m_khatmahService->loadVerse(id).value();
    QString name = m_khatmahService->getKhatmahName(id);
    khatmah.append(khatmahJson({ name, v }));
  }

  m_fileObj["khatmah"] = khatmah;
}

void
JsonDataExporter::exportThoughts()
{
  QJsonArray thoughts;
  QList<QPair<Verse, QString>> all = m_thoughtsService->allThoughts();
  for (const QPair<Verse, QString>& item : all) {
    thoughts.append(thoughtJson(item));
  }
  m_fileObj["thoughts"] = thoughts;
}

QJsonObject
JsonDataExporter::verseJson(const Verse& v)
{
  QJsonObject obj;
  obj["page"] = v.page();
  obj["surah"] = v.surah();
  obj["number"] = v.number();
  return obj;
}

QJsonObject
JsonDataExporter::khatmahJson(const QPair<QString, Verse>& entry)
{
  QJsonObject obj;
  obj["name"] = entry.first;
  obj["verse"] = verseJson(entry.second);
  return obj;
}

QJsonObject
JsonDataExporter::thoughtJson(const QPair<Verse, QString>& entry)
{
  QJsonObject obj;
  obj["verse"] = verseJson(entry.first);
  obj["text"] = entry.second;
  return obj;
}

void
JsonDataExporter::setFile(QString path)
{
  m_file.setFile(path);
}

bool
JsonDataExporter::save()
{
  QFile jsonFile(m_file.absoluteFilePath());
  if (!jsonFile.open(QIODevice::WriteOnly)) {
    qWarning() << "Failed to open JSON file for writing";
    emit UserDataExporter::error(
      IOError, "Failed to open json file: " + m_file.absoluteFilePath());
    return false;
  }

  jsonFile.write(QJsonDocument(m_fileObj).toJson());
  jsonFile.close();
  return true;
}
