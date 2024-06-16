#ifndef VERSEOBSERVER_H
#define VERSEOBSERVER_H

class VerseObserver
{
public:
  virtual ~VerseObserver(){};
  virtual void verseChanged() = 0;
};

#endif // VERSEOBSERVER_H
