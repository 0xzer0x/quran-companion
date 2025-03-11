#ifndef CONTINUOUSPLAYBACKSTRATEGY_H
#define CONTINUOUSPLAYBACKSTRATEGY_H

#include <player/playbackstrategy.h>
#include <player/verseplayer.h>

#include <service/quranservice.h>

class ContinuousPlaybackStrategy : public PlaybackStrategy
{
private:
  QuranService* m_quranService;

public:
  ContinuousPlaybackStrategy();

  virtual Verse start() override;
  virtual Verse stop() override;
  virtual std::optional<Verse> nextVerse() override;
  virtual int getNextVerseDelay() override;
  bool verseInRange(const Verse&) override;
};

#endif // !CONTINUOUSPLAYBACKSTATE_H
