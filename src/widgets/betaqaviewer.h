#ifndef BETAQAVIEWER_H
#define BETAQAVIEWER_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include <QSettings>
#include <QWidget>

namespace Ui {
class BetaqaViewer;
}

class BetaqaViewer : public QWidget
{
  Q_OBJECT

public:
  explicit BetaqaViewer(QWidget* parent = nullptr);
  ~BetaqaViewer();

  void showSurah(int surah);
  void center();

protected:
  void focusOutEvent(QFocusEvent* event);

private:
  Ui::BetaqaViewer* ui;
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  int m_surah = -1;
};

#endif // BETAQAVIEWER_H
