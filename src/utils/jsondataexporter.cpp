#include "jsondataexporter.h"
#include <QJsonArray>

JsonDataExporter::JsonDataExporter() {}

void
JsonDataExporter::exportBookmarks()
{
  QJsonArray bookmarks;
  QList<Verse> all = m_bookmarksDb->bookmarkedVerses();
  foreach (const Verse& v, all) {
    bookmarks.append(verseJson(v));
  }

  m_fileObj["bookmarks"] = bookmarks;
}

void
JsonDataExporter::exportKhatmah()
{
  QJsonArray khatmah;
  QList<int> ids = m_bookmarksDb->getAllKhatmah();
  foreach (const int id, ids) {
    Verse v;
    m_bookmarksDb->loadVerse(id, v);
    QString name = m_bookmarksDb->getKhatmahName(id);
    khatmah.append(khatmahJson({ name, v }));
  }

  m_fileObj["khatmah"] = khatmah;
}

void
JsonDataExporter::exportThoughts()
{
  QJsonArray thoughts;
  QList<QPair<Verse, QString>> all = m_bookmarksDb->allThoughts();
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
    emit UserDataExporter::error(IOError);
    return false;
  }

  jsonFile.write(QJsonDocument(m_fileObj).toJson());
  jsonFile.close();
  return true;
}
