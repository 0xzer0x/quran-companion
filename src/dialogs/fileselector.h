#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QFileDialog>

class FileSelector : public QFileDialog
{
public:
  enum Mode
  {
    Read,
    Write
  };
  FileSelector(QWidget* parent = nullptr);
  QString selectJson(Mode mode);

private:
  void setCaption(Mode mode);
  void setAcceptMode(Mode mode);
  bool validFile(Mode mode, const QString& file);
};

#endif // FILESELECTOR_H
