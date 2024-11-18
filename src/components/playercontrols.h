#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include "quranreader.h"
#include <QWidget>
#include <player/playbackcontroller.h>
#include <player/verseplayer.h>
#include <types/verse.h>
#include <widgets/repeaterpopup.h>

namespace Ui {
class PlayerControls;
}

/**
 * @class PlayerControls
 * @brief PlayerControls class provides a widget for controlling the
 * playback of audio verses from the Quran.
 */
class PlayerControls : public QWidget
{
  Q_OBJECT

public:
  explicit PlayerControls(QWidget* parent,
                          QPointer<PlaybackController> playbackController,
                          QPointer<QuranReader> reader,
                          QPointer<RepeaterPopup> repeater);
  ~PlayerControls();

  /**
   * @brief getter for the index of the currently selected reciter in the
   * reciters combobox
   * @return index of reciter in the combobox
   */
  int currentReciter() const;

public slots:
  /**
   * @brief toggle play/pause of the current verse
   */
  void togglePlayback();
  void adjustWidth();

protected:
  void hideEvent(QHideEvent* event);

private slots:
  /**
   * @brief sets the current position in the audio file as the position of the
   * slider
   * @param position - position in audio file in milliseconds
   */
  void mediaPosChanged(qint64 position);
  /**
   * @brief continue playback of the current verse
   */
  void btnPlayClicked();
  /**
   * @brief pause playback of the current verse
   */
  void btnPauseClicked();
  /**
   * @brief stops playback, sets the current verse to the 1st in the page and
   * update verses combobox & selected surah
   */
  void btnStopClicked();
  /**
   * @brief callback for updating the selected reciter
   * @param newIndex
   */
  void cmbReciterChanged(int newIndex);
  /**
   * @brief disables/enables control buttons according to the media player state
   * and update the systray tooltip
   * @param state - playback state of the current audio file
   */
  void mediaStateChanged(QMediaPlayer::PlaybackState state);
  /**
   * @brief change the player volume level as the volume slider changes
   * @param position - position in the slider (0 - 100)
   */
  void volumeSliderValueChanged(int position);
  /**
   * @brief utility to increment the VersePlayer playback volume by steps of 5
   */
  void incrementVolume();
  /**
   * @brief utility to decrement the VersePlayer playback volume by steps of 5
   */
  void decrementVolume();
  /**
   * @brief callback for toggling the repeater dialog
   * @param on - boolean indicating the state of the button
   */
  void btnRepeatClicked(bool on);

private:
  Ui::PlayerControls* ui;
  /**
   * @brief reference to the shared current verse instance
   */
  const Verse& m_currVerse;
  /**
   * @brief reference to the singleton Configuration instance
   */
  Configuration& m_config;
  /**
   * @brief reference to the static QList of available reciters
   */
  const QList<Reciter>& m_reciters;
  /**
   * @brief load icons for different UI elements
   */
  void loadIcons();
  /**
   * @brief connect ShortcutHandler signals to their corresponding slots
   */
  void setupConnections();
  /**
   * @brief float value of the current playback volume (0 - 1.0)
   */
  qreal m_volume = 1;
  /**
   * @brief pointer to VersePlayer instance
   */
  QPointer<PlaybackController> m_playbackController;
  /**
   * @brief pointer to the QuranReader instance
   */
  QPointer<QuranReader> m_reader;
  /**
   * @brief pointer to the RepeaterPopup instance
   */
  QPointer<RepeaterPopup> m_repeater;
};

#endif // PLAYERCONTROLS_H
