#ifndef KHATMAHSERVICE_H
#define KHATMAHSERVICE_H

#include <types/verse.h>

class KhatmahService
{
public:
  /**
   * @brief load the last saved Verse for the given khatmah
   * @return An optional containing the Verse corresponding to the given khatmah
   */
  virtual std::optional<Verse> loadVerse(const int khatmahId) const = 0;
  /**
   * @brief sets the given Verse as the last position reached in the current
   * active khatmah
   * @param v - Verse reached in khatmah
   */
  virtual bool saveActiveKhatmah(const Verse& verse) = 0;
  /**
   * @brief get all available khatmah ids
   * @return QList of khatmah id(s)
   */
  virtual QList<int> getAllKhatmah() const = 0;
  /**
   * @brief get the name of the khatmah with id given
   * @return QString containing the khatmah name
   */
  virtual QString getKhatmahName(const int id) const = 0;
  /**
   * @brief add a new khatmah/replace khatmah with given id with position of
   * Verse v
   * @param v - Verse to set as the khatmah position
   * @param name - new khatmah name
   * @param id - id of khatmah to replace, -1 means do not replace (default: -1)
   * @return id of newly added khatmah or id parameter if defined
   */
  virtual int addKhatmah(const Verse& verse,
                         const QString name,
                         const int id = -1) const = 0;
  /**
   * @brief rename the khatmah with the given id to newName
   * @param khatmahId - id of khatmah to rename
   * @param newName - new name to set
   * @return boolean indicating a successful operation (false in case the name
   * exists)
   */
  virtual bool editKhatmahName(const int khatmahId, QString newName) = 0;
  /**
   * @brief remove the khatmah with the given id from database
   * @param id - id of khatmah to remove
   */
  virtual void removeKhatmah(const int id) const = 0;
  /**
   * @brief setter for m_activeKhatmah
   * @param id - id of the active khatmah
   */
  virtual void setActiveKhatmah(const int id) = 0;
  /**
   * @brief getter for m_activeKhatmah
   * @return int - id of active khatmah
   */
  virtual int activeKhatmah() const = 0;
};

#endif // KHATMAHSERVICE_H
