#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QObject>
#include <QString>

class DbConnection : public QObject
{
public:
  /**
   * @brief Type enum holds different values representing database files
   * used in different member functions.
   */
  enum Type
  {
    Quran,  ///< (quran.db) main Quran database file
    Glyphs, ///< (glyphs.db) QCF glyphs database
    Betaqat,
    Bookmarks,  ///< (bookmarks.db) bookmarked verses and khatmah database
    Tafsir,     ///< currently selected tafsir database file
    Translation ///< currently selected translation database file
  };

  /**
   * @brief sets and opens the database connection
   */
  virtual void open() = 0;
  /**
   * @brief getter for the type of the connection
   * @return - DbConnection::Translation
   */
  virtual Type type() = 0;
};

#endif // DBCONNECTION_H
