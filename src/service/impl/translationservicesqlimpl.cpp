#include "translationservicesqlimpl.h"

TranslationServiceSqlImpl::TranslationServiceSqlImpl()
  : m_translationRepository(TranslationRepository::getInstance())
{
}

bool
TranslationServiceSqlImpl::setCurrentTranslation(int idx)
{
  return m_translationRepository.setCurrentTranslation(idx);
}

QString
TranslationServiceSqlImpl::getTranslation(const int sIdx, const int vIdx) const
{
  return m_translationRepository.getTranslation(sIdx, vIdx);
}

const Translation*
TranslationServiceSqlImpl::currTranslation() const
{
  return m_translationRepository.currTranslation();
}

void
TranslationServiceSqlImpl::loadTranslation()
{
  return m_translationRepository.loadTranslation();
}
