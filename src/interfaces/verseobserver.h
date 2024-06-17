#ifndef VERSEOBSERVER_H
#define VERSEOBSERVER_H

#include <types/navigationinitiator.h>

class VerseObserver
{
public:
  virtual ~VerseObserver(){};
  virtual void activeVerseChanged() = 0;
};

#endif // VERSEOBSERVER_H
