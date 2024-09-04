#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QObject>
#include <QString>

/**
 * @class DbConnection
 * @brief Abstract base class for managing database connections.
 *
 * This class defines the interface for database connection management. Derived
 * classes should implement the `open()` method to set and open the database
 * connection and the `type()` method to return the type of database connection.
 */
class DbConnection : public QObject
{
public:
  /**
   * @enum Type
   * @brief Enum to specify different types of database connections.
   *
   * This enum defines various types of database connections, each associated
   * with different database files or functionalities.
   */
  enum Type
  {
    Quran,      ///< Represents the main Quran database file (quran.db)
    Glyphs,     ///< Represents the QCF glyphs database file (glyphs.db)
    Betaqat,    ///< Represents the Betaqat database file
    Bookmarks,  ///< Represents the bookmarks database file (bookmarks.db)
    Tafsir,     ///< Represents the currently selected tafsir database file
    Translation ///< Represents the currently selected translation database file
  };
  /**
   * @brief Sets and opens the database connection.
   *
   * This pure virtual function must be implemented by derived classes to
   * set up and open the database connection.
   */
  virtual void open() = 0;
  /**
   * @brief Returns the type of the database connection.
   * @return Type of the database connection as DbConnection::Type.
   */
  virtual Type type() = 0;
};

#endif // DBCONNECTION_H
