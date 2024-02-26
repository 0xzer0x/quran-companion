#include "bookmarksdb.h"
#include <QSqlError>
#include <QSqlQuery>

QSharedPointer<BookmarksDb>
BookmarksDb::current()
{
  static QSharedPointer<BookmarksDb> bookmarkDb =
    QSharedPointer<BookmarksDb>::create();
  return bookmarkDb;
}

BookmarksDb::BookmarksDb()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "BookmarksCon"))
{
  BookmarksDb::open();
}

void
BookmarksDb::open()
{
  setDatabaseName(m_configDir->absoluteFilePath("bookmarks.db"));
  if (!QSqlDatabase::open())
    qFatal("Error opening bookmarks db");
}

DbConnection::Type
BookmarksDb::type()
{
  return DbConnection::Bookmarks;
}

bool
BookmarksDb::saveActiveKhatmah(QList<int> vInfo)
{
  QSqlQuery dbQuery(*this);
  QString q = QString::asprintf(
    "UPDATE khatmah SET page=%i, surah=%i, number=%i WHERE id=%i",
    vInfo[0],
    vInfo[1],
    vInfo[2],
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
BookmarksDb::getAllKhatmah() const
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
BookmarksDb::getKhatmahName(const int id) const
{
  QSqlQuery dbQuery(*this);
  if (!dbQuery.exec("SELECT name FROM khatmah WHERE id=" + QString::number(id)))
    qCritical() << "Couldn't execute sql query: " << dbQuery.lastQuery();

  dbQuery.next();
  return dbQuery.value(0).toString();
}

bool
BookmarksDb::loadVerse(const int khatmahId, QList<int>& vInfo) const
{
  QSqlQuery dbQuery(*this);

  QString q = QString::asprintf(
    "SELECT page,surah,number FROM khatmah WHERE id=%i", khatmahId);
  if (!dbQuery.exec(q)) {
    qCritical() << "Couldn't execute getPosition SQL query!";
    return false;
  }
  if (!dbQuery.next())
    return false;

  vInfo[0] = dbQuery.value(0).toInt();
  vInfo[1] = dbQuery.value(1).toInt();
  vInfo[2] = dbQuery.value(2).toInt();
  return true;
}

int
BookmarksDb::addKhatmah(QList<int> vInfo,
                        const QString name,
                        const int id) const
{
  QSqlQuery dbQuery(*this);
  dbQuery.exec(
    "CREATE TABLE IF NOT EXISTS khatmah(id INTEGER PRIMARY KEY "
    "AUTOINCREMENT, name TEXT, page INTEGER, surah INTEGER, number INTEGER)");
  QString q;
  if (id == -1) {
    q = "INSERT INTO khatmah(name, page, surah, number) VALUES ('%0', %1, %2, "
        "%3)";
    dbQuery.prepare(q.arg(name,
                          QString::number(vInfo[0]),
                          QString::number(vInfo[1]),
                          QString::number(vInfo[2])));
  } else {
    q = "REPLACE INTO khatmah VALUES "
        "(%0, "
        "'%1', %2, %3, %4)";
    dbQuery.prepare(q.arg(QString::number(id),
                          name,
                          QString::number(vInfo[0]),
                          QString::number(vInfo[1]),
                          QString::number(vInfo[2])));
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
BookmarksDb::editKhatmahName(const int khatmahId, QString newName)
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
BookmarksDb::removeKhatmah(const int id) const
{
  QSqlQuery dbQuery(*this);
  if (!dbQuery.exec(QString::asprintf("DELETE FROM khatmah WHERE id=%i", id)))
    qDebug() << "Couldn't execute query: " << dbQuery.lastQuery();
}

QList<QList<int>>
BookmarksDb::bookmarkedVerses(int surahIdx) const
{
  QList<QList<int>> results;
  QSqlQuery dbQuery(*this);
  QString q = "SELECT page,surah,number FROM favorites";
  if (surahIdx != -1)
    q.append(" WHERE surah=" + QString::number(surahIdx));

  dbQuery.prepare(q.append(" ORDER BY surah, number"));
  if (!dbQuery.exec())
    qCritical() << "Couldn't execute bookmarkedVerses SELECT query";

  while (dbQuery.next()) {
    results.append({ dbQuery.value(0).toInt(),
                     dbQuery.value(1).toInt(),
                     dbQuery.value(2).toInt() });
  }

  return results;
}

bool
BookmarksDb::isBookmarked(QList<int> vInfo) const
{
  QSqlQuery dbQuery(*this);

  dbQuery.prepare(
    "SELECT page FROM favorites WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, vInfo[0]);
  dbQuery.bindValue(1, vInfo[1]);
  dbQuery.bindValue(2, vInfo[2]);

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't check if verse is bookmarked";
    return false;
  }

  dbQuery.next();

  return dbQuery.isValid();
}

bool
BookmarksDb::addBookmark(QList<int> vInfo)
{
  QSqlQuery dbQuery(*this);
  dbQuery.exec("CREATE TABLE IF NOT EXISTS favorites(id INTEGER PRIMARY KEY "
               "AUTOINCREMENT,"
               "page INTEGER, surah INTEGER, number INTEGER)");

  dbQuery.prepare(
    "INSERT INTO favorites(page, surah, number) VALUES (:p, :s, :n)");
  dbQuery.bindValue(0, vInfo[0]);
  dbQuery.bindValue(1, vInfo[1]);
  dbQuery.bindValue(2, vInfo[2]);

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't add verse to bookmarks db";
    return false;
  }

  commit();
  emit bookmarkAdded();
  return true;
}

bool
BookmarksDb::removeBookmark(QList<int> vInfo)
{
  QSqlQuery dbQuery(*this);
  dbQuery.prepare(
    "DELETE FROM favorites WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, vInfo[0]);
  dbQuery.bindValue(1, vInfo[1]);
  dbQuery.bindValue(2, vInfo[2]);

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't remove verse from bookmarks";
    return false;
  }

  emit bookmarkRemoved();
  return true;
}

void
BookmarksDb::saveThoughts(QList<int> vInfo, const QString& text)
{
  int id = m_quranDb->verseId(vInfo[1], vInfo[2]);
  QSqlQuery dbQuery(*this);
  dbQuery.exec("CREATE TABLE IF NOT EXISTS thoughts(id INTEGER PRIMARY KEY "
               "UNIQUE,"
               "page INTEGER, surah INTEGER, number INTEGER, text TEXT)");

  dbQuery.prepare("REPLACE INTO thoughts(id, page, surah, number, text) "
                  "VALUES(:i, :p, :s, :n, :t)");
  dbQuery.bindValue(0, id);
  dbQuery.bindValue(1, vInfo[0]);
  dbQuery.bindValue(2, vInfo[1]);
  dbQuery.bindValue(3, vInfo[2]);
  dbQuery.bindValue(4, text);

  if (!dbQuery.exec())
    qCritical() << "SQL statement execution error:" << dbQuery.lastError();

  commit();
}

QString
BookmarksDb::getThoughts(QList<int> vInfo) const
{
  QSqlQuery dbQuery(*this);
  dbQuery.prepare(
    "SELECT text FROM thoughts WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, vInfo[0]);
  dbQuery.bindValue(1, vInfo[1]);
  dbQuery.bindValue(2, vInfo[2]);

  if (!dbQuery.exec())
    qCritical() << "SQL statement execution error:" << dbQuery.lastError();

  dbQuery.next();
  return dbQuery.value(0).toString();
}

QList<QPair<QList<int>, QString>>
BookmarksDb::allThoughts() const
{
  QList<QPair<QList<int>, QString>> all;
  QSqlQuery dbQuery(*this);
  dbQuery.exec("SELECT page,surah,number,text FROM thoughts WHERE text!=''");
  while (dbQuery.next()) {
    QList<int> vInfo(3);
    vInfo[0] = dbQuery.value(0).toInt();
    vInfo[1] = dbQuery.value(1).toInt();
    vInfo[2] = dbQuery.value(2).toInt();

    all.append({ vInfo, dbQuery.value(3).toString() });
  }

  return all;
}

void
BookmarksDb::setActiveKhatmah(const int id)
{
  m_activeKhatmah = id;
}

int
BookmarksDb::activeKhatmah() const
{
  return m_activeKhatmah;
}
