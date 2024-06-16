#include "setplaybackstrategy.h"

#include <utils/servicefactory.h>

SetPlaybackStrategy::SetPlaybackStrategy(Verse start,
                                         Verse end,
                                         int repeatCount,
                                         int verseFrequency)
  : m_quranService(ServiceFactory::quranService())
  , m_start(start)
  , m_end(end)
  , m_current(Verse::getCurrent())
  , m_currentIteration(0)
  , m_currentVerseIteration(0)
  , m_repeatCount(repeatCount)
  , m_verseFrequency(verseFrequency)
{
}

Verse
SetPlaybackStrategy::start()
{
  return m_start;
}

Verse
SetPlaybackStrategy::stop()
{
  return m_start;
}

Verse
SetPlaybackStrategy::nextVerse()
{
  bool iterationDone = m_current == m_end;
  bool setDone = iterationDone && ++m_currentIteration == m_repeatCount;
  bool verseRepeatDone = ++m_currentVerseIteration == m_verseFrequency;

  if (setDone) {
    if (verseRepeatDone) {
      return stop();
    } else {
      return m_current;
    }
  } else if (iterationDone) {
    if (verseRepeatDone) {
      return m_start;
    } else {
      return m_current;
    }
  } else {
    if (verseRepeatDone) {
      return m_quranService->next(m_current, true);
    } else {
      return m_current;
    }
  }
}

bool
SetPlaybackStrategy::verseInRange(const Verse &v)
{
  return v >= m_start && v <= m_end;
}
