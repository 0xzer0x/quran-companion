#ifndef PLAYBACKCONTROLLER_H
#define PLAYBACKCONTROLLER_H

#include "playbackstrategy.h"
#include "verseplayer.h"
#include <QObject>
#include <navigation/navigator.h>
#include <navigation/verseobserver.h>

/**
 * @class PlaybackController
 * @brief Manages the playback of Quranic verses and their navigation.
 *
 * This class handles the playback of Quranic verses using a specified playback
 * strategy. It interacts with the `VersePlayer` to control playback, and uses
 * the `Navigator` to manage navigation between verses. It also observes changes
 * in the active verse.
 */
class PlaybackController
  : public QObject
  , public VerseObserver
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a PlaybackController object.
   * @param parent The parent QObject for this controller.
   * @param player A pointer to the VersePlayer used for playback.
   */
  explicit PlaybackController(QObject* parent, QPointer<VersePlayer> player);

  /**
   * @brief Responds to changes in the active verse.
   *
   * This method is called when the active verse changes. It updates the
   * playback strategy if the current verse is out of range and reloads the
   * active verse.
   */
  void activeVerseChanged() override;
  /**
   * @brief Resets the playback strategy to the default strategy.
   */
  void resetStrategy();
  /**
   * @brief Sets a new playback strategy.
   * @param newStrategy A shared pointer to the new PlaybackStrategy.
   */
  void setStrategy(std::shared_ptr<PlaybackStrategy> newStrategy);
  /**
   * @brief Starts playback using the current strategy.
   */
  void start();
  /**
   * @brief Advances to the next verse in the playback sequence.
   * If there are no more verses, stops playback and emits the playbackFinished
   * signal.
   */
  void next();
  /**
   * @brief Stops playback and navigates to the verse where playback stopped.
   */
  void stop();
  /**
   * @brief Retrieves the pointer to the VersePlayer used for playback.
   * @return A pointer to the VersePlayer.
   */
  QPointer<VersePlayer> player() const;

signals:
  /**
   * @brief Emitted when the current verse is out of playback range.
   */
  void verseOutOfPlaybackRange();
  /**
   * @brief Emitted when playback is finished.
   */
  void playbackFinished();

private slots:
  /**
   * @brief Handles changes in media status.
   * @param status The new media status of the player.
   */
  void mediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
  Verse& m_current; ///< Reference to the current verse being played.
  Navigator&
    m_navigator; ///< Reference to the Navigator for managing verse navigation.
  std::shared_ptr<PlaybackStrategy>
    m_defaultStrategy; ///< Shared pointer to the default playback strategy.
  std::shared_ptr<PlaybackStrategy>
    m_strategy; ///< Shared pointer to the current playback strategy.
  QPointer<VersePlayer>
    m_player; ///< Pointer to the VersePlayer used for playback.
  QPointer<QTimer>
    m_nextVerseTimer; ///< Pointer to the QTimer used to schedule playing next verse.
};

#endif // PLAYBACKCONTROLLER_H
