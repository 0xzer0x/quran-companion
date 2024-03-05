#ifndef TRANSLATION_H
#define TRANSLATION_H

#include "content.h"
#include <QList>
#include <QSharedPointer>
#include <QString>

class Translation : public Content
{
public:
  static void populateTranslations();
  static QList<Translation> translations;

  bool operator==(const Translation& v2) const;
  bool operator!=(const Translation& v2) const;

  explicit Translation(QString display, QString filename, bool isExtra);
  bool isAvailable() const;
};

#endif // TRANSLATION_H
