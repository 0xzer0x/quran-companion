#ifndef BETAQAT_SERVICE
#define BETAQAT_SERVICE

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
