#ifndef DOWNLOADJOB_H
#define DOWNLOADJOB_H

#include <QObject>

/**
 * @class DownloadJob
 * @brief An abstract base class representing a download job.
 *
 * This class provides a common interface for different types of download jobs.
 * It defines methods for starting and stopping downloads, checking download
 * status, and retrieving information about the progress of the download.
 * Derived classes should implement these methods according to their specific
 * download requirements.
 */
class DownloadJob : public QObject
{
  Q_OBJECT

public:
  /**
   * @enum Type
   * @brief Enumeration of different download job types.
   *
   * This enumeration defines various types of download jobs that can be handled
   * by the DownloadJob class. It helps in categorizing the type of download
   * being performed.
   */
  enum Type
  {
    TafsirFile,      ///< Download job for Tafsir file.
    TranslationFile, ///< Download job for Translation file.
    Qcf,             ///< Download job for QCF (Quran Code File).
    Recitation       ///< Download job for Recitation file.
  };
  /**
   * @brief Starts the download job.
   *
   * This pure virtual method must be implemented by derived classes to initiate
   * the download process. It should handle all necessary steps to start the
   * download.
   */
  virtual void start() = 0;
  /**
   * @brief Stops the download job.
   *
   * This pure virtual method must be implemented by derived classes to halt an
   * ongoing download. It should properly terminate the download process and
   * clean up resources.
   */
  virtual void stop() = 0;
  /**
   * @brief Checks if the download job is currently active.
   *
   * This pure virtual method must be implemented by derived classes to
   * determine whether the download is in progress.
   * @return True if the download is active, false otherwise.
   */
  virtual bool isDownloading() = 0;
  /**
   * @brief Gets the number of completed download units.
   *
   * This pure virtual method must be implemented by derived classes to provide
   * the count of units that have been successfully downloaded.
   * @return The number of completed units.
   */
  virtual int completed() = 0;
  /**
   * @brief Gets the total number of units to be downloaded.
   *
   * This pure virtual method must be implemented by derived classes to provide
   * the total number of units that are required to complete the download job.
   * @return The total number of units.
   */
  virtual int total() = 0;
  /**
   * @brief Gets the type of the download job.
   *
   * This pure virtual method must be implemented by derived classes to specify
   * the type of download job being performed.
   * @return The type of the download job.
   */
  virtual Type type() = 0;
  /**
   * @brief Gets the name of the download job.
   *
   * This pure virtual method must be implemented by derived classes to provide
   * the name or description of the download job.
   * @return The name of the download job.
   */
  virtual QString name() = 0;
  /**
   * @brief Destructor for the DownloadJob class.
   *
   * The destructor is virtual to ensure proper cleanup of derived classes.
   */
  virtual ~DownloadJob() {};

signals:
  /**
   * @brief Signal emitted when the download job is aborted.
   *
   * This signal is emitted when the download process is canceled or stopped
   * before completion.
   */
  void aborted();
  /**
   * @brief Signal emitted when the download job is finished.
   *
   * This signal is emitted when the download process has completed
   * successfully.
   */
  void finished();
  /**
   * @brief Signal emitted to indicate progress in the download job.
   *
   * This signal is emitted periodically to update the progress of the download.
   */
  void progressed();
  /**
   * @brief Signal emitted when the download job fails.
   *
   * This signal is emitted if the download process encounters an error and
   * cannot complete.
   */
  void failed();
  /**
   * @brief Signal emitted to update the download speed.
   *
   * This signal is emitted to report the current download speed.
   * @param speed The speed of the download.
   * @param unit The unit of measurement for the speed (e.g., KB/s, MB/s).
   */
  void downloadSpeedUpdated(int speed, QString unit);
};

#endif // DOWNLOADJOB_H
