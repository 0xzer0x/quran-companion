#include "playbackcontroller.h"
#include <player/impl/continuousplaybackstrategy.h>

PlaybackController::PlaybackController(QObject* parent,
                                       QPointer<VersePlayer> player)
  : QObject(parent)
  , m_player(player)
  , m_current(Verse::getCurrent())
{
  resetStrategy();
  connect(m_player,
          &QMediaPlayer::mediaStatusChanged,
          this,
          &PlaybackController::mediaStatusChanged);
}

void
PlaybackController::addVerseObserver(VerseObserver* observer)
{
  m_verseObservers.append(observer);
}

void
PlaybackController::removeVerseObserver(VerseObserver* observer)
{
  m_verseObservers.removeAll(observer);
}

void
PlaybackController::notifyVerseObservers() const
{
  for (VerseObserver* observer : m_verseObservers)
    observer->verseChanged();
}

void
PlaybackController::resetStrategy()
{
  m_strategy =
    std::unique_ptr<PlaybackStrategy>(new ContinuousPlaybackStrategy);
}

void
PlaybackController::setStrategy(PlaybackStrategy* newStrategy)
{
  m_strategy = std::unique_ptr<PlaybackStrategy>(newStrategy);
}

void
PlaybackController::start()
{
  Verse startVerse = m_strategy->start();
  m_current.update(startVerse);
  notifyVerseObservers();
  m_player->playCurrentVerse();
}

void
PlaybackController::next()
{
  std::optional<Verse> nextVerse = m_strategy->nextVerse();
  if (nextVerse.has_value()) {
    m_current.update(nextVerse.value());
    notifyVerseObservers();
  } else {
    stop();
  }
}

void
PlaybackController::stop()
{
  Verse stopVerse = m_strategy->stop();
  m_current.update(stopVerse);
  m_player->loadActiveVerse();
  notifyVerseObservers();
}

bool
PlaybackController::isPlaying() const
{
  return m_player->playbackState() == QMediaPlayer::PlayingState;
}

void
PlaybackController::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
  if (status != QMediaPlayer::EndOfMedia)
    return;
  std::optional<Verse> nextVerse = m_strategy->nextVerse();
  if (nextVerse.has_value()) {
    m_current.update(nextVerse.value());
    notifyVerseObservers();
    m_player->playCurrentVerse();
  } else {
    stop();
  }
}

void
PlaybackController::verseChanged()
{
  if (m_strategy->verseInRange(m_current))
    m_player->loadActiveVerse();
  else {
    emit verseOutOfRange();
    resetStrategy();
  }
}

QPointer<VersePlayer>
PlaybackController::player() const
{
  return m_player;
}
