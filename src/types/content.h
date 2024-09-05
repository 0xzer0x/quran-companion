#ifndef CONTENT_H
#define CONTENT_H

#include <QString>

class Content
{
public:
  explicit Content(QString id, QString display, QString filename, bool isExtra);
  const QString& id() const;
  const QString& displayName() const;
  const QString& filename() const;
  const bool& isExtra() const;
  virtual bool isAvailable() const = 0;

protected:
  QString m_id;
  QString m_displayName;
  QString m_filename;
  bool m_isExtra;
};

#endif // CONTENT_H
