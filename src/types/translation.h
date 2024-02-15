#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QList>
#include <QSharedPointer>
#include <QString>

class Translation
{
public:
  static QList<QSharedPointer<Translation>> translations;
  static void populateTranslations();

  static bool translationExists(int idx);
  static bool translationExists(const QSharedPointer<Translation>& tr);

  explicit Translation(QString display, QString filename, bool isExtra);

  const QString& displayName() const;
  const QString& filename() const;
  const bool& isExtra() const;

private:
  QString m_displayName;
  QString m_filename;
  bool m_isExtra;
};

#endif // TRANSLATION_H
