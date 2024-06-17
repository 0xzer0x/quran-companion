#include "playbackcontroller.h"
#include <player/impl/continuousplaybackstrategy.h>

PlaybackController::PlaybackController(QObject* parent,
                                       QPointer<VersePlayer> player)
  : QObject(parent)
  , m_player(player)
  , m_current(Verse::getCurrent())
  , m_navigator(Navigator::getInstance())
  , m_defaultStrategy(new ContinuousPlaybackStrategy)
{
  resetStrategy();
  connect(m_player,
          &QMediaPlayer::mediaStatusChanged,
          this,
          &PlaybackController::mediaStatusChanged);
}

void
PlaybackController::resetStrategy()
{
  m_strategy = m_defaultStrategy;
}

void
PlaybackController::setStrategy(PlaybackStrategy* newStrategy)
{
  m_strategy = std::shared_ptr<PlaybackStrategy>(newStrategy);
}

void
PlaybackController::start()
{
  Verse startVerse = m_strategy->start();
  m_current.update(startVerse);
  m_player->playCurrentVerse();
}

void
PlaybackController::next()
{
  std::optional<Verse> nextVerse = m_strategy->nextVerse();
  m_navigator.navigateToVerse(nextVerse.value_or(m_strategy->stop()));
}

void
PlaybackController::stop()
{
  Verse stopVerse = m_strategy->stop();
  m_navigator.navigateToVerse(stopVerse);
}

void
PlaybackController::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
  if (status != QMediaPlayer::EndOfMedia)
    return;
  next();
}

void
PlaybackController::activeVerseChanged()
{
  bool keepPlaying = m_player->isPlaying();
  m_player->stop();

  if (!m_strategy->verseInRange(m_current)) {
    emit verseOutOfPlaybackRange();
    resetStrategy();
  }

  m_player->loadActiveVerse();
  if (keepPlaying)
    m_player->play();
}

QPointer<VersePlayer>
PlaybackController::player() const
{
  return m_player;
}
