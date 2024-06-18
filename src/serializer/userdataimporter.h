#ifndef USERDATAIMPORTER_H
#define USERDATAIMPORTER_H

#include <QObject>
#include <QString>

class UserDataImporter : public QObject
{
  Q_OBJECT
public:
  enum Error
  {
    IOError,
    ParseError,
    MissingKeyError,
    InvalidValueError
  };
  virtual void importBookmarks() = 0;
  virtual void importKhatmah() = 0;
  virtual void importThoughts() = 0;
  virtual void setFile(QString path) = 0;
  virtual bool fileContains(QString key) = 0;
  virtual bool read() = 0;
signals:
  void error(Error err, QString msg);
};

#endif // USERDATAIMPORTER_H
