#ifndef PLAYBACKCONTROLLER_H
#define PLAYBACKCONTROLLER_H

#include "playbackstrategy.h"
#include "verseplayer.h"
#include <QObject>
#include <interfaces/verseobserver.h>

class PlaybackController
  : public QObject
  , public VerseObserver
{
  Q_OBJECT
public:
  explicit PlaybackController(QObject* parent, QPointer<VersePlayer> player);

  void addVerseObserver(VerseObserver* observer);
  void removeVerseObserver(VerseObserver* observer);
  void verseChanged();

  void resetStrategy();
  void setStrategy(PlaybackStrategy* newStrategy);

  void start();
  void next();
  void stop();

  bool isPlaying() const;
  QPointer<VersePlayer> player() const;

signals:
  void verseOutOfRange();

private slots:
  void mediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
  Verse& m_current;
  void notifyVerseObservers() const;
  std::unique_ptr<PlaybackStrategy> m_strategy;
  QList<VerseObserver*> m_verseObservers;
  QPointer<VersePlayer> m_player;
};

#endif // PLAYBACKCONTROLLER_H
