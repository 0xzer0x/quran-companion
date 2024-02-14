#ifndef BETAQAVIEWER_H
#define BETAQAVIEWER_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
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
  DBManager* m_dbMgr = DBManager::instance();
  int m_surah = -1;
  QGraphicsDropShadowEffect* m_shadowEffect = nullptr;
  QPropertyAnimation* m_sizeAnim = nullptr;
};

#endif // BETAQAVIEWER_H
