/**
 * @file verseplayer.cpp
 * @brief Implementation file for VersePlayer
 */

#include "verseplayer.h"
#include <QFileInfo>
#include <player/impl/continuousplaybackstrategy.h>
#include <utils/dirmanager.h>

VersePlayer::VersePlayer(QObject* parent)
  : QMediaPlayer(parent)
  , m_config(Configuration::getInstance())
  , m_output(new QAudioOutput(this))
  , m_activeVerse(Verse::getCurrent())
  , m_reciter(&m_config.reciter())
  , m_reciterDir(
      DirManager::getInstance().downloadsDir().absoluteFilePath("recitations"))
{
  setAudioOutput(m_output);

  // WARN: Make sure passed m_reciter is not a nullptr before passing
  if (m_reciter == nullptr) {
    qFatal() << "Invalid value for reciter pointer in verse player:"
             << m_reciter;
  }

  m_reciterDir.cd(m_reciter->baseDirName());
  loadActiveVerse();
}

bool
VersePlayer::isOn() const
{
  return m_isOn;
}

void
VersePlayer::play()
{
  m_isOn = true;
  QMediaPlayer::play();
}

void
VersePlayer::pause()
{
  m_isOn = false;
  QMediaPlayer::pause();
}

void
VersePlayer::stop()
{
  m_isOn = false;
  QMediaPlayer::stop();
}

void
VersePlayer::changeUsedAudioDevice(QAudioDevice dev)
{
  m_output->setDevice(dev);
  setAudioOutput(m_output);
}

void
VersePlayer::setPlayerVolume(qreal volume)
{
  m_output->setVolume(volume);
}

QString
VersePlayer::constructVerseFilename(const Verse& v)
{
  // construct verse mp3 filename e.g. 002005.mp3
  QString filename;
  filename.append(QString::number(v.surah()).rightJustified(3, '0'));
  filename.append(QString::number(v.number()).rightJustified(3, '0'));

  filename.append(".mp3");
  return filename;
}

void
VersePlayer::playCurrentVerse()
{
  if (loadActiveVerse())
    play(); // start playback of audio
}

bool
VersePlayer::changeReciter(const Reciter* reciter)
{
  if (m_activeVerse.number() == 0) {
    m_activeVerse.setNumber(1);
  }

  pause();
  if (reciter != m_reciter && reciter != nullptr) {
    m_reciterDir.cdUp();
    m_reciterDir.cd(reciter->baseDirName());
    m_reciter = reciter;
  }

  return loadActiveVerse();
}

bool
VersePlayer::setVerseFile(const QString& newVerseFilename)
{
  setSource(QUrl());

  QFileInfo verseFile(m_reciterDir.filePath(newVerseFilename));
  if (!verseFile.exists() || verseFile.size() == 0) {
    qDebug() << "file " + newVerseFilename + " is missing.";
    emit missingVerseFile(m_reciter, m_activeVerse.surah());
    return false;
  }

  m_verseFile = newVerseFilename;
  setSource(QUrl::fromLocalFile(verseFile.absoluteFilePath()));

  return true;
}

bool
VersePlayer::loadActiveVerse()
{
  if (m_activeVerse.number() == 0) {
    setSource(QUrl::fromLocalFile(m_reciter->basmallahPath()));
    return true;
  }

  return setVerseFile(constructVerseFilename(m_activeVerse));
}

QString
VersePlayer::reciterName() const
{
  return m_reciter->displayName();
}

QAudioOutput*
VersePlayer::getOutput() const
{
  return m_output;
}

QString
VersePlayer::verseFilename() const
{
  return m_verseFile;
}
