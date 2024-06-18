#include "bookmarksrepository.h"
#include <QSqlError>
#include <QSqlQuery>
#include <service/servicefactory.h>

BookmarksRepository&
BookmarksRepository::getInstance()
{
  static BookmarksRepository bookmarkDb;
  return bookmarkDb;
}

BookmarksRepository::BookmarksRepository()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "BookmarksCon"))
  , m_config(Configuration::getInstance())
  , m_configDir(DirManager::getInstance().configDir())
  , m_quranService(ServiceFactory::quranService())
{
  BookmarksRepository::open();
  QSqlQuery dbQuery(*this);
  dbQuery.exec(
    "CREATE TABLE IF NOT EXISTS khatmah(id INTEGER PRIMARY KEY "
    "AUTOINCREMENT, name TEXT, page INTEGER, surah INTEGER, number INTEGER)");

  dbQuery.exec("CREATE TABLE IF NOT EXISTS favorites(id INTEGER PRIMARY KEY "
               "AUTOINCREMENT,"
               "page INTEGER, surah INTEGER, number INTEGER)");

  dbQuery.exec("CREATE TABLE IF NOT EXISTS thoughts(id INTEGER PRIMARY KEY "
               "UNIQUE,"
               "page INTEGER, surah INTEGER, number INTEGER, text TEXT)");
}

void
BookmarksRepository::open()
{
  setDatabaseName(m_configDir.absoluteFilePath("bookmarks.db"));
  if (!QSqlDatabase::open())
    qFatal("Error opening bookmarks db");
}

DbConnection::Type
BookmarksRepository::type()
{
  return DbConnection::Bookmarks;
}

bool
BookmarksRepository::saveActiveKhatmah(const Verse& verse)
{
  QSqlQuery dbQuery(*this);
  QString q = QString::asprintf(
    "UPDATE khatmah SET page=%i, surah=%i, number=%i WHERE id=%i",
    verse.page(),
    verse.surah(),
    verse.number(),
    m_activeKhatmah);
  if (!dbQuery.exec(q)) {
    qCritical() << "Couldn't save position in mushaf";
    return false;
  }
  if (!commit())
    return false;

  return true;
}

QList<int>
BookmarksRepository::getAllKhatmah() const
{
  QList<int> res;
  QSqlQuery dbQuery(*this);
  if (!dbQuery.exec("SELECT id FROM khatmah"))
    qCritical() << "Couldn't execute sql query: " << dbQuery.lastQuery();

  while (dbQuery.next())
    res.append(dbQuery.value(0).toInt());

  return res;
}

QString
BookmarksRepository::getKhatmahName(const int id) const
{
  QSqlQuery dbQuery(*this);
  if (!dbQuery.exec("SELECT name FROM khatmah WHERE id=" + QString::number(id)))
    qCritical() << "Couldn't execute sql query: " << dbQuery.lastQuery();

  dbQuery.next();
  return dbQuery.value(0).toString();
}

std::optional<Verse>
BookmarksRepository::loadVerse(const int khatmahId) const
{
  QSqlQuery dbQuery(*this);

  QString q = QString::asprintf(
    "SELECT page,surah,number FROM khatmah WHERE id=%i", khatmahId);
  if (!dbQuery.exec(q)) {
    qCritical() << "Couldn't execute getPosition SQL query!";
    return std::nullopt;
  }
  if (!dbQuery.next())
    return std::nullopt;

  int page = dbQuery.value(0).toInt();
  int surah = dbQuery.value(1).toInt();
  int num = dbQuery.value(2).toInt();
  return std::optional<Verse>(Verse(page, surah, num));
}

int
BookmarksRepository::addKhatmah(const Verse& verse,
                                const QString name,
                                const int id) const
{
  QSqlQuery dbQuery(*this);
  QString q;
  if (id == -1) {
    q = "INSERT INTO khatmah(name, page, surah, number) VALUES ('%0', %1, %2, "
        "%3)";
    dbQuery.prepare(q.arg(name,
                          QString::number(verse.page()),
                          QString::number(verse.surah()),
                          QString::number(verse.number())));
  } else {
    q = "REPLACE INTO khatmah VALUES "
        "(%0, "
        "'%1', %2, %3, %4)";
    dbQuery.prepare(q.arg(QString::number(id),
                          name,
                          QString::number(verse.page()),
                          QString::number(verse.surah()),
                          QString::number(verse.number())));
  }

  if (!dbQuery.exec()) {
    qCritical() << "Couldn't create new khatmah entry!";
    qDebug() << lastError();
    return -1;
  }

  if (id != -1)
    return id;

  dbQuery.exec("SELECT id FROM khatmah ORDER BY id DESC limit 1");
  dbQuery.next();
  return dbQuery.value(0).toInt();
}

bool
BookmarksRepository::editKhatmahName(const int khatmahId, QString newName)
{
  QSqlQuery dbQuery(*this);
  QString q = "SELECT DISTINCT id FROM khatmah WHERE name='%0'";
  if (!dbQuery.exec(q.arg(newName))) {
    qCritical() << "Couldn't execute sql query: " << dbQuery.lastQuery();
    qDebug() << lastError();
    return false;
  }
  if (dbQuery.next())
    return false;

  q = "UPDATE khatmah SET name='%0' WHERE id=%1";
  if (!dbQuery.exec(q.arg(newName, QString::number(khatmahId)))) {
    qCritical() << "Couldn't rename khatmah entry!";
    qDebug() << lastError();
    return false;
  }

  commit();
  return true;
}

void
BookmarksRepository::removeKhatmah(const int id) const
{
  QSqlQuery dbQuery(*this);
  if (!dbQuery.exec(QString::asprintf("DELETE FROM khatmah WHERE id=%i", id)))
    qDebug() << "Couldn't execute query: " << dbQuery.lastQuery();
}

QList<Verse>
BookmarksRepository::bookmarkedVerses(int surahIdx) const
{
  QList<Verse> results;
  QSqlQuery dbQuery(*this);
  QString q = "SELECT page,surah,number FROM favorites";
  if (surahIdx != -1)
    q.append(" WHERE surah=" + QString::number(surahIdx));

  dbQuery.prepare(q.append(" ORDER BY surah, number"));
  if (!dbQuery.exec())
    qCritical() << "Couldn't execute bookmarkedVerses SELECT query";

  while (dbQuery.next()) {
    results.append(Verse(dbQuery.value(0).toInt(),
                         dbQuery.value(1).toInt(),
                         dbQuery.value(2).toInt()));
  }

  return results;
}

bool
BookmarksRepository::isBookmarked(const Verse& verse) const
{
  QSqlQuery dbQuery(*this);

  dbQuery.prepare(
    "SELECT page FROM favorites WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, verse.page());
  dbQuery.bindValue(1, verse.surah());
  dbQuery.bindValue(2, verse.number());

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't check if verse is bookmarked";
    return false;
  }

  dbQuery.next();

  return dbQuery.isValid();
}

bool
BookmarksRepository::addBookmark(const Verse& verse)
{
  QSqlQuery dbQuery(*this);

  dbQuery.prepare(
    "INSERT INTO favorites(page, surah, number) VALUES (:p, :s, :n)");
  dbQuery.bindValue(0, verse.page());
  dbQuery.bindValue(1, verse.surah());
  dbQuery.bindValue(2, verse.number());

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't add verse to bookmarks db";
    return false;
  }

  commit();
  return true;
}

bool
BookmarksRepository::removeBookmark(const Verse& verse)
{
  QSqlQuery dbQuery(*this);
  dbQuery.prepare(
    "DELETE FROM favorites WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, verse.page());
  dbQuery.bindValue(1, verse.surah());
  dbQuery.bindValue(2, verse.number());

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't remove verse from bookmarks";
    return false;
  }

  return true;
}

void
BookmarksRepository::saveThoughts(Verse& verse, const QString& text)
{
  int id = Verse::id(verse.surah(), verse.number());
  QSqlQuery dbQuery(*this);
  dbQuery.prepare("REPLACE INTO thoughts(id, page, surah, number, text) "
                  "VALUES(:i, :p, :s, :n, :t)");
  dbQuery.bindValue(0, id);
  dbQuery.bindValue(1, verse.page());
  dbQuery.bindValue(2, verse.surah());
  dbQuery.bindValue(3, verse.number());
  dbQuery.bindValue(4, text);

  if (!dbQuery.exec())
    qCritical() << "SQL statement execution error:" << dbQuery.lastError();

  commit();
}

QString
BookmarksRepository::getThoughts(const Verse& verse) const
{
  QSqlQuery dbQuery(*this);
  dbQuery.prepare(
    "SELECT text FROM thoughts WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, verse.page());
  dbQuery.bindValue(1, verse.surah());
  dbQuery.bindValue(2, verse.number());

  if (!dbQuery.exec())
    qCritical() << "SQL statement execution error:" << dbQuery.lastError();

  dbQuery.next();
  return dbQuery.value(0).toString();
}

QList<QPair<Verse, QString>>
BookmarksRepository::allThoughts() const
{
  QList<QPair<Verse, QString>> all;
  QSqlQuery dbQuery(*this);
  dbQuery.exec("SELECT page,surah,number,text FROM thoughts WHERE text!=''");
  while (dbQuery.next()) {
    const Verse verse(dbQuery.value(0).toInt(),
                      dbQuery.value(1).toInt(),
                      dbQuery.value(2).toInt());

    all.append({ verse, dbQuery.value(3).toString() });
  }

  return all;
}

void
BookmarksRepository::setActiveKhatmah(const int id)
{
  m_activeKhatmah = id;
}

int
BookmarksRepository::activeKhatmah() const
{
  return m_activeKhatmah;
}
