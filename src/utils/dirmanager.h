#ifndef DIRMANAGER_H
#define DIRMANAGER_H

#include <QDir>
#include <QSharedPointer>

class DirManager
{
public:
  static DirManager& getInstance();

  void setFontsDir(const QDir& newFontsDir);
  void setConfigDir(const QDir& newConfigDir);
  void setAssetsDir(const QDir& newAssetsDir);
  void setDownloadsDir(const QDir& newDownloadsDir);
  void setBasmallahDir(const QDir& newBasmallahDir);

  const QDir& fontsDir() const;
  const QDir& configDir() const;
  const QDir& assetsDir() const;
  const QDir& downloadsDir() const;
  const QDir& basmallahDir() const;

  void createDirSkeleton() const;

private:
  DirManager();
  QDir m_fontsDir;
  QDir m_configDir;
  QDir m_assetsDir;
  QDir m_downloadsDir;
  QDir m_basmallahDir;
};

#endif // DIRMANAGER_H
