#ifndef USERDATAEXPORTER_H
#define USERDATAEXPORTER_H

#include <QObject>
#include <QString>

class UserDataExporter : public QObject
{
  Q_OBJECT
public:
  enum Error
  {
    IOError
  };
  virtual void setFile(QString path) = 0;
  virtual void exportBookmarks() = 0;
  virtual void exportKhatmah() = 0;
  virtual void exportThoughts() = 0;
  virtual bool save() = 0;

signals:
  void error(Error err);
};

#endif // USERDATAEXPORTER_H
