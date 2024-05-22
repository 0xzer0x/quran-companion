#ifndef JSONDATAEXPORTER_H
#define JSONDATAEXPORTER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <interfaces/userdataexporter.h>
#include <repository/bookmarksrepository.h>
#include <service/bookmarkservice.h>
#include <service/khatmahservice.h>
#include <service/thoughtsservice.h>
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
  BookmarkService* m_bookmarkService;
  KhatmahService* m_khatmahService;
  ThoughtsService* m_thoughtsService;
  QJsonObject verseJson(const Verse& v);
  QJsonObject khatmahJson(const QPair<QString, Verse>& entry);
  QJsonObject thoughtJson(const QPair<Verse, QString>& entry);
  QJsonObject m_fileObj;
  QFileInfo m_file;
};

#endif // JSONDATAEXPORTER_H
