#ifndef SERVICEFACTORY_H
#define SERVICEFACTORY_H

#include <QPointer>
#include <service/betaqatservice.h>
#include <service/bookmarkservice.h>
#include <service/glyphservice.h>
#include <service/khatmahservice.h>
#include <service/quranservice.h>
#include <service/tafsirservice.h>
#include <service/thoughtsservice.h>
#include <service/translationservice.h>

class ServiceFactory
{
public:
  static QuranService* quranService();

  static GlyphService* glyphService();

  static BookmarkService* bookmarkService();

  static KhatmahService* khatmahService();

  static ThoughtsService* thoughtsService();

  static BetaqatService* betaqatService();

  static TafsirService* tafsirService();

  static TranslationService* translationService();
};

#endif // SERVICEFACTORY_H
