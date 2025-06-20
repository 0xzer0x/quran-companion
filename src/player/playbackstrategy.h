#ifndef PLAYBACKSTRATEGY_H
#define PLAYBACKSTRATEGY_H

#include <types/verse.h>

/**
 * @class PlaybackStrategy
 * @brief Abstract base class for different playback strategies.
 *
 * This class defines the interface for various playback strategies used to
 * manage the playback of Quranic verses. Implementations of this class should
 * define how playback starts, stops, progresses, and determines if a verse is
 * within the playback range.
 */
class PlaybackStrategy
{
public:
  /**
   * @brief Gets the starting verse for the playback strategy.
   * @return The Verse object representing the starting verse.
   */
  virtual Verse start() = 0;
  /**
   * @brief Gets the stopping verse for the playback strategy.
   * @return The Verse object representing the stopping verse.
   */
  virtual Verse stop() = 0;
  /**
   * @brief Gets the final/ending verse for the playback strategy.
   * @return The Verse object representing the last played verse.
   */
  virtual Verse end() = 0;
  /**
   * @brief Gets the next verse in the playback sequence.
   * @return An optional Verse object representing the next verse. If there is
   * no next verse, the result is an empty optional.
   */
  virtual std::optional<Verse> nextVerse() = 0;
  /**
   * @brief Checks if a given verse is within the playback range.
   * @param verse The Verse object to be checked.
   * @return True if the verse is within range, false otherwise.
   */
  virtual bool verseInRange(const Verse&) = 0;
  /**
   * @brief Gets post verse pause in seconds.
   * @return Verse frequency delay in seconds.
   */
  virtual int getPostVersePause() = 0;
  /**
   * @brief Gets if it's the last verse repetition.
   * @return 1 if it's last verse repetition.
   */
  virtual bool isLastVerseRepetition() = 0;
  /**
   * @brief Virtual destructor for the PlaybackStrategy class.
   */
  virtual ~PlaybackStrategy() {};
};

#endif // PLAYBACKSTRATEGY_H
