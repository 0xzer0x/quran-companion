#ifndef KHATMAHDIALOG_H
#define KHATMAHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPointer>
#include <QSet>
#include <components/navigator.h>
#include <repository/bookmarksrepository.h>
#include <service/bookmarkservice.h>
#include <service/khatmahservice.h>
#include <service/quranservice.h>
#include <types/verse.h>
#include <widgets/inputfield.h>

namespace Ui {
class KhatmahDialog;
}

/**
 * @brief KhatmahDialog is responsible for management and tracking of different
 * khatmah entries
 */
class KhatmahDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief KhatmahDialog class constructor
   * @param curr - reference to the current active verse
   * @param parent - pointer to the parent widget
   */
  explicit KhatmahDialog(QWidget* parent = nullptr);
  ~KhatmahDialog();
  /**
   * @brief reload shown khatmah entries and show the dialog
   */
  void show();

protected:
  /**
   * @brief Re-implementation of QWidget::closeEvent() in order to hide the
   * window instead of closing it
   * @param event
   */
  void closeEvent(QCloseEvent* event);

private slots:
  /**
   * @brief add a new khatmah entry with the default name of Khatmah N, where N
   * is the id of the khatmah entry
   */
  void startNewKhatmah();
  /**
   * @brief rename the khatmah attached with the sender InputField to the name
   * given
   * @param name - new name of the khatmah
   */
  void renameKhatmah(QString name);
  /**
   * @brief remove the khatmah attached with the sender QPushButton
   */
  void removeKhatmah();
  /**
   * @brief set the khatmah attached with the sender QPushButton as the active
   * khatmah, navigate to it
   */
  void setActiveKhatmah();

private:
  Ui::KhatmahDialog* ui;
  const Verse& m_currVerse;
  QuranService* m_quranService;
  KhatmahService* m_khatmahService;
  Configuration& m_config;
  Navigator& m_navigator;
  /**
   * @brief load all khatmah entries available
   */
  void loadAll();
  /**
   * @brief load the khatmah with the given id
   * @param id - id of khatmah to load
   * @return InputField* - pointer to the InputField of the loaded khatmah name
   */
  QPointer<InputField> loadKhatmah(const int id);
  /**
   * @brief pointer to the current active khatmah QFrame
   */
  QPointer<QFrame> m_currActive;
  /**
   * @brief QList of all QFrame(s) loaded for the different khatmah entries
   */
  QList<QPointer<QFrame>> m_frmLst;
  /**
   * @brief QList of all available khatmah id(s)
   */
  QList<int> m_khatmahIds;
  /**
   * @brief QHash of khatmah ids and their associated names
   */
  QHash<int, QString> m_names;
};

#endif // KHATMAHDIALOG_H
