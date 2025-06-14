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

  Verse start() override;
  Verse stop() override;
  Verse end() override;
  std::optional<Verse> nextVerse() override;
  int getPostVersePause() override;
  bool isLastVerseRepetition() override;
  bool verseInRange(const Verse&) override;
};

#endif // !CONTINUOUSPLAYBACKSTATE_H
