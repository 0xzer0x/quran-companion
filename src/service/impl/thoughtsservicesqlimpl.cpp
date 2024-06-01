#include "thoughtsservicesqlimpl.h"

ThoughtsServiceSqlImpl::ThoughtsServiceSqlImpl()
  : m_thoughtsRepository(BookmarksRepository::getInstance())
{
}

void
ThoughtsServiceSqlImpl::saveThoughts(Verse& verse, const QString& text)
{
  return m_thoughtsRepository.saveThoughts(verse, text);
}

QString
ThoughtsServiceSqlImpl::getThoughts(const Verse& verse) const
{
  return m_thoughtsRepository.getThoughts(verse);
}

QList<QPair<Verse, QString>>
ThoughtsServiceSqlImpl::allThoughts() const
{
  return m_thoughtsRepository.allThoughts();
}
