/**
 * @file verseplayer.cpp
 * @brief Implementation file for VersePlayer
 */

#include "verseplayer.h"

VersePlayer::VersePlayer(QObject* parent, Verse initVerse, int reciterIdx)
  : QMediaPlayer(parent)
  , m_activeVerse{ initVerse }
  , m_reciter{ reciterIdx }
  , m_output{ new QAudioOutput(this) }
{
  updateSurahVerseCount();
  setAudioOutput(m_output);
  setupConnections();

  m_reciterDir.cd(m_recitersList.at(m_reciter).baseDirName);
  loadActiveVerse();
}

void
VersePlayer::setupConnections()
{
  // Connectors
  connect(this,
          &QMediaPlayer::mediaStatusChanged,
          this,
          &VersePlayer::verseStateChanged,
          Qt::UniqueConnection);

  connect(this,
          &VersePlayer::verseNoChanged,
          this,
          &VersePlayer::playCurrentVerse,
          Qt::UniqueConnection);
}

void
VersePlayer::setVerse(Verse& newVerse)
{
  m_activeVerse = newVerse;
}

void
VersePlayer::nextVerse()
{
  // if last verse in surah, play basmalah before playing first verse
  if (m_activeVerse.number == m_surahCount) {
    // if last verse in surah an-nas (114), do nothing (i.e stop playback)
    if (m_activeVerse.surah < 114) {
      m_activeVerse.surah++;
      emit surahChanged(); // signals surah change
    }

  } else {
    m_activeVerse.number++;
    emit verseNoChanged(); // signals active verse change
  }
}

void
VersePlayer::verseStateChanged(QMediaPlayer::MediaStatus status)
{
  if (status == QMediaPlayer::EndOfMedia)
    nextVerse();
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
VersePlayer::constructVerseFilename(Verse v)
{
  // construct verse mp3 filename e.g. 002005.mp3
  QString filename;
  filename.append(QString::number(v.surah).rightJustified(3, '0'));
  filename.append(QString::number(v.number).rightJustified(3, '0'));

  filename.append(".mp3");
  return filename;
}

void
VersePlayer::playCurrentVerse()
{
  if (loadActiveVerse())
    play(); // start playback of audio
}

void
VersePlayer::playBasmalah()
{
  m_activeVerse.number = 0;

  // no basmalah with surah al-tawbah (9)
  if (m_activeVerse.surah == 9) {
    nextVerse();
  } else {
    if (m_activeVerse.surah == 1)
      m_activeVerse.number = 1;

    setSource(QUrl::fromLocalFile(m_recitersList.at(m_reciter).basmallahPath));
    play();
  }
}

bool
VersePlayer::changeReciter(int reciterIdx)
{
  if (m_activeVerse.number == 0)
    m_activeVerse.number = 1;

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
    emit missingVerseFile(m_reciter, m_activeVerse.surah);
    return false;
  }

  m_verseFile = newVerseFilename;
  setSource(QUrl::fromLocalFile(m_reciterDir.filePath(m_verseFile)));

  return true;
}

bool
VersePlayer::loadActiveVerse()
{
  return setVerseFile(constructVerseFilename(m_activeVerse));
}

void
VersePlayer::updateSurahVerseCount()
{
  m_surahCount = m_dbMgr->getSurahVerseCount(m_activeVerse.surah);
}

/* -------------------- Getters ----------------------- */

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

int
VersePlayer::surahCount() const
{
  return m_surahCount;
}

Verse
VersePlayer::activeVerse() const
{
  return m_activeVerse;
}
