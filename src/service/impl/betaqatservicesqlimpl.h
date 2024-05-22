#ifndef BETAQATSERVICESQLIMPL_H
#define BETAQATSERVICESQLIMPL_H

#include <repository/betaqatrepository.h>
#include <service/betaqatservice.h>

class BetaqatServiceSqlImpl : public BetaqatService
{
private:
  const BetaqatRepository& m_betaqatRepositroy;

public:
  BetaqatServiceSqlImpl();

  QString getBetaqa(const int surah) const override;
};

#endif // BETAQATSERVICESQLIMPL_H
