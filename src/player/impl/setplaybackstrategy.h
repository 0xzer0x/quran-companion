#ifndef SETPLAYBACKSTRATEGY_H
#define SETPLAYBACKSTRATEGY_H

#include <player/playbackstrategy.h>
#include <player/verseplayer.h>

#include <service/quranservice.h>

class SetPlaybackStrategy : public PlaybackStrategy
{
public:
  SetPlaybackStrategy(Verse start,
                      Verse end,
                      int repeatCount = 1,
                      int verseFrequency = 1,
                      int verseFrequencyDelay = 0);

  Verse start() override;
  Verse stop() override;
  std::optional<Verse> nextVerse() override;
  bool verseInRange(const Verse& v) override;
  int getNextVerseDelay() override;

private:
  int m_repeatCount;
  int m_verseFrequency;
  int m_verseFrequencyDelay;
  int m_currentIteration;
  int m_currentRepetition;
  QuranService* m_quranService;
  const Verse& m_current;
  Verse m_start;
  Verse m_end;
};

#endif // SETPLAYBACKSTRATEGY_H
