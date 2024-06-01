#ifndef BETAQATSERVICE_H
#define BETAQATSERVICE_H

#include <QString>

class BetaqatService
{
public:
  /**
   * @brief get the surah card (betaqa) content
   * @param surah - surah number
   * @return QString of the surah card text
   */
  virtual QString getBetaqa(const int surah) const = 0;
};

#endif
