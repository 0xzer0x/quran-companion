#include "searchdialog.h"
#include "ui_searchdialog.h"

SearchDialog::SearchDialog(QWidget *parent, DBManager *dbPtr)
    : QDialog(parent)
    , ui(new Ui::SearchDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Verse search"));

    m_dbPtr = dbPtr;

    connect(ui->btnSrch, &QPushButton::clicked, this, &SearchDialog::getResults);
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::getResults()
{
    if (!m_lbLst.empty()) {
        qDeleteAll(m_lbLst);
        m_lbLst.clear();
    }

    if (m_searchText == ui->ledSearchBar->text().trimmed())
        return;

    m_searchText = ui->ledSearchBar->text().trimmed();

    if (m_searchText.isEmpty() || m_searchText.size() < 3)
        return;

    qInfo() << "searching db...";
    QList<Verse> results = m_dbPtr->searchVerses(m_searchText);

    foreach (const Verse v, results) {
        QString fontName = "QCF_P" + QString::number(v.page).rightJustified(3, '0');
        ClickableLabel *clkLb = new ClickableLabel(ui->srclResults);

        clkLb->setMargin(5);
        clkLb->setObjectName(QString::number(v.page) + '-' + QString::number(v.surah) + '-'
                             + QString::number(v.number));

        clkLb->setFont(QFont(fontName, 15));
        clkLb->setText(m_dbPtr->getVerseGlyphs(v.surah, v.number));
        clkLb->setWordWrap(true);

        connect(clkLb, &ClickableLabel::clicked, this, &SearchDialog::verseClicked);

        ui->srclResults->layout()->addWidget(clkLb);
        m_lbLst.append(clkLb);
    }

    ui->scrollArea->setAlignment(Qt::AlignRight);
}

void SearchDialog::verseClicked()
{
    QStringList data = sender()->objectName().split('-');
    Verse selected(data.at(0).toInt(), data.at(1).toInt(), data.at(2).toInt());

    emit navigateToVerse(selected);
}

void SearchDialog::closeEvent(QCloseEvent *event)
{
    this->hide();
}
