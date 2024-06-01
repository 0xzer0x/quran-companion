#ifndef KHATMAHSERVICESQLIMPL_H
#define KHATMAHSERVICESQLIMPL_H

#include <repository/bookmarksrepository.h>
#include <service/khatmahservice.h>

class KhatmahServiceSqlImpl : public KhatmahService
{
private:
  BookmarksRepository& m_khatmahRepository;

public:
  KhatmahServiceSqlImpl();

  std::optional<Verse> loadVerse(const int khatmahId) const override;

  bool saveActiveKhatmah(const Verse& verse) override;

  QList<int> getAllKhatmah() const override;

  QString getKhatmahName(const int id) const override;

  int addKhatmah(const Verse& verse,
                 const QString name,
                 const int id) const override;

  bool editKhatmahName(const int khatmahId, QString newName) override;

  void removeKhatmah(const int id) const override;

  void setActiveKhatmah(const int id) override;

  int activeKhatmah() const override;
};

#endif // KHATMAHSERVICESQLIMPL_H
