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
  m_navigator.addObserver(this);
}

void
PlaybackController::resetStrategy()
{
  m_strategy = m_defaultStrategy;
}

void
PlaybackController::setStrategy(std::shared_ptr<PlaybackStrategy> newStrategy)
{
  m_strategy = newStrategy;
}

void
PlaybackController::start()
{
  Verse startVerse = m_strategy->start();
  m_navigator.navigateToVerse(startVerse);
  m_player->play();
}

void
PlaybackController::next()
{
  std::optional<Verse> nextVerse = m_strategy->nextVerse();
  if (nextVerse.has_value())
    m_navigator.navigateToVerse(nextVerse.value());
  else
    stop();
}

void
PlaybackController::stop()
{
  m_player->stop();
  Verse stopVerse = m_strategy->stop();
  m_navigator.navigateToVerse(stopVerse);
}

void
PlaybackController::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
  if (status == QMediaPlayer::EndOfMedia)
    next();
}

void
PlaybackController::activeVerseChanged()
{
  bool keepPlaying = m_player->isOn();
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
