#ifndef PLAYBACKSTRATEGY_H
#define PLAYBACKSTRATEGY_H

#include <types/verse.h>

class PlaybackStrategy
{
public:
  virtual ~PlaybackStrategy(){};
  virtual Verse start() = 0;
  virtual Verse stop() = 0;
  virtual std::optional<Verse> nextVerse() = 0;
  virtual bool verseInRange(const Verse&) = 0;
};

#endif // !PLAYBACKSTATE_H
