/**
 * @file verseplayer.h
 * @brief Header file for the VersePlayer class
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

/**
 * @class VersePlayer
 * @brief Responsible for the playback of Quran verse recitations.
 *
 * This class extends QMediaPlayer and manages the playback of Quranic verses by
 * interacting with audio files. It supports changing reciters, setting audio
 * output devices, and loading verse files for playback.
 */
class VersePlayer : public QMediaPlayer
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a VersePlayer object.
   * @param parent Pointer to the parent QObject.
   * @param reciterIdx Index of the reciter in the reciters list.
   */
  explicit VersePlayer(QObject* parent, int reciterIdx);
  /**
   * @brief Constructs the filename for a given verse.
   * @param v The Verse object to generate the filename for.
   * @return QString containing the constructed filename (e.g., 002005.mp3).
   */
  QString constructVerseFilename(const Verse& v);
  /**
   * @brief Sets the filename of the verse to be loaded and played.
   * @param newVerseFilename The filename of the verse mp3 file.
   * @return Boolean indicating whether the verse file was successfully loaded.
   */
  bool setVerseFile(const QString& newVerseFilename);
  /**
   * @brief Loads the current active verse into the player.
   * @return Boolean indicating whether the active verse was successfully
   * loaded.
   */
  bool loadActiveVerse();
  /**
   * @brief Gets the name of the currently selected reciter.
   * @return QString containing the reciter's display name.
   */
  QString reciterName() const;
  /**
   * @brief Gets the filename of the currently loaded verse.
   * @return QString containing the filename of the verse.
   */
  QString verseFilename() const;
  /**
   * @brief Gets the QAudioOutput object used for controlling audio output.
   * @return Pointer to the QAudioOutput object.
   */
  QAudioOutput* getOutput() const;
  /**
   * @brief Checks if the player is currently active (on).
   * @return Boolean indicating whether the player is on.
   */
  bool isOn() const;

public slots:
  /**
   * @brief Starts playback of the current verse and sets the player state to
   * on.
   */
  void play();
  /**
   * @brief Pauses playback and sets the player state to off.
   */
  void pause();
  /**
   * @brief Stops playback and sets the player state to off.
   */
  void stop();
  /**
   * @brief Plays the mp3 file corresponding to the current active verse.
   */
  void playCurrentVerse();
  /**
   * @brief Changes the currently selected reciter and loads the active verse
   * from the new reciter's directory.
   * @param reciterIdx Index of the new reciter in the reciters list.
   * @return Boolean indicating whether the active verse was successfully loaded
   * from the new reciter.
   */
  bool changeReciter(int reciterIdx);
  /**
   * @brief Changes the audio device used for playback.
   * @param dev The QAudioDevice to use for playback.
   */
  void changeUsedAudioDevice(QAudioDevice dev);
  /**
   * @brief Sets the volume of the audio output.
   * @param volume The volume level (0.0 to 1.0).
   */
  void setPlayerVolume(qreal volume);

signals:
  /**
   * @brief Emitted when a verse file is missing.
   * @param reciterIdx Index of the reciter.
   * @param surah The surah number associated with the missing verse file.
   */
  void missingVerseFile(int reciterIdx, int surah);

private:
  Verse& m_activeVerse; ///< Current active verse.
  QDir m_reciterDir;    ///< Directory of the current reciter's recitations.
  const QList<Reciter>& m_reciters; ///< List of available reciters.
  bool m_isOn = false;              ///< Indicates whether the player is on.
  int m_reciter = 0;   ///< Index of the currently selected reciter.
  QString m_verseFile; ///< Filename of the current verse.
  QPointer<QAudioOutput>
    m_output; ///< Pointer to the QAudioOutput object for playback control.
};

#endif // VERSEPLAYER_H
