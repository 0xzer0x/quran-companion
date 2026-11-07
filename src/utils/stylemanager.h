#ifndef STYLEMANAGER_H
#define STYLEMANAGER_H

#include "configuration.h"
#include <QDir>
#include <QPointer>
#include <QSharedPointer>
#include <QtAwesome.h>

class StyleManager
{
public:
  static StyleManager& getInstance();

  void loadTheme();
  static QPalette themePalette(int themeId);
  fa::QtAwesome& awesome();
  const QDir& themeResources() const;

private:
  StyleManager();
  const Configuration& m_config;
  QPointer<fa::QtAwesome> m_awesome;
  QDir m_themeResources;
};

#endif // STYLEMANAGER_H
