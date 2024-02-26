#ifndef JSONDATAEXPORTER_H
#define JSONDATAEXPORTER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <database/bookmarksdb.h>
#include <interfaces/userdataexporter.h>
#include <types/verse.h>

class JsonDataExporter : public UserDataExporter
{
public:
  JsonDataExporter();
  void setFile(QString path);
  void exportBookmarks();
  void exportKhatmah();
  void exportThoughts();
  bool save();

private:
  QSharedPointer<BookmarksDb> m_bookmarksDb = BookmarksDb::current();
  QJsonObject verseJson(const Verse& v);
  QJsonObject verseJson(const QList<int>& vInfo);
  QJsonObject khatmahJson(const QPair<QString, QList<int>>& entry);
  QJsonObject thoughtJson(const QPair<QList<int>, QString>& entry);
  QJsonObject m_fileObj;
  QFileInfo m_file;
};

#endif // JSONDATAEXPORTER_H
