#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include <QFileInfo>
#include <QNetworkReply>
#include <QPointer>
#include <QUrl>

/**
 * @class DownloadTask
 * @brief An abstract base class representing a download task.
 *
 * This class provides an interface for defining a download task, including the
 * URL of the file to be downloaded and the destination where the file should be
 * saved. Derived classes should implement the methods to specify the URL and
 * destination.
 */
class DownloadTask
{
public:
  /**
   * @brief Gets the URL of the download task.
   *
   * This pure virtual method must be implemented by derived classes to provide
   * the URL from which the file will be downloaded.
   * @return The URL of the file to be downloaded.
   */
  virtual QUrl url() const = 0;
  /**
   * @brief Gets the destination file information for the download task.
   *
   * This pure virtual method must be implemented by derived classes to specify
   * the destination file where the downloaded content will be saved.
   * @return QFileInfo containing the destination file information.
   */
  virtual QFileInfo destination() const = 0;
  /**
   * @brief Destructor for the DownloadTask class.
   *
   * The destructor is virtual to ensure proper cleanup of derived classes.
   */
  virtual ~DownloadTask() {};
};

#endif // DOWNLOADTASK_H
