#ifndef TAFSIRSERVICE_H
#define TAFSIRSERVICE_H

#include <QString>
#include <types/tafsir.h>

class TafsirService
{
public:
  /**
   * @brief set tafsir to the one in the settings, update the selected db
   */
  virtual void loadTafsir() = 0;
  /**
   * @brief sets the active tafsir
   * @param tafsirName - DBManager::Tafsir entry
   */
  virtual bool setCurrentTafsir(QString id) = 0;
  /**
   * @brief gets the tafsir content for the given verse using the active
   * DBManager::Tafsir
   * @param sIdx - surah number
   * @param vIdx - verse number
   * @return QString containing the tafsir of the verse
   */
  virtual QString getTafsir(const int sIdx, const int vIdx) = 0;
  /**
   * @brief getter for m_currTafsir
   * @return pointer to the currently selected Tafasir
   */
  virtual std::optional<const Tafsir> currTafsir() const = 0;
};

#endif
