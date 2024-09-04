#ifndef VERSEOBSERVER_H
#define VERSEOBSERVER_H

/**
 * @class VerseObserver
 * @brief Abstract base class for observing changes in the active verse.
 *
 * This class defines an interface for objects that need to be notified when the
 * active verse changes.
 */
class VerseObserver
{
public:
  /**
   * @brief Pure virtual method that is called when the active verse changes.
   *
   * Derived classes must implement this method to handle verse change
   * notifications.
   */
  virtual void activeVerseChanged() = 0;
  /**
   * @brief Virtual destructor for the VerseObserver class.
   */
  virtual ~VerseObserver() {};
};

#endif // VERSEOBSERVER_H
