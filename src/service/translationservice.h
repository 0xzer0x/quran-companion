#ifndef TRANSLATIONSERVICE_H
#define TRANSLATIONSERVICE_H

#include <QObject>
#include <QString>
#include <types/translation.h>

class TranslationService : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief sets the active translation
   * @param translationName - DBManager::Translation entry
   */
  virtual bool setCurrentTranslation(QString id) = 0;
  /**
   * @brief gets the translation of the given verse using the active
   * DBManager::Translation
   * @param sIdx - surah number
   * @param vIdx - verse number
   * @return QString containing the verse translation
   */
  virtual QString getTranslation(const int sIdx, const int vIdx) const = 0;
  /**
   * @brief getter for m_currTr
   * @return pointer to the currently selected translation
   */
  virtual std::optional<const Translation> currTranslation() const = 0;
  /**
   * @brief set translation to the one in the settings, update the selected db
   */
  virtual void loadTranslation() = 0;
};

#endif
