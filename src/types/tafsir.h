#ifndef TAFSIR_H
#define TAFSIR_H

#include "content.h"
#include <QList>
#include <QSharedPointer>
#include <QString>

class Tafsir : public Content
{
public:
  static void populateTafasir();
  static std::optional<Tafsir> findById(QString id);
  static QList<Tafsir> tafasir;

  Tafsir& operator=(const Tafsir& other);
  bool operator==(const Tafsir& v2) const;
  bool operator!=(const Tafsir& v2) const;

  explicit Tafsir(QString id,
                  QString display,
                  QString filename,
                  bool isText,
                  bool isExtra);
  const bool isText() const;
  bool isAvailable() const;

private:
  bool m_isText;
};

#endif // TAFSIR_H
