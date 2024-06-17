#ifndef PLAYBACKCONTROLLER_H
#define PLAYBACKCONTROLLER_H

#include <QObject>
#include "playbackstrategy.h"
#include "verseplayer.h"
#include <components/navigator.h>
#include <interfaces/verseobserver.h>

class PlaybackController
  : public QObject
  , public VerseObserver
{
  Q_OBJECT
public:
  explicit PlaybackController(QObject* parent, QPointer<VersePlayer> player);

  void activeVerseChanged();

  void resetStrategy();
  void setStrategy(PlaybackStrategy* newStrategy);

  void start();
  void next();
  void stop();

  QPointer<VersePlayer> player() const;

signals:
  void verseOutOfPlaybackRange();

private slots:
  void mediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
  Verse& m_current;
  Navigator& m_navigator;
  std::shared_ptr<PlaybackStrategy> m_defaultStrategy;
  std::shared_ptr<PlaybackStrategy> m_strategy;
  QList<VerseObserver*> m_verseObservers;
  QPointer<VersePlayer> m_player;
};

#endif // PLAYBACKCONTROLLER_H
