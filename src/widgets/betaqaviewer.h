#ifndef BETAQAVIEWER_H
#define BETAQAVIEWER_H

#include <QGraphicsDropShadowEffect>
#include <QPointer>
#include <QPropertyAnimation>
#include <QSettings>
#include <QWidget>
#include <database/betaqatdb.h>

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
  QSharedPointer<BetaqatDb> m_betaqatDb = BetaqatDb::current();

  int m_surah = -1;
  QPointer<QGraphicsDropShadowEffect> m_shadowEffect;
  QPointer<QPropertyAnimation> m_sizeAnim;
};

#endif // BETAQAVIEWER_H
