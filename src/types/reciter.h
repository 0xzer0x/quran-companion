#ifndef RECITER_H
#define RECITER_H

#include <QList>
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QVariant>

/**
 * @class Reciter
 * @brief Reciter class represents a quran reciter
 */
class Reciter
{
public:
  static QList<Reciter> reciters;
  static void populateReciters();
  static std::optional<Reciter> findById(const QString id);
  static const int indexForReciter(const Reciter& reciter);
  static const Reciter defaultReciter();

  explicit Reciter(QString id,
                   QString dir,
                   QString display,
                   QString basmallah,
                   QString url,
                   bool downloadById = false);

  // NOTE: Override copy operator
  Reciter& operator=(const Reciter& r);
  // NOTE: Override comparison operators
  bool operator==(const Reciter& r) const;
  bool operator!=(const Reciter& r) const;

  const QString& id() const;
  const QString& basmallahPath() const;
  const QString& displayName() const;
  const QString& baseDirName() const;
  const QString& baseUrl() const;
  const bool downloadVerseById() const;

private:
  QString m_id;
  /**
   * @brief The name of the directory conatining recitations in the application
   * recitations directory.
   */
  QString m_baseDirName;
  /**
   * @brief The reciter name as its displayed in the UI.
   */
  QString m_displayName;
  /**
   * @brief Absolute path to the reciters bismillah audio file.
   */
  QString m_basmallahPath;
  /**
   * @brief Url to download recitation files from.
   */
  QString m_baseUrl;
  /**
   * @brief Boolean value representing whether the verse recitations should be
   * downloading using the verse number relative to the beginning of the Quran
   * or a combination of surah and verse numbers.
   */
  bool m_downloadVerseById;
};

#endif // RECITER_H
