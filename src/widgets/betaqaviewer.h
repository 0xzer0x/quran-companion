#ifndef BETAQAVIEWER_H
#define BETAQAVIEWER_H

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

  void center();

public slots:
  void showSurah(int surah);

protected:
  void focusOutEvent(QFocusEvent* event);

private:
  QSharedPointer<DBManager> m_dbMgr = DBManager::current();
  Ui::BetaqaViewer* ui;

  int m_surah = -1;
  QGraphicsDropShadowEffect* m_shadowEffect = nullptr;
  QPropertyAnimation* m_sizeAnim = nullptr;
};

#endif // BETAQAVIEWER_H
