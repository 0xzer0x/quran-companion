#include "playbackcontroller.h"
#include <player/impl/continuousplaybackstrategy.h>

PlaybackController::PlaybackController(QObject* parent,
                                       QPointer<VersePlayer> player)
  : QObject(parent)
  , m_player(player)
  , m_defaultStrategy(new ContinuousPlaybackStrategy())
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
PlaybackController::verseChanged()
{
  if (m_strategy->verseInRange(m_current))
    m_player->loadActiveVerse();
  else {
    emit verseOutOfRange();
    resetStrategy();
  }
}

void
PlaybackController::resetStrategy()
{
  m_strategy = m_defaultStrategy;
}

void
PlaybackController::setStrategy(PlaybackStrategy* newStrategy)
{
  m_strategy = newStrategy;
}

void
PlaybackController::start()
{
  Verse startVerse = m_strategy->start();
  m_current.update(startVerse);
  notifyVerseObservers();
}

void
PlaybackController::stop()
{
  Verse stopVerse = m_strategy->stop();
  m_current.update(stopVerse);
  notifyVerseObservers();
}

void
PlaybackController::pause()
{
  m_player->pause();
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
  m_current.update(m_strategy->nextVerse());
  notifyVerseObservers();
}
