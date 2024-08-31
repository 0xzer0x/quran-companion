#include "tafsirservicesqlimpl.h"

TafsirServiceSqlImpl::TafsirServiceSqlImpl()
  : m_tafsirRepository(TafsirRepository::getInstance())
{
}

void
TafsirServiceSqlImpl::loadTafsir()
{
  return m_tafsirRepository.loadTafsir();
}

bool
TafsirServiceSqlImpl::setCurrentTafsir(QString id)
{
  return m_tafsirRepository.setCurrentTafsir(id);
}

QString
TafsirServiceSqlImpl::getTafsir(const int sIdx, const int vIdx)
{
  return m_tafsirRepository.getTafsir(sIdx, vIdx);
}

std::optional<const Tafsir>
TafsirServiceSqlImpl::currTafsir() const
{
  return m_tafsirRepository.currTafsir();
}
