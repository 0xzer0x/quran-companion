#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include "../globals.h"
#include "../utils/shortcuthandler.h"
#include "../utils/verse.h"
#include "../utils/verseplayer.h"
#include "quranreader.h"
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
                          QuranReader* reader,
                          ShortcutHandler* handler);
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
  Verse* m_currVerse = Verse::current();
  QSettings* const m_settings = Globals::settings;
  const QList<Reciter>& m_recitersList = Globals::recitersList;
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  fa::QtAwesome* m_fa = Globals::awesome;
  Ui::PlayerControls* ui;
  /**
   * @brief load icons for different UI elements
   */
  void loadIcons();
  /**
   * @brief connects signals and slots for different UI components
   */
  void setupConnections();
  /**
   * @brief connect ShortcutHandler signals to their corresponding slots
   */
  void setupShortcuts(ShortcutHandler* handler);
  /**
   * @brief float value of the current playback volume (0 - 1.0)
   */
  qreal m_volume = 1;
  /**
   * @brief pointer to VersePlayer instance
   */
  VersePlayer* m_player = nullptr;
  QuranReader* m_reader = nullptr;
};

#endif // PLAYERCONTROLS_H
