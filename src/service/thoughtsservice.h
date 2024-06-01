#ifndef THOUGHTSSERVICE_H
#define THOUGHTSSERVICE_H

#include <types/verse.h>


class ThoughtsService
{
public:
  /**
   * @brief save thoughts for the verse given
   * @param verse - Verse to save the thoughts for
   * @param text - QString of the thoughts text
   */
  virtual void saveThoughts(Verse& verse, const QString& text) = 0;
  /**
   * @brief get the user stored thoughts for the verse given
   * @param verse - Verse to get thoughts for
   * @return QString of the thought text
   */
  virtual QString getThoughts(const Verse& verse) const = 0;
  /**
   * @brief get all user stored thoughts
   * @return QList of thoughts represented as QPair(s) of Verse and QString of
   * the thought text
   */
  virtual QList<QPair<Verse, QString>> allThoughts() const = 0;
};

#endif // THOUGHTSSERVICE_H
