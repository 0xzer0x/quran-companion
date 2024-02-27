#include "fileselector.h"
#include <QApplication>
#include <utils/dirmanager.h>

FileSelector::FileSelector(QWidget* parent)
  : QFileDialog(parent)
{
  setFileMode(QFileDialog::AnyFile);
  setDirectory(DirManager::downloadsDir->absolutePath());
}

QString
FileSelector::selectJson(Mode mode)
{
  setCaption(mode);
  setAcceptMode(mode);
  setDefaultSuffix("json");
  setNameFilter("JSON Document (*.json)");
  if (!exec() || !validFile(mode, selectedFiles().at(0)))
    return "";
  return selectedFiles().at(0);
}

void
FileSelector::setCaption(Mode mode)
{
  switch (mode) {
    case Mode::Read:
      setWindowTitle(qApp->translate("QFileDialog", "Open File"));
      break;
    case Mode::Write:
      setWindowTitle(qApp->translate("QFileDialog", "Save File"));
      break;
    default:
      break;
  }
}

void
FileSelector::setAcceptMode(Mode mode)
{
  switch (mode) {
    case Mode::Read:
      QFileDialog::setAcceptMode(QFileDialog::AcceptOpen);
      break;
    case Mode::Write:
      QFileDialog::setAcceptMode(QFileDialog::AcceptSave);
      break;
    default:
      break;
  }
}

bool
FileSelector::validFile(Mode mode, const QString& file)
{
  if (file.isEmpty())
    return false;

  QFileInfo fInf(file);
  if (mode == Read)
    return fInf.isReadable();
  if (mode == Write)
    return fInf.isWritable() ||
           QFileInfo(fInf.dir().absolutePath()).isWritable();

  return false;
}
