#ifndef TAFSIR_H
#define TAFSIR_H

#include <QList>
#include <QSharedPointer>
#include <QString>

class Tafsir
{
public:
  static void populateTafasir();
  static QList<QSharedPointer<Tafsir>> tafasir;

  static bool tafsirExists(int idx);
  static bool tafsirExists(const QSharedPointer<Tafsir>& t);

  explicit Tafsir(QString display, QString filename, bool isText, bool isExtra);

  const QString& displayName() const;
  const QString& filename() const;
  const bool isText() const;
  const bool isExtra() const;

private:
  QString m_displayName;
  QString m_filename;
  bool m_isText;
  bool m_isExtra;
};

#endif // TAFSIR_H
