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

  bool setCurrentTranslation(QString id) override;

  QString getTranslation(const int sIdx, const int vIdx) const override;

  std::optional<const Translation> currTranslation() const override;

  void loadTranslation() override;
};

#endif // TRANSLATIONSERVICESQLIMPL_H
