#ifndef VERSEPLAYER_H
#define VERSEPLAYER_H

#include "dbmanager.h"
#include <QApplication>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QDir>
#include <QMediaPlayer>
#include <QObject>

typedef DBManager::Verse Verse;
typedef DBManager::Reciter Reciter;

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
  void fillRecitersList();
  void setVerse(Verse& newVerse);
  QString verseFilename() const;
  bool setVerseFile(const QString& newVerseFilename);
  QString constructVerseFilename();
  void updateSurahVerseCount();
  int surahCount() const;
  QString reciterName();
  Verse activeVerse() const;
  QAudioOutput* getOutput() const;
  QList<Reciter> recitersList() const;

public slots:
  void playBasmalah();
  void nextVerse();
  void playCurrentVerse();
  bool changeReciter(int reciterIdx);
  void verseStateChanged(QMediaPlayer::MediaStatus status);
  void changeUsedAudioDevice(QAudioDevice dev);

signals:
  void surahChanged();
  void verseNoChanged();
  void missingVerseFile(int reciterIdx, int surah);

private:
  int m_reciter = 0;
  int m_surahCount = 0;
  Verse m_activeVerse;
  Reciter m_currentReciter;
  QList<Reciter> m_recitersList;
  QString m_verseFile;
  QDir m_reciterDir;
  QAudioOutput* m_output;
  DBManager* m_dbMgr;
};

#endif // VERSEPLAYER_H
