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
  , m_currentVerseRepetition(0)
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

std::optional<Verse>
SetPlaybackStrategy::nextVerse()
{
  bool iterationDone = m_current == m_end;
  bool setDone = iterationDone && ++m_currentIteration == m_repeatCount;
  bool verseRepeatDone = ++m_currentVerseRepetition == m_verseFrequency;

  if (verseRepeatDone) {
    m_currentVerseRepetition = 0;
    if (setDone) {
      return std::nullopt;
    } else if (iterationDone) {
      return m_start;
    } else {
      return m_quranService->next(m_current, true);
    }
  } else {
    return m_current;
  }
}

bool
SetPlaybackStrategy::verseInRange(const Verse& v)
{
  return v >= m_start && v <= m_end;
}
