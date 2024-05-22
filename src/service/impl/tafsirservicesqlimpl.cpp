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
TafsirServiceSqlImpl::setCurrentTafsir(int idx)
{
  return m_tafsirRepository.setCurrentTafsir(idx);
}

QString
TafsirServiceSqlImpl::getTafsir(const int sIdx, const int vIdx)
{
  return m_tafsirRepository.getTafsir(sIdx, vIdx);
}

const Tafsir*
TafsirServiceSqlImpl::currTafsir() const
{
  return m_tafsirRepository.currTafsir();
}
