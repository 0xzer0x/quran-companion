#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include "quranreader.h"
#include "types/verse.h"
#include "utils/verseplayer.h"
#include <QWidget>

namespace Ui {
class PlayerControls;
}

class PlayerControls : public QWidget
{
  Q_OBJECT

public:
  explicit PlayerControls(QWidget* parent,
                          VersePlayer* player,
                          QuranReader* reader);
  ~PlayerControls();

  int currentReciter() const;

public slots:
  /**
   * @brief toggle play/pause of the current verse
   */
  void togglePlayback();

signals:
  void currentVerseChanged();
  void currentSurahChanged();

private slots:
  /**
   * @brief sets the current position in the audio file as the position of the
   * slider
   * @param position - position in audio file in milliseconds
   */
  void mediaPosChanged(qint64 position);
  /**
   * @brief continues playback of the current verse
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
   * @brief toggle the visibility of the player controls in the main window
   */
  void toggleVisibility();
  /**
   * @brief utility to increment the VersePlayer playback volume by steps of 5
   */
  void incrementVolume();
  /**
   * @brief utility to decrement the VersePlayer playback volume by steps of 5
   */
  void decrementVolume();

private:
  Ui::PlayerControls* ui;
  QSharedPointer<Verse> m_currVerse = Verse::current();
  QSharedPointer<DBManager> m_dbMgr = DBManager::current();
  QSharedPointer<QSettings> const m_settings = Settings::settings;
  const QList<QSharedPointer<Reciter>>& m_reciters = Reciter::reciters;
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
  QPointer<VersePlayer> m_player = nullptr;
  QPointer<QuranReader> m_reader = nullptr;
};

#endif // PLAYERCONTROLS_H
