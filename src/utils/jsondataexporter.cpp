#include "jsondataexporter.h"
#include <QJsonArray>

JsonDataExporter::JsonDataExporter() {}

void
JsonDataExporter::exportBookmarks()
{
  QJsonArray bookmarks;
  QList<QList<int>> all = m_bookmarksDb->bookmarkedVerses();
  foreach (const QList<int>& vInfo, all) {
    bookmarks.append(verseJson(vInfo));
  }

  m_fileObj["bookmarks"] = bookmarks;
}

void
JsonDataExporter::exportKhatmah()
{
  QJsonArray khatmah;
  QList<int> ids = m_bookmarksDb->getAllKhatmah();
  foreach (const int id, ids) {
    QList<int> vInfo(3);
    m_bookmarksDb->loadVerse(id, vInfo);
    QString name = m_bookmarksDb->getKhatmahName(id);
    khatmah.append(khatmahJson({ name, vInfo }));
  }

  m_fileObj["khatmah"] = khatmah;
}

void
JsonDataExporter::exportThoughts()
{
  QJsonArray thoughts;
  QList<QPair<QList<int>, QString>> all = m_bookmarksDb->allThoughts();
  for (const QPair<QList<int>, QString>& item : all) {
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
JsonDataExporter::verseJson(const QList<int>& vInfo)
{
  QJsonObject obj;
  obj["page"] = vInfo[0];
  obj["surah"] = vInfo[1];
  obj["number"] = vInfo[2];
  return obj;
}

QJsonObject
JsonDataExporter::khatmahJson(const QPair<QString, QList<int>>& entry)
{
  QJsonObject obj;
  obj["name"] = entry.first;
  obj["verse"] = verseJson(entry.second);
  return obj;
}

QJsonObject
JsonDataExporter::thoughtJson(const QPair<QList<int>, QString>& entry)
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
    return false;
  }

  jsonFile.write(QJsonDocument(m_fileObj).toJson());
  jsonFile.close();
  return true;
}
