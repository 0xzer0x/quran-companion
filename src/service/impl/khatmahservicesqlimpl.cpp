#include "khatmahservicesqlimpl.h"

KhatmahServiceSqlImpl::KhatmahServiceSqlImpl()
  : m_khatmahRepository(BookmarksRepository::getInstance())
{
}

std::optional<Verse>
KhatmahServiceSqlImpl::loadVerse(const int khatmahId) const
{
  return m_khatmahRepository.loadVerse(khatmahId);
}

bool
KhatmahServiceSqlImpl::saveActiveKhatmah(const Verse& verse)
{
  return m_khatmahRepository.saveActiveKhatmah(verse);
}

QList<int>
KhatmahServiceSqlImpl::getAllKhatmah() const
{
  return m_khatmahRepository.getAllKhatmah();
}

QString
KhatmahServiceSqlImpl::getKhatmahName(const int id) const
{
  return m_khatmahRepository.getKhatmahName(id);
}

int
KhatmahServiceSqlImpl::addKhatmah(const Verse& verse,
                                  const QString name,
                                  const int id) const
{
  return m_khatmahRepository.addKhatmah(verse, name, id);
}

bool
KhatmahServiceSqlImpl::editKhatmahName(const int khatmahId, QString newName)
{
  return m_khatmahRepository.editKhatmahName(khatmahId, newName);
}

void
KhatmahServiceSqlImpl::removeKhatmah(const int id) const
{
  return m_khatmahRepository.removeKhatmah(id);
}

void
KhatmahServiceSqlImpl::setActiveKhatmah(const int id)
{
  return m_khatmahRepository.setActiveKhatmah(id);
}

int
KhatmahServiceSqlImpl::activeKhatmah() const
{
  return m_khatmahRepository.activeKhatmah();
}
