#include "searchdialog.h"
#include "ui_searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent, DBManager *dbPtr)
    : QDialog(parent)
    , ui(new Ui::SearchDialog)
{
    setLayoutDirection(Qt::LeftToRight);
    ui->setupUi(this);

    ui->btnFwdRes->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->btnBwdRes->setIcon(QApplication::style()->standardIcon(QStyle::SP_ArrowRight));
    setWindowTitle(tr("Verse search"));

    m_dbPtr = dbPtr;

    connect(ui->btnSrch, &QPushButton::clicked, this, &SearchDialog::getResults);
    connect(ui->btnFwdRes, &QPushButton::clicked, this, &SearchDialog::moveFwd);
    connect(ui->btnBwdRes, &QPushButton::clicked, this, &SearchDialog::moveBwd);
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::getResults()
{
    if (m_searchText == ui->ledSearchBar->text().trimmed())
        return;

    m_searchText = ui->ledSearchBar->text().trimmed();

    if (!m_lbLst.empty()) {
        qDeleteAll(m_lbLst);
        m_lbLst.clear();
        ui->lbResultCount->setText("");
        m_currResults.clear();
    }

    if (m_searchText.isEmpty())
        return;

    qInfo() << " running verse search query";
    m_currResults = m_dbPtr->searchVerses(m_searchText);

    ui->lbResultCount->setText(QString::number(m_currResults.size()) + tr(" Search results"));
    m_startResult = 0;
    showResults();
}

void SearchDialog::verseClicked()
{
    QStringList data = sender()->objectName().split('-');
    Verse selected(data.at(0).toInt(), data.at(1).toInt(), data.at(2).toInt());

    emit navigateToVerse(selected);
}

void SearchDialog::showResults()
{
    int endIdx = m_currResults.size() > m_startResult + 50 ? m_startResult + 50
                                                           : m_currResults.size();
    for (int i = m_startResult; i < endIdx; i++) {
        Verse v = m_currResults.at(i);
        QString fontName = "QCF_P" + QString::number(v.page).rightJustified(3, '0');
        VerseFrame *vFrame = new VerseFrame(ui->srclResults);
        QLabel *lbInfo = new QLabel(vFrame);
        ClickableLabel *clkLb = new ClickableLabel(vFrame);

        QString info = tr("Surah: ") + QString::number(v.surah) + " - " + tr("Ayah: ")
                       + QString::number(v.number);
        lbInfo->setText(info);
        lbInfo->setAlignment(Qt::AlignLeft);
        clkLb->setMargin(5);
        clkLb->setObjectName(QString::number(v.page) + '-' + QString::number(v.surah) + '-'
                             + QString::number(v.number));

        clkLb->setFont(QFont(fontName, 15));
        clkLb->setText(m_dbPtr->getVerseGlyphs(v.surah, v.number));
        clkLb->setAlignment(Qt::AlignLeft);
        clkLb->setWordWrap(true);

        connect(clkLb, &ClickableLabel::clicked, this, &SearchDialog::verseClicked);

        vFrame->layout()->addWidget(lbInfo);
        vFrame->layout()->addWidget(clkLb);
        vFrame->setObjectName("F-" + clkLb->objectName());
        ui->srclResults->layout()->addWidget(vFrame);
        m_lbLst.append(vFrame);
    }
}

void SearchDialog::moveFwd()
{
    if (!m_currResults.empty() && m_startResult < m_currResults.size()) {
        if (!m_lbLst.empty()) {
            qDeleteAll(m_lbLst);
            m_lbLst.clear();
        }

        m_startResult += 50;
        showResults();
    }
}

void SearchDialog::moveBwd()
{
    if (!m_currResults.empty() && m_startResult > 0) {
        if (!m_lbLst.empty()) {
            qDeleteAll(m_lbLst);
            m_lbLst.clear();
        }

        m_startResult -= 50;
        showResults();
    }
}

void SearchDialog::closeEvent(QCloseEvent *event)
{
    if (!m_lbLst.empty()) {
        qDeleteAll(m_lbLst);
        m_lbLst.clear();
        ui->lbResultCount->setText("");
        m_currResults.clear();
    }

    this->hide();
}
