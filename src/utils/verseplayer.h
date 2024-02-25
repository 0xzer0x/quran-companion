/**
 * @file verseplayer.h
 * @brief Header file for VersePlayer
 */

#ifndef VERSEPLAYER_H
#define VERSEPLAYER_H

#include <QApplication>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QDir>
#include <QMediaPlayer>
#include <QObject>
#include <QPointer>
#include <types/reciter.h>
#include <types/verse.h>

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
   * @param initVerse - inital ::Verse to load recitation for
   * @param reciterIdx - ::Globals::recitersList index for the reciter
   */
  explicit VersePlayer(QObject* parent, int reciterIdx);

  /**
   * @brief construct the verse filename for the ::Verse given using the
   * surah and verse number (002005.mp3)
   * @param v - ::Verse to get the filename of
   * @return QString of the filename
   */
  QString constructVerseFilename(const QSharedPointer<Verse> v);
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
   * @brief getter for m_output, used for controlling audio output device
   * @return pointer to the used QAudioOutput object
   */
  QAudioOutput* getOutput() const;
  /**
   * @brief getter for the player m_isOn attribute
   * @return boolean indicating whether the player is on
   */
  bool isOn() const;

public slots:
  /**
   * @brief re-implementation of QMediaPlayer::play() in order to set the m_isOn
   * variable when manually calling play()
   */
  void play();
  /**
   * @brief re-implementation of QMediaPlayer::pause() in order to set the
   * m_isOn variable when manually calling pause()
   */
  void pause();
  /**
   * @brief re-implementation of QMediaPlayer::stop() in order to set the m_isOn
   * variable when manually calling stop()
   */
  void stop();
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
  /**
   * @brief change the QAudioDevice used for playback
   * @param dev - QAudioDevice to use for playback
   */
  void changeUsedAudioDevice(QAudioDevice dev);
  /**
   * @brief sets the current playback volume
   * @param volume - float value for volume (0-1.0)
   */
  void setPlayerVolume(qreal volume);

signals:
  void missingVerseFile(int reciterIdx, int surah);

private:
  QSharedPointer<Verse> m_activeVerse = Verse::current();
  QDir m_reciterDir = DirManager::downloadsDir->absoluteFilePath("recitations");
  const QList<QSharedPointer<Reciter>>& m_recitersList = Reciter::reciters;
  /**
   * @brief boolean indicating whether the player is on or off, 'on' implies
   * that playback should continue in case of verse change
   */
  bool m_isOn = false;
  /**
   * @brief ::Globals::recitersList index for the reciter
   */
  int m_reciter = 0;
  /**
   * @brief current verse mp3 filename
   */
  QString m_verseFile;
  /**
   * @brief QAudioOutput used for playback
   */
  QPointer<QAudioOutput> m_output;
};

#endif // VERSEPLAYER_H
