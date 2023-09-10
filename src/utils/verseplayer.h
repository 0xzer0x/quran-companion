/**
 * @file verseplayer.h
 * @brief Header file for VersePlayer
 */

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
 * @brief VersePlayer class is responsible for the playback of Quran verse
 * recitations
 */
class VersePlayer : public QMediaPlayer
{
  Q_OBJECT

public:
  /**
   * @brief Class constructor
   * @param parent - pointer to parent widget
   * @param dbPtr - pointer to DBManager instance
   * @param initVerse - inital ::Verse to load recitation for
   * @param reciterIdx - chosen reciter to load recitation of
   */
  explicit VersePlayer(QObject* parent = nullptr,
                       Verse initVerse = Verse{},
                       int reciterIdx = 0);

  /**
   * @brief construct the verse filename for the ::Verse given using the
   * surah and verse number (002005.mp3)
   * @param v - ::Verse to get the filename of
   * @return QString of the filename
   */
  QString constructVerseFilename(Verse v);
  /**
   * @brief updates m_surahCount to match the verse count of the currently
   * active verse
   */
  void updateSurahVerseCount();

  /**
   * @brief setter for the m_activeVerse member
   * @param newVerse - ::Verse instance
   */
  void setVerse(Verse& newVerse);
  /**
   * @brief load the verse mp3 from the current reciter directory and set the
   * m_verseFile variable
   * @param newVerseFilename - verse filename to load
   * @return boolean value indicating successful load
   */
  bool setVerseFile(const QString& newVerseFilename);
  /**
   * @brief utility function that provides a convinent interface for loading the
   * current m_activeVerse
   * @return boolean indicating successful load
   */
  bool loadActiveVerse();

  /**
   * @brief getter for the currently set reciter name
   * @return QString containing the reciter's display name
   */
  QString reciterName() const;
  /**
   * @brief getter for the currently loaded verse filename
   * @return QString containing the verse filename
   */
  QString verseFilename() const;
  /**
   * @brief getter for m_surahCount
   * @return number of verses in the current surah
   */
  int surahCount() const;
  /**
   * @brief getter for m_activeVerse
   * @return ::Verse instance
   */
  Verse activeVerse() const;
  /**
   * @brief getter for m_output, used for controlling audio output device
   * @return pointer to the used QAudioOutput object
   */
  QAudioOutput* getOutput() const;

public slots:
  /**
   * @brief set the active ::Verse number according to the surah and set
   * the current source as the bismillah file for the chosen reciter and start
   * playback
   */
  void playBasmalah();
  /**
   * @brief increment the verse & surah variables appropriately according to the
   * surah, emits a signal on verse change & on changing from surah to another
   */
  void nextVerse();
  /**
   * @brief plays the mp3 file corresponding to m_activeVerse
   */
  void playCurrentVerse();
  /**
   * @brief changes the currently selected reciter
   * @param reciterIdx - ::Globals::recitersList index for the reciter
   * @return boolean indicating successful load of the current verse from the
   * new reciter's directory
   */
  bool changeReciter(int reciterIdx);
  /*!
   * @brief slot to call the nextVerse() method on verse audio end
   * @param status - status of the media file, refer to QMediaPlayer
   * docs for enum.
   */
  void verseStateChanged(QMediaPlayer::MediaStatus status);
  /**
   * @brief change the QAudioDevice used for playback
   * @param dev - QAudioDevice to use for playback
   */
  void changeUsedAudioDevice(QAudioDevice dev);
  /**
   * @brief sets the current playback volume
   * @param volume - float value for volume (0-1)
   */
  void setPlayerVolume(qreal volume);

signals:
  void surahChanged();
  void verseNoChanged();
  void missingVerseFile(int reciterIdx, int surah);

private:
  QDir m_reciterDir = Globals::recitationsDir;
  const QList<Reciter>& m_recitersList = Globals::recitersList;
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief ::Globals::recitersList index for the reciter
   */
  int m_reciter = 0;
  /**
   * @brief verse count for the current surah being played
   */
  int m_surahCount = 0;
  /**
   * @brief ::Verse instance representing the current verse being played
   */
  Verse m_activeVerse;
  /**
   * @brief current verse mp3 filename
   */
  QString m_verseFile;
  /**
   * @brief QAudioOutput used for playback
   */
  QAudioOutput* m_output;
};

#endif // VERSEPLAYER_H
