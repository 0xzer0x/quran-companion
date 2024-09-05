#include "khatmahdialog.h"
#include "ui_khatmahdialog.h"
#include <QGraphicsDropShadowEffect>
#include <service/servicefactory.h>
#include <utils/configuration.h>
#include <utils/stylemanager.h>

KhatmahDialog::KhatmahDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::KhatmahDialog)
  , m_currVerse(Verse::getCurrent())
  , m_config(Configuration::getInstance())
  , m_navigator(Navigator::getInstance())
  , m_quranService(ServiceFactory::quranService())
  , m_khatmahService(ServiceFactory::khatmahService())
{
  ui->setupUi(this);
  setWindowIcon(
    StyleManager::getInstance().awesome().icon(fa::fa_solid, fa::fa_list));
  ui->lbCurrKhatmah->setText(
    m_khatmahService->getKhatmahName(m_khatmahService->activeKhatmah()));
  loadAll();

  connect(ui->btnStartKhatmah,
          &QPushButton::clicked,
          this,
          &KhatmahDialog::startNewKhatmah);
}

QPointer<InputField>
KhatmahDialog::loadKhatmah(const int id)
{
  Verse v = m_khatmahService->loadVerse(id).value();
  QFrame* frame = new QFrame(ui->scrlDialogContent);
  // property used for styling
  frame->setProperty("khatmah", true);
  if (!m_config.darkMode()) {
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(frame);
    shadow->setOffset(0, 2);
    shadow->setBlurRadius(4);
    shadow->setColor(QColor(0, 0, 0, 60));
    frame->setGraphicsEffect(shadow);
  }

  QHBoxLayout* frmLayout = new QHBoxLayout();
  QVBoxLayout* lbLayout = new QVBoxLayout();
  QVBoxLayout* btnLayout = new QVBoxLayout();
  InputField* ifName =
    new InputField(frame, m_khatmahService->getKhatmahName(id));
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
  if (id == m_khatmahService->activeKhatmah()) {
    m_currActive = frame;
    activate->setDisabled(true);
    remove->setDisabled(true);
  }

  QString info = tr("Surah: ") +
                 m_quranService->surahNames().at(v.surah() - 1) + " - " +
                 tr("Verse: ") + QString::number(v.number());
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
  m_khatmahIds = m_khatmahService->getAllKhatmah();
  for (int i = 0; i < m_khatmahIds.size(); i++) {
    m_names.insert(m_khatmahIds.at(i),
                   m_khatmahService->getKhatmahName(m_khatmahIds.at(i)));
    loadKhatmah(m_khatmahIds.at(i));
  }
}

void
KhatmahDialog::startNewKhatmah()
{
  int id = m_khatmahService->addKhatmah(m_currVerse.toList(), "new");
  QString gen = tr("Khatmah ") + QString::number(id);
  m_khatmahService->editKhatmahName(id, gen);
  InputField* inpField = loadKhatmah(id);
  m_khatmahIds.append(id);
  m_names.insert(id, gen);
  inpField->setFocus();
}

void
KhatmahDialog::renameKhatmah(QString name)
{
  InputField* caller = qobject_cast<InputField*>(sender());
  int id = caller->parent()->objectName().toInt();
  bool ok = m_khatmahService->editKhatmahName(id, name);
  if (!ok)
    caller->setText(m_names.value(id));
  else {
    m_names[id] = name;
    caller->setText(name);
    caller->clearFocus();
    if (id == m_khatmahService->activeKhatmah())
      ui->lbCurrKhatmah->setText(name);
  }
}

void
KhatmahDialog::removeKhatmah()
{
  int id = sender()->parent()->objectName().toInt();
  m_khatmahService->removeKhatmah(id);
  QFrame* rem = ui->scrlDialogContent->findChild<QFrame*>(QString::number(id));
  m_frmLst[m_frmLst.indexOf(rem)] = nullptr;
  delete rem;
}

void
KhatmahDialog::setActiveKhatmah()
{
  QFrame* newActive = qobject_cast<QFrame*>(sender()->parent());
  QVariant id = newActive->objectName();

  m_config.settings().setValue("Reader/Khatmah", id);
  m_khatmahService->saveActiveKhatmah(m_currVerse);
  m_khatmahService->setActiveKhatmah(id.toInt());
  Verse v = m_khatmahService->loadVerse(id.toInt()).value();

  newActive->findChild<QPushButton*>("activate")->setEnabled(false);
  newActive->findChild<QPushButton*>("remove")->setEnabled(false);
  m_currActive->findChild<QPushButton*>("activate")->setEnabled(true);
  m_currActive->findChild<QPushButton*>("remove")->setEnabled(true);
  m_currActive = newActive;

  ui->lbCurrKhatmah->setText(m_khatmahService->getKhatmahName(id.toInt()));
  m_navigator.navigateToVerse(v);
}

void
KhatmahDialog::show()
{
  qDeleteAll(m_frmLst);
  m_frmLst.clear();
  loadAll();
  QDialog::show();
}

void
KhatmahDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

KhatmahDialog::~KhatmahDialog()
{
  delete ui;
}
