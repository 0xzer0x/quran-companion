#include "jsondataimporter.h"
#include <QFile>
#include <QJsonArray>

JsonDataImporter::JsonDataImporter() {}

void
JsonDataImporter::importBookmarks()
{
  if (!validArray("bookmarks"))
    return;
  QJsonArray arr = m_fileObj.value("bookmarks").toArray();
  foreach (const QJsonValue& item, arr) {
    Verse bookmark = verseFromJson(item.toObject());
    if (!m_bookmarksDb->isBookmarked(bookmark))
      m_bookmarksDb->addBookmark(bookmark, true);
  }
}

void
JsonDataImporter::importKhatmah()
{
  if (!validArray("khatmah"))
    return;
  QJsonArray arr = m_fileObj.value("khatmah").toArray();
  foreach (const QJsonValue& item, arr) {
    QPair<QString, Verse> khatmah = khatmahFromJson(item.toObject());
    m_bookmarksDb->addKhatmah(khatmah.second, khatmah.first);
  }
}

void
JsonDataImporter::importThoughts()
{
  if (!validArray("thoughts"))
    return;
  QJsonArray arr = m_fileObj.value("thoughts").toArray();
  foreach (const QJsonValue& item, arr) {
    QPair<Verse, QString> thought = thoughtFromJson(item.toObject());
    m_bookmarksDb->saveThoughts(thought.first, thought.second);
  }
}

bool
JsonDataImporter::validArray(QString key)
{
  if (!m_fileObj.contains(key)) {
    emit UserDataImporter::error(MissingKeyError);
    return false;
  }
  if (!m_fileObj.value(key).isArray()) {
    emit UserDataImporter::error(InvalidValueError);
    return false;
  }
  return true;
}

bool
JsonDataImporter::validVerse(const QJsonObject& obj)
{
  if (!obj.contains("page") || !obj.contains("surah") ||
      !obj.contains("number")) {
    emit UserDataImporter::error(MissingKeyError);
    return false;
  }

  int page = obj.value("page").toInt(), surah = obj.value("surah").toInt(),
      number = obj.value("number").toInt();
  if (page < 1 || page > 604 || surah < 1 || surah > 114 || number < 1 ||
      number > 286) {
    emit UserDataImporter::error(InvalidValueError);
    return false;
  }

  return true;
}

bool
JsonDataImporter::validKhatmah(const QJsonObject& obj)
{
  if (!obj.contains("name") || !obj.contains("verse")) {
    emit UserDataImporter::error(MissingKeyError);
    return false;
  }

  if (!obj.value("name").isString() ||
      !validVerse(obj.value("verse").toObject())) {
    emit UserDataImporter::error(InvalidValueError);
    return false;
  }

  return true;
}

bool
JsonDataImporter::validThought(const QJsonObject& obj)
{
  if (!obj.contains("text") || !obj.contains("verse")) {
    emit UserDataImporter::error(MissingKeyError);
    return false;
  }

  if (!obj.value("text").isString() ||
      !validVerse(obj.value("verse").toObject())) {
    emit UserDataImporter::error(InvalidValueError);
    return false;
  }

  return true;
}

Verse
JsonDataImporter::verseFromJson(const QJsonObject& obj)
{
  Verse v;
  if (!validVerse(obj))
    return v;

  v.setPage(obj.value("page").toInt());
  v.setSurah(obj.value("surah").toInt());
  v.setNumber(obj.value("number").toInt());
  return v;
}

QPair<QString, Verse>
JsonDataImporter::khatmahFromJson(const QJsonObject& obj)
{
  QString name;
  Verse v;
  if (!validKhatmah(obj))
    return { name, v };

  name = obj.value("name").toString();
  v = verseFromJson(obj.value("verse").toObject());
  return { name, v };
}

QPair<Verse, QString>
JsonDataImporter::thoughtFromJson(const QJsonObject& obj)
{
  Verse v;
  QString text;
  if (!validThought(obj))
    return { v, text };

  v = verseFromJson(obj.value("verse").toObject());
  text = obj.value("text").toString();
  return { v, text };
}

void
JsonDataImporter::setFile(QString path)
{
  m_filepath = path;
}

bool
JsonDataImporter::read()
{
  QFile jsonFile(m_filepath);
  if (!jsonFile.open(QIODevice::ReadOnly)) {
    qWarning() << "Failed to open json file during import";
    emit UserDataImporter::error(IOError);
    return false;
  }

  QJsonParseError* err = nullptr;
  QJsonDocument document = QJsonDocument::fromJson(jsonFile.readAll(), err);
  if (document.isNull()) {
    qWarning() << "Failed to parse json file";
    emit UserDataImporter::error(ParseError);
    if (err)
      qWarning() << "Error string:" << err->errorString();
    return false;
  }

  m_fileObj = document.object();
  return true;
}
