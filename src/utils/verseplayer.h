#ifndef VERSEPLAYER_H
#define VERSEPLAYER_H

#include "../globals.h"
#include "dbmanager.h"
#include <QApplication>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QDir>
#include <QMediaPlayer>
#include <QObject>

/*!
 * \class VersePlayer
 *
 * \brief The VersePlayer class is responsible for the playback of verses audio
 * files
 */
class VersePlayer : public QMediaPlayer
{
  Q_OBJECT

public:
  explicit VersePlayer(QObject* parent = nullptr,
                       DBManager* dbPtr = nullptr,
                       Verse initVerse = Verse{},
                       int reciterIdx = 0);

  QString constructVerseFilename();
  void updateSurahVerseCount();

  void setVerse(Verse& newVerse);
  bool setVerseFile(const QString& newVerseFilename);

  QString reciterName() const;
  QString verseFilename() const;
  int surahCount() const;
  Verse activeVerse() const;
  QAudioOutput* getOutput() const;

public slots:
  void playBasmalah();
  void nextVerse();
  void playCurrentVerse();
  bool changeReciter(int reciterIdx);
  void verseStateChanged(QMediaPlayer::MediaStatus status);
  void changeUsedAudioDevice(QAudioDevice dev);
  void setPlayerVolume(qreal volume);

signals:
  void surahChanged();
  void verseNoChanged();
  void missingVerseFile(int reciterIdx, int surah);

private:
  QDir m_reciterDir = Globals::recitationsDir;
  const QList<Reciter>& m_recitersList = Globals::recitersList;
  void setupConnections();
  int m_reciter = 0;
  int m_surahCount = 0;
  Verse m_activeVerse;
  Reciter m_currentReciter;
  QString m_verseFile;
  QAudioOutput* m_output;
  DBManager* m_dbMgr;
};

#endif // VERSEPLAYER_H
