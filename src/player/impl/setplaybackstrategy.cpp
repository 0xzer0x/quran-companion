#include "setplaybackstrategy.h"

#include <service/servicefactory.h>

SetPlaybackStrategy::SetPlaybackStrategy(Verse start,
                                         Verse end,
                                         int repeatCount,
                                         int verseFrequency,
                                         int postVersePause)
  : m_currentIteration(0)
  , m_currentRepetition(0)
  , m_repeatCount(repeatCount)
  , m_verseFrequency(verseFrequency)
  , m_postVersePauseSeconds(postVersePause)
  , m_start(start)
  , m_end(end)
  , m_current(Verse::getCurrent())
  , m_quranService(ServiceFactory::quranService())
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
SetPlaybackStrategy::end()
{
  return m_end;
}

std::optional<Verse>
SetPlaybackStrategy::nextVerse()
{
  bool iterationDone = m_current == m_end;
  bool setDone = iterationDone && (m_currentIteration + 1) == m_repeatCount;
  bool verseRepeatDone = isLastVerseRepetition();

  if (verseRepeatDone) {
    m_currentRepetition = 0;
    if (setDone) {
      return std::nullopt;
    } else if (iterationDone) {
      m_currentIteration++;
      return m_start;
    } else {
      return m_quranService->next(m_current, true);
    }
  } else {
    m_currentRepetition++;
    return m_current;
  }
}

bool
SetPlaybackStrategy::verseInRange(const Verse& v)
{
  return v >= m_start && v <= m_end;
}

int
SetPlaybackStrategy::getPostVersePause()
{
  return m_postVersePauseSeconds;
}

bool
SetPlaybackStrategy::isLastVerseRepetition()
{
  return (m_currentRepetition + 1) == m_verseFrequency;
}
