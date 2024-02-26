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
  void exportBookmarks();
  void exportKhatmah();
  void exportThoughts();
  void setFile(QString path);
  bool save();

private:
  QSharedPointer<BookmarksDb> m_bookmarksDb = BookmarksDb::current();
  QJsonObject verseJson(const Verse& v);
  QJsonObject khatmahJson(const QPair<QString, Verse>& entry);
  QJsonObject thoughtJson(const QPair<Verse, QString>& entry);
  QJsonObject m_fileObj;
  QFileInfo m_file;
};

#endif // JSONDATAEXPORTER_H
