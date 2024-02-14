/**
 * @file verseplayer.cpp
 * @brief Implementation file for VersePlayer
 */

#include "verseplayer.h"

VersePlayer::VersePlayer(QObject* parent, int reciterIdx)
  : QMediaPlayer(parent)
  , m_reciter(reciterIdx)
  , m_output(new QAudioOutput(this))
{
  setAudioOutput(m_output);

  m_reciterDir.cd(m_recitersList.at(m_reciter).baseDirName);
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
VersePlayer::constructVerseFilename(const Verse* v)
{
  // construct verse mp3 filename e.g. 002005.mp3
  QString filename;
  filename.append(QString::number(v->surah()).rightJustified(3, '0'));
  filename.append(QString::number(v->number()).rightJustified(3, '0'));

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
VersePlayer::changeReciter(int reciterIdx)
{
  if (m_activeVerse->number() == 0)
    m_activeVerse->setNumber(1);

  stop();
  if (reciterIdx != m_reciter) {
    m_reciterDir.cdUp();
    m_reciterDir.cd(m_recitersList.at(reciterIdx).baseDirName);
    m_reciter = reciterIdx;
  }

  return loadActiveVerse();
}

bool
VersePlayer::setVerseFile(const QString& newVerseFilename)
{
  if (!m_reciterDir.exists(newVerseFilename)) {
    setSource(QUrl());
    qDebug() << "file " + newVerseFilename + " is missing.";
    emit missingVerseFile(m_reciter, m_activeVerse->surah());
    return false;
  }

  m_verseFile = newVerseFilename;
  setSource(QUrl::fromLocalFile(m_reciterDir.filePath(m_verseFile)));

  return true;
}

bool
VersePlayer::loadActiveVerse()
{
  if (m_activeVerse->number() == 0) {
    setSource(QUrl::fromLocalFile(m_recitersList.at(m_reciter).basmallahPath));
    return true;
  }

  return setVerseFile(constructVerseFilename(m_activeVerse));
}

QString
VersePlayer::reciterName() const
{
  return m_recitersList.at(m_reciter).displayName;
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
