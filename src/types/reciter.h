#ifndef RECITER_H
#define RECITER_H

#include <QList>
#include <QSharedPointer>
#include <QString>

/**
 * @class Reciter
 * @brief Reciter class represents a quran reciter
 */
class Reciter
{
public:
  static QList<Reciter> reciters;
  static void populateReciters();

  explicit Reciter(QString dir,
                   QString display,
                   QString basmallah,
                   QString url,
                   bool useId = false);

  QString basmallahPath() const;
  QString displayName() const;
  QString baseDirName() const;
  QString baseUrl() const;
  bool useId() const;

private:
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
  bool m_useId;
};

#endif // RECITER_H
