#ifndef USERDATAIMPORTER_H
#define USERDATAIMPORTER_H

#include <QString>

class UserDataImporter
{
public:
  virtual void setFile(QString path) = 0;
  virtual void importBookmarks() = 0;
  virtual void importKhatmah() = 0;
  virtual void importThoughts() = 0;
};

#endif // USERDATAIMPORTER_H
