#include "continuousplaybackstrategy.h"

#include <utils/servicefactory.h>

ContinuousPlaybackStrategy::ContinuousPlaybackStrategy()
  : m_quranService(ServiceFactory::quranService())
{
}

Verse
ContinuousPlaybackStrategy::start()
{
  return Verse::getCurrent();
}

Verse
ContinuousPlaybackStrategy::stop()
{
  const Verse& current = Verse::getCurrent();
  return m_quranService->firstInPage(current.page());
}

Verse
ContinuousPlaybackStrategy::nextVerse()
{
  return m_quranService->next(Verse::getCurrent(), true);
}

bool
ContinuousPlaybackStrategy::verseInRange(const Verse &)
{
  return true;
}
