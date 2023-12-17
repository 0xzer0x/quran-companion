#include "khatmahdialog.h"
#include "ui_khatmahdialog.h"
#include <qnamespace.h>
#include <qpushbutton.h>
#include <qvariant.h>

KhatmahDialog::KhatmahDialog(const Verse& curr, QWidget* parent)
  : QDialog(parent)
  , m_currVerse(curr)
  , ui(new Ui::KhatmahDialog)
{
  ui->setupUi(this);
  ui->lbCurrKhatmah->setText(m_dbMgr->getKhatmahName(m_dbMgr->activeKhatmah()));
  loadAll();

  connect(ui->btnStartKhatmah,
          &QPushButton::clicked,
          this,
          &KhatmahDialog::startNewKhatmah,
          Qt::UniqueConnection);
}

InputField*
KhatmahDialog::loadKhatmah(const int id)
{
  Verse v;
  m_dbMgr->getPosition(id, v);
  QFrame* frame = new QFrame(ui->scrlDialogContent);
  QHBoxLayout* frmLayout = new QHBoxLayout();
  QVBoxLayout* lbLayout = new QVBoxLayout();
  QVBoxLayout* btnLayout = new QVBoxLayout();
  InputField* ifName = new InputField(frame, m_dbMgr->getKhatmahName(id));
  QLabel* lbPosition = new QLabel(frame);
  QPushButton* activate = new QPushButton(tr("Set as active"), frame);
  QPushButton* remove = new QPushButton(tr("Remove"), frame);
  activate->setObjectName("activate");
  remove->setObjectName("remove");
  activate->setCursor(Qt::PointingHandCursor);
  remove->setCursor(Qt::PointingHandCursor);
  activate->setStyleSheet(
    "QPushButton { min-width: 150px; max-width: 150px; }");
  remove->setStyleSheet("QPushButton { min-width: 150px; max-width: 150px; }");
  if (id == m_dbMgr->activeKhatmah()) {
    m_currActive = frame;
    activate->setDisabled(true);
    remove->setDisabled(true);
  }

  QString info = tr("Surah: ") + m_dbMgr->surahNameList().at(v.surah - 1) +
                 " - " + tr("Verse: ") + QString::number(v.number);
  lbPosition->setText(info);

  activate->setFocusPolicy(Qt::NoFocus);
  remove->setFocusPolicy(Qt::NoFocus);
  ifName->setFocusPolicy(Qt::ClickFocus);
  ifName->setAlignment(Qt::AlignLeft);
  lbPosition->setAlignment(Qt::AlignLeft);

  lbLayout->addStretch();
  lbLayout->addWidget(ifName);
  lbLayout->addWidget(lbPosition);
  lbLayout->addStretch();

  btnLayout->setSpacing(2);
  btnLayout->addStretch();
  btnLayout->addWidget(activate);
  btnLayout->addWidget(remove);
  btnLayout->addStretch();

  frmLayout->addItem(lbLayout);
  frmLayout->addItem(btnLayout);

  frame->setLayout(frmLayout);
  frame->setObjectName(QString::number(id));
  ui->lytKhatmahList->addWidget(frame);
  m_frmLst.append(frame);

  connect(ifName, &InputField::rename, this, &KhatmahDialog::renameKhatmah);
  connect(remove, &QPushButton::clicked, this, &KhatmahDialog::removeKhatmah);
  connect(
    activate, &QPushButton::clicked, this, &KhatmahDialog::setActiveKhatmah);
  return ifName;
}

void
KhatmahDialog::loadAll()
{
  m_khatmahId = m_dbMgr->getAllKhatmah();
  for (int i = 0; i < m_khatmahId.size(); i++) {
    m_names.insert(m_khatmahId.at(i),
                   m_dbMgr->getKhatmahName(m_khatmahId.at(i)));
    loadKhatmah(m_khatmahId.at(i));
  }
}

void
KhatmahDialog::startNewKhatmah()
{
  int id = m_dbMgr->addKhatmah(m_currVerse, "new");
  QString gen = tr("Khatmah ") + QString::number(id);
  m_dbMgr->editKhatmah(id, gen);
  InputField* inpField = loadKhatmah(id);
  m_khatmahId.append(id);
  m_names.insert(id, gen);
  inpField->setFocus();
}

void
KhatmahDialog::renameKhatmah(QString name)
{
  InputField* caller = qobject_cast<InputField*>(sender());
  int id = caller->parent()->objectName().toInt();
  bool ok = m_dbMgr->editKhatmah(id, name);
  if (!ok)
    caller->setText(m_names.value(id));
  else {
    m_names[id] = name;
    caller->setText(name);
    if (id == m_dbMgr->activeKhatmah())
      ui->lbCurrKhatmah->setText(name);
  }
}

void
KhatmahDialog::removeKhatmah()
{
  int id = sender()->parent()->objectName().toInt();
  m_dbMgr->removeKhatmah(id);
  QFrame* rem = ui->scrlDialogContent->findChild<QFrame*>(QString::number(id));
  m_frmLst[m_frmLst.indexOf(rem)] = nullptr;
  delete rem;
}

void
KhatmahDialog::setActiveKhatmah()
{
  Verse v;
  QFrame* newActive = qobject_cast<QFrame*>(sender()->parent());
  QVariant id = newActive->objectName();

  m_settings->setValue("Reader/Khatmah", id);
  m_dbMgr->savePosition(m_currVerse);
  m_dbMgr->setActiveKhatmah(id.toInt());
  m_dbMgr->getPosition(id.toInt(), v);

  newActive->findChild<QPushButton*>("activate")->setEnabled(false);
  newActive->findChild<QPushButton*>("remove")->setEnabled(false);
  m_currActive->findChild<QPushButton*>("activate")->setEnabled(true);
  m_currActive->findChild<QPushButton*>("remove")->setEnabled(true);
  m_currActive = newActive;

  ui->lbCurrKhatmah->setText(m_dbMgr->getKhatmahName(id.toInt()));
  emit navigateToVerse(v);
}

void
KhatmahDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

void
KhatmahDialog::show()
{
  qDeleteAll(m_frmLst);
  m_frmLst.clear();
  loadAll();
  QDialog::show();
}

KhatmahDialog::~KhatmahDialog()
{
  delete ui;
}