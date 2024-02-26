#ifndef USERDATAEXPORTER_H
#define USERDATAEXPORTER_H

#include <QString>

class UserDataExporter
{
public:
  virtual void setFile(QString path) = 0;
  virtual void exportBookmarks() = 0;
  virtual void exportKhatmah() = 0;
  virtual void exportThoughts() = 0;
  virtual bool save() = 0;
};

#endif // USERDATAEXPORTER_H
