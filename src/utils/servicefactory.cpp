#include "servicefactory.h"
#include <service/impl/betaqatservicesqlimpl.h>
#include <service/impl/bookmarkservicesqlimpl.h>
#include <service/impl/glyphservicesqlimpl.h>
#include <service/impl/khatmahservicesqlimpl.h>
#include <service/impl/quranservicesqlimpl.h>
#include <service/impl/tafsirservicesqlimpl.h>
#include <service/impl/thoughtsservicesqlimpl.h>
#include <service/impl/translationservicesqlimpl.h>

QuranService*
ServiceFactory::quranService()
{
  static QuranServiceSqlImpl quranService = QuranServiceSqlImpl();
  return (QuranService*)&quranService;
}

GlyphService*
ServiceFactory::glyphService()
{
  static GlyphServiceSqlImpl glyphService = GlyphServiceSqlImpl();
  return (GlyphService*)&glyphService;
}

BookmarkService*
ServiceFactory::bookmarkService()
{
  static BookmarkServiceSqlImpl bookmarkService = BookmarkServiceSqlImpl();
  return (BookmarkService*)&bookmarkService;
}

KhatmahService*
ServiceFactory::khatmahService()
{
  static KhatmahServiceSqlImpl khatmahService = KhatmahServiceSqlImpl();
  return (KhatmahService*)&khatmahService;
}

ThoughtsService*
ServiceFactory::thoughtsService()
{
  static ThoughtsServiceSqlImpl thoughtsService = ThoughtsServiceSqlImpl();
  return (ThoughtsService*)&thoughtsService;
}

BetaqatService*
ServiceFactory::betaqatService()
{
  static BetaqatServiceSqlImpl betaqatService = BetaqatServiceSqlImpl();
  return (BetaqatService*)&betaqatService;
}

TafsirService*
ServiceFactory::tafsirService()
{
  static TafsirServiceSqlImpl tafsirService = TafsirServiceSqlImpl();
  return (TafsirService*)&tafsirService;
}

TranslationService*
ServiceFactory::translationService()
{
  static TranslationServiceSqlImpl translationService =
    TranslationServiceSqlImpl();
  return (TranslationService*)&translationService;
}
