/**
 * @file fileselector.h
 * @brief Header file for the FileSelector class, which extends QFileDialog to
 * provide file selection for JSON files.
 */

#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QFileDialog>

/**
 * @brief The FileSelector class extends QFileDialog to offer a specialized
 * dialog for selecting JSON files.
 * @details It allows users to open or save JSON files, with modes specified by
 * the user. It ensures that the selected file is valid for the specified
 * operation.
 */
class FileSelector : public QFileDialog
{
public:
  /**
   * @brief Enum defining the modes for file selection.
   */
  enum Mode
  {
    Read, ///< Mode for opening files.
    Write ///< Mode for saving files.
  };
  /**
   * @brief Constructor for FileSelector.
   * @param parent Pointer to the parent widget.
   */
  FileSelector(QWidget* parent = nullptr);
  /**
   * @brief Opens a file dialog for selecting a JSON file.
   * @param mode The mode for file selection (Read or Write).
   * @return The path of the selected JSON file, or an empty QString if no valid
   * file was selected.
   */
  QString selectJson(Mode mode);

private:
  /**
   * @brief Sets the dialog's caption based on the mode.
   * @param mode The mode for file selection (Read or Write).
   */
  void setCaption(Mode mode);
  /**
   * @brief Sets the dialog's accept mode based on the mode.
   * @param mode The mode for file selection (Read or Write).
   */
  void setAcceptMode(Mode mode);
  /**
   * @brief Checks if the selected file is valid based on the mode.
   * @param mode The mode for file selection (Read or Write).
   * @param file The path of the selected file.
   * @return True if the file is valid for the specified mode; otherwise, false.
   */
  bool validFile(Mode mode, const QString& file);
};

#endif // FILESELECTOR_H
