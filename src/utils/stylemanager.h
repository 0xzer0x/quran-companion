#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include <QDir>
#include <QSharedPointer>
#include <QtAwesome.h>

class StyleManager
{
public:
  static QSharedPointer<fa::QtAwesome> awesome;
  static QDir themeResources;
  static void loadTheme();
};

#endif // STYLEMANAGER_H
