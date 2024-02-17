#ifndef BETAQAVIEWER_H
#define BETAQAVIEWER_H

#include "../utils/dbmanager.h"
#include <QGraphicsDropShadowEffect>
#include <QPointer>
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
  Ui::BetaqaViewer* ui;
  QSharedPointer<DBManager> m_dbMgr = DBManager::current();

  int m_surah = -1;
  QPointer<QGraphicsDropShadowEffect> m_shadowEffect;
  QPointer<QPropertyAnimation> m_sizeAnim;
};

#endif // BETAQAVIEWER_H
