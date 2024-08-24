#include "dirmanager.h"
#include <QApplication>
#include <QStandardPaths>

DirManager&
DirManager::getInstance()
{
  static DirManager dirmanager;
  return dirmanager;
}

DirManager::DirManager()
{
  m_downloadsDir.setPath(
    QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) +
    QDir::separator() + "QuranCompanion");
  m_configDir.setPath(
    QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) +
    QDir::separator() + "QuranCompanion");
  m_assetsDir.setPath(QApplication::applicationDirPath() + QDir::separator() +
                      "assets");
  m_fontsDir.setPath(m_assetsDir.absoluteFilePath("fonts"));
  m_basmallahDir.setPath(QApplication::applicationDirPath() +
                         QDir::separator() + "bismillah");
}

void
DirManager::setFontsDir(const QDir& newFontsDir)
{
  m_fontsDir = newFontsDir;
}

void
DirManager::setConfigDir(const QDir& newConfigDir)
{
  m_configDir = newConfigDir;
}

void
DirManager::setAssetsDir(const QDir& newAssetsDir)
{
  m_assetsDir = newAssetsDir;
}

void
DirManager::setDownloadsDir(const QDir& newDownloadsDir)
{
  m_downloadsDir = newDownloadsDir;
}

void
DirManager::setBasmallahDir(const QDir& newBasmallahDir)
{
  m_basmallahDir = newBasmallahDir;
}

const QDir&
DirManager::fontsDir() const
{
  return m_fontsDir;
}

const QDir&
DirManager::configDir() const
{
  return m_configDir;
}

const QDir&
DirManager::assetsDir() const
{
  return m_assetsDir;
}

const QDir&
DirManager::downloadsDir() const
{
  return m_downloadsDir;
}

const QDir&
DirManager::basmallahDir() const
{
  return m_basmallahDir;
}

void
DirManager::createDirSkeleton() const
{
  // config & downloads
  if (!m_configDir.exists())
    m_configDir.mkpath(m_configDir.absolutePath());

  if (!m_downloadsDir.exists())
    m_downloadsDir.mkpath(m_downloadsDir.absolutePath());

  if (!m_downloadsDir.exists("recitations"))
    m_downloadsDir.mkpath("recitations");

  if (!m_downloadsDir.exists("QCFV2"))
    m_downloadsDir.mkpath("QCFV2");

  if (!m_downloadsDir.exists("tafasir"))
    m_downloadsDir.mkpath("tafasir");

  if (!m_downloadsDir.exists("translations"))
    m_downloadsDir.mkpath("translations");
}
