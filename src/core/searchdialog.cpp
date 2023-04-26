#include "searchdialog.h"
#include "ui_searchdialog.h"

/*!
 * \brief SearchDialog::SearchDialog class constructor
 * \param parent pointer to parent widget
 * \param dbPtr pointer to database management interface
 */
SearchDialog::SearchDialog(QWidget *parent,
                           QSettings *settings,
                           DBManager *dbPtr,
                           const QString &iconPath)
    : QDialog(parent)
    , ui(new Ui::SearchDialog)
    , m_settings{settings}
    , m_dbPtr{dbPtr}
    , m_iconsPath{iconPath}
{
    setLayoutDirection(Qt::LeftToRight);
    m_fontPrefix = m_settings->value("Reader/QCF", 1).toInt() == 1 ? "QCF_P" : "QCF2";

    ui->setupUi(this);
    setWindowIcon(QIcon(m_iconsPath + "search.png"));
    ui->btnFwdRes->setIcon(QIcon(m_iconsPath + "arrow-left.png"));
    ui->btnBwdRes->setIcon(QIcon(m_iconsPath + "arrow-right.png"));
    ui->btnTransfer->setStyleSheet("QPushButton{image: url(" + m_iconsPath + "transfer.png" + ")}");
    ui->btnFwdRes->setDisabled(true);
    ui->btnBwdRes->setDisabled(true);

    ui->listViewAllSurahs->setModel(&m_modelAllSurahs);
    ui->listViewSelected->setModel(&m_modelSelectedSurahs);

    connect(ui->btnSrch, &QPushButton::clicked, this, &SearchDialog::getResults);
    connect(ui->btnFwdRes, &QPushButton::clicked, this, &SearchDialog::moveFwd);
    connect(ui->btnBwdRes, &QPushButton::clicked, this, &SearchDialog::moveBwd);

    fillListView();
}

/*!
 * \brief SearchDialog::getResults slot to get search results and update UI accordingly
 */
void SearchDialog::getResults()
{
    m_searchText = ui->ledSearchBar->text().trimmed();
    if (ui->chkWholeWord->isChecked())
        m_searchText = ' ' + m_searchText + ' ';

    if (!m_lbLst.empty()) {
        qDeleteAll(m_lbLst);
        m_lbLst.clear();
        ui->lbResultCount->setText("");
        m_currResults.clear();
    }

    if (m_searchText.isEmpty()) {
        ui->btnFwdRes->setDisabled(true);
        ui->btnBwdRes->setDisabled(true);
        return;
    }

    int range[2];
    range[0] = ui->spnStartPage->value();
    if (ui->spnEndPage->value() < range[0])
        ui->spnEndPage->setValue(range[0]);
    range[1] = ui->spnEndPage->value();

    m_currResults = m_dbPtr->searchVerses(m_searchText, range);
    ui->lbResultCount->setText(QString::number(m_currResults.size()) + tr(" Search results"));
    m_startResult = 0;
    showResults();
}

/*!
 * \brief SearchDialog::verseClicked slot to emit signal for mainwindow to navigate to the clicked verse
 */
void SearchDialog::verseClicked()
{
    QStringList data = sender()->objectName().split('-');
    Verse selected{data.at(0).toInt(), data.at(1).toInt(), data.at(2).toInt()};
    emit navigateToVerse(selected);
}

/*!
 * \brief SearchDialog::showResults slot to display 50 search results according to the startIdx
 */
void SearchDialog::showResults()
{
    int endIdx = m_currResults.size() > m_startResult + 50 ? m_startResult + 50
                                                           : m_currResults.size();

    if (m_startResult == 0)
        ui->btnBwdRes->setDisabled(true);
    else
        ui->btnBwdRes->setDisabled(false);
    if (endIdx == m_currResults.size())
        ui->btnFwdRes->setDisabled(true);
    else
        ui->btnFwdRes->setDisabled(false);

    for (int i = m_startResult; i < endIdx; i++) {
        Verse v = m_currResults.at(i);
        QString fontName = m_fontPrefix + QString::number(v.page).rightJustified(3, '0');
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

    ui->scrollArea->verticalScrollBar()->setValue(0);
}

/*!
 * \brief SearchDialog::moveFwd slot for displaying the next 50 results from the search results
 */
void SearchDialog::moveFwd()
{
    if (!m_currResults.empty() && m_startResult + 50 < m_currResults.size()) {
        if (!m_lbLst.empty()) {
            qDeleteAll(m_lbLst);
            m_lbLst.clear();
        }

        m_startResult += 50;
        showResults();
    }
}

/*!
 * \brief SearchDialog::moveBwd slot for displaying the previous 50 results from the search results
 */
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

void SearchDialog::fillListView()
{
    bool en = m_settings->value("Language") == "العربية" ? false : true;
    for (int i = 1; i <= 114; i++) {
        QStandardItem *surah = new QStandardItem(m_dbPtr->getSurahName(i, en));
        m_modelAllSurahs.invisibleRootItem()->appendRow(surah);
    }
}

void SearchDialog::btnTransferClicked() {}

void SearchDialog::closeEvent(QCloseEvent *event)
{
    if (!m_lbLst.empty()) {
        qDeleteAll(m_lbLst);
        m_lbLst.clear();
        ui->lbResultCount->setText("");
        ui->ledSearchBar->clear();
        m_currResults.clear();
        ui->btnFwdRes->setDisabled(true);
        ui->btnBwdRes->setDisabled(true);
    }

    this->hide();
}

SearchDialog::~SearchDialog()
{
    delete ui;
}
