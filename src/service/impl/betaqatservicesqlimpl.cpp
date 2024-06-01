#include "betaqatservicesqlimpl.h"

BetaqatServiceSqlImpl::BetaqatServiceSqlImpl()
  : m_betaqatRepositroy(BetaqatRepository::getInstance())
{
}

QString
BetaqatServiceSqlImpl::getBetaqa(const int surah) const
{
  return m_betaqatRepositroy.getBetaqa(surah);
}
