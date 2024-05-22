#ifndef THOUGHTSSERVICESQLIMPL_H
#define THOUGHTSSERVICESQLIMPL_H

#include <repository/bookmarksrepository.h>
#include <service/thoughtsservice.h>

class ThoughtsServiceSqlImpl : public ThoughtsService
{
private:
    BookmarksRepository& m_thoughtsRepository;

public:
  ThoughtsServiceSqlImpl();

  void saveThoughts(Verse& verse, const QString& text);

  QString getThoughts(const Verse& verse) const;

  QList<QPair<Verse, QString>> allThoughts() const;
};

#endif // THOUGHTSSERVICESQLIMPL_H
