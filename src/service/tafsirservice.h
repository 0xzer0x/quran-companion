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
   * @brief Sets the active tafsir
   * @param tafsir - ::Tafsir instance to set as the current tafsir
   */
  virtual bool setCurrentTafsir(const Tafsir tafsir) = 0;
  /**
   * @brief gets the tafsir content for the given verse using the active
   * ::Tafsir
   * @param sIdx - surah number
   * @param vIdx - verse number
   * @return QString containing the tafsir of the verse
   */
  virtual QString getTafsir(const int sIdx, const int vIdx) = 0;
  /**
   * @brief getter for the currently active ::Tafsir
   * @return The currently selected ::Tafsir
   */
  virtual const Tafsir currTafsir() const = 0;
};

#endif
