#ifndef TRANSLATIONSERVICESQLIMPL_H
#define TRANSLATIONSERVICESQLIMPL_H

#include <repository/translationrepository.h>
#include <service/translationservice.h>

class TranslationServiceSqlImpl : public TranslationService
{
  Q_OBJECT
private:
  TranslationRepository& m_translationRepository;

public:
  TranslationServiceSqlImpl();

  bool setCurrentTranslation(int idx) override;

  QString getTranslation(const int sIdx, const int vIdx) const override;

  const Translation* currTranslation() const override;

  void loadTranslation() override;
};

#endif // TRANSLATIONSERVICESQLIMPL_H
