#ifndef TAFSIRSERVICESQLIMPL_H
#define TAFSIRSERVICESQLIMPL_H

#include <repository/tafsirrepository.h>
#include <service/tafsirservice.h>

class TafsirServiceSqlImpl : public TafsirService
{
private:
  TafsirRepository& m_tafsirRepository;

public:
  TafsirServiceSqlImpl();

  void loadTafsir() override;

  bool setCurrentTafsir(QString id) override;

  QString getTafsir(const int sIdx, const int vIdx) override;

  std::optional<const Tafsir> currTafsir() const override;
};

#endif // TAFSIRSERVICESQLIMPL_H
