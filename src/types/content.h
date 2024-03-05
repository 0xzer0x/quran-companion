#ifndef CONTENT_H
#define CONTENT_H

#include <QString>

class Content
{
public:
  explicit Content(QString display, QString filename, bool isExtra);
  const QString& displayName() const;
  const QString& filename() const;
  const bool& isExtra() const;
  virtual bool isAvailable() const = 0;

private:
  QString m_displayName;
  QString m_filename;
  bool m_isExtra;
};

#endif // CONTENT_H
