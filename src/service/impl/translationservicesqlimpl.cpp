#include "translationservicesqlimpl.h"

TranslationServiceSqlImpl::TranslationServiceSqlImpl()
  : m_translationRepository(TranslationRepository::getInstance())
{
}

bool
TranslationServiceSqlImpl::setCurrentTranslation(QString id)
{
  return m_translationRepository.setCurrentTranslation(id);
}

QString
TranslationServiceSqlImpl::getTranslation(const int sIdx, const int vIdx) const
{
  return m_translationRepository.getTranslation(sIdx, vIdx);
}

std::optional<const Translation>
TranslationServiceSqlImpl::currTranslation() const
{
  return m_translationRepository.currTranslation();
}

void
TranslationServiceSqlImpl::loadTranslation()
{
  return m_translationRepository.loadTranslation();
}
