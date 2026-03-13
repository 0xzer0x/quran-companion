#include "translationservicesqlimpl.h"

TranslationServiceSqlImpl::TranslationServiceSqlImpl()
  : m_translationRepository(TranslationRepository::getInstance())
{
}

bool
TranslationServiceSqlImpl::setCurrentTranslation(Translation translation)
{
  return m_translationRepository.setCurrentTranslation(translation);
}

QString
TranslationServiceSqlImpl::getTranslation(const int sIdx, const int vIdx) const
{
  return m_translationRepository.getTranslation(sIdx, vIdx);
}

const Translation
TranslationServiceSqlImpl::currTranslation() const
{
  return m_translationRepository.currTranslation();
}

void
TranslationServiceSqlImpl::loadTranslation()
{
  return m_translationRepository.loadTranslation();
}
