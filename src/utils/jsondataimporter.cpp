#include "jsondataimporter.h"
#include <QFile>
#include <QJsonArray>
#include <utils/servicefactory.h>

JsonDataImporter::JsonDataImporter()
  : m_bookmarkService(ServiceFactory::bookmarkService())
  , m_khatmahService(ServiceFactory::khatmahService())
  , m_thoughtsService(ServiceFactory::thoughtsService())
{
}

void
JsonDataImporter::importBookmarks()
{
  if (!validArray("bookmarks"))
    return;
  QJsonArray arr = m_fileObj.value("bookmarks").toArray();
  foreach (const QJsonValue& item, arr) {
    Verse bookmark = verseFromJson(item.toObject());
    if (bookmark.page() < 1 || bookmark.surah() < 1 || bookmark.number() < 1)
      continue;
    if (!m_bookmarkService->isBookmarked(bookmark))
      m_bookmarkService->addBookmark(bookmark, true);
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
    if (!khatmah.first.isEmpty())
      m_khatmahService->addKhatmah(khatmah.second, khatmah.first);
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
    if (!thought.second.isEmpty())
      m_thoughtsService->saveThoughts(thought.first, thought.second);
  }
}

bool
JsonDataImporter::validArray(QString key)
{
  if (!m_fileObj.contains(key)) {
    emit UserDataImporter::error(MissingKeyError, "Missing key: " + key);
    return false;
  }
  if (!m_fileObj.value(key).isArray()) {
    emit UserDataImporter::error(InvalidValueError, "Invalid array: " + key);
    return false;
  }
  return true;
}

bool
JsonDataImporter::validVerse(const QJsonObject& obj)
{
  if (!obj.contains("page") || !obj.contains("surah") ||
      !obj.contains("number")) {
    emit UserDataImporter::error(MissingKeyError, "Missing verse key");
    return false;
  }

  int page = obj.value("page").toInt(), surah = obj.value("surah").toInt(),
      number = obj.value("number").toInt();
  if (page < 1 || page > 604 || surah < 1 || surah > 114 || number < 1 ||
      number > 286) {
    emit UserDataImporter::error(InvalidValueError, "Invalid verse values");
    return false;
  }

  return true;
}

bool
JsonDataImporter::validKhatmah(const QJsonObject& obj)
{
  if (!obj.contains("name") || !obj.contains("verse")) {
    emit UserDataImporter::error(MissingKeyError, "Missing khatmah keys");
    return false;
  }

  if (!obj.value("name").isString() ||
      !validVerse(obj.value("verse").toObject())) {
    emit UserDataImporter::error(InvalidValueError, "Invalid khatmah values");
    return false;
  }

  return true;
}

bool
JsonDataImporter::validThought(const QJsonObject& obj)
{
  if (!obj.contains("text") || !obj.contains("verse")) {
    emit UserDataImporter::error(MissingKeyError, "Missing thought keys");
    return false;
  }

  if (!obj.value("text").isString() ||
      !validVerse(obj.value("verse").toObject())) {
    emit UserDataImporter::error(InvalidValueError, "Invalid thought values");
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
  Verse v(1, 1, 1);
  if (!validKhatmah(obj))
    return { name, v };

  name = obj.value("name").toString();
  v = verseFromJson(obj.value("verse").toObject());
  return { name, v };
}

QPair<Verse, QString>
JsonDataImporter::thoughtFromJson(const QJsonObject& obj)
{
  Verse v(1, 1, 1);
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
  m_file.setFile(path);
}

bool
JsonDataImporter::read()
{
  QFile jsonFile(m_file.absoluteFilePath());
  if (!jsonFile.open(QIODevice::ReadOnly)) {
    qWarning() << "Failed to open json file during import";
    emit UserDataImporter::error(
      IOError, "Failed to open json file: " + m_file.absoluteFilePath());
    return false;
  }

  QJsonParseError* err = nullptr;
  QJsonDocument document = QJsonDocument::fromJson(jsonFile.readAll(), err);
  if (document.isNull()) {
    qWarning() << "Failed to parse json file";
    emit UserDataImporter::error(
      ParseError, "Failed to parse json file: " + m_file.absoluteFilePath());
    if (err)
      qWarning() << "Error string:" << err->errorString();
    return false;
  }

  m_fileObj = document.object();
  return true;
}

bool
JsonDataImporter::fileContains(QString key)
{
  return m_fileObj.contains(key);
}
