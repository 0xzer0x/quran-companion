#include "shareimagedialog.h"
#include "ui_shareimagedialog.h"

#include <QAbstractTextDocumentLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QScreen>
#include <QScrollBar>
#include <QtMath>
#include <QStandardPaths>
#include <navigation/navigator.h>
#include <service/servicefactory.h>
#include <types/tafsir.h>
#include <types/translation.h>
#include <utils/fontmanager.h>
#include <utils/stylemanager.h>
#include <widgets/quranpagebrowser.h>

namespace {
// width of the saved image, the preview is rendered scaled to the dialog width
const int IMAGE_WIDTH = 1080;
// margin around the image content
const int IMAGE_MARGIN = 48;

// plain text content may still contain markup, which QTextEdit::setText detects in the ContentDialog
QString
contentToHtml(const QString& text, bool isText)
{
  if (!isText || Qt::mightBeRichText(text))
    return text;
  return text.toHtmlEscaped().replace('\n', "<br>");
}

// recolors the black on white surah frame of the reader to a color on a transparent background
QImage
tintedSurahFrame(int surah, const QColor& color)
{
  const QImage frame = QuranPageBrowser::surahFrame(surah).convertToFormat(QImage::Format_ARGB32);
  QImage tinted(frame.size(), QImage::Format_ARGB32);
  for (int y = 0; y < frame.height(); y++) {
    const QRgb* src = reinterpret_cast<const QRgb*>(frame.constScanLine(y));
    QRgb* dst = reinterpret_cast<QRgb*>(tinted.scanLine(y));
    for (int x = 0; x < frame.width(); x++)
      dst[x] = qRgba(color.red(), color.green(), color.blue(), qAlpha(src[x]) * (255 - qGray(src[x])) / 255);
  }
  return tinted;
}
}

ShareImageDialog::ShareImageDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::ShareImageDialog)
  , m_currVerse(Verse::getCurrent())
  , m_verse(m_currVerse)
  , m_quranService(ServiceFactory::quranService())
  , m_tafsirService(ServiceFactory::tafsirService())
  , m_translationService(ServiceFactory::translationService())
{
  ui->setupUi(this);
  for (QAbstractButton* btn : ui->buttonBox->buttons())
    btn->setIcon(QIcon());

  ui->cmbTheme->setCurrentIndex(Configuration::getInstance().themeId());

  m_doc.setDocumentMargin(IMAGE_MARGIN);
  m_doc.setTextWidth(IMAGE_WIDTH);

  connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked, this, &ShareImageDialog::saveImage);
  connect(ui->cmbFrom, &QComboBox::currentIndexChanged, this, &ShareImageDialog::updatePreview);
  connect(ui->cmbTo, &QComboBox::currentIndexChanged, this, &ShareImageDialog::updatePreview);
  connect(ui->cmbContent, &QComboBox::currentIndexChanged, this, &ShareImageDialog::updatePreview);
  connect(ui->cmbTheme, &QComboBox::currentIndexChanged, this, &ShareImageDialog::updatePreview);
  connect(ui->spnVerseSize, &QSpinBox::valueChanged, this, &ShareImageDialog::updatePreview);
  connect(ui->spnContentSize, &QSpinBox::valueChanged, this, &ShareImageDialog::updatePreview);
  Navigator::getInstance().addObserver(this);
}

void
ShareImageDialog::populateContent()
{
  const int kind = ui->cmbContent->currentData(KindRole).toInt();
  const QString id = ui->cmbContent->currentData(IdRole).toString();

  ui->cmbContent->clear();
  ui->cmbContent->addItem(tr("Nothing"), NoContent);
  for (const ::Tafsir& tafsir : std::as_const(Tafsir::tafasir)) {
    if (!tafsir.isAvailable())
      continue;
    ui->cmbContent->addItem(tr("Tafsir: %1").arg(tafsir.displayName()), TafsirContent);
    ui->cmbContent->setItemData(ui->cmbContent->count() - 1, tafsir.id(), IdRole);
  }
  for (const ::Translation& translation : std::as_const(Translation::translations)) {
    if (!translation.isAvailable())
      continue;
    ui->cmbContent->addItem(tr("Translation: %1").arg(translation.displayName()), TranslationContent);
    ui->cmbContent->setItemData(ui->cmbContent->count() - 1, translation.id(), IdRole);
  }

  int idx = 0;
  for (int i = 1; i < ui->cmbContent->count(); i++) {
    if (ui->cmbContent->itemData(i, KindRole).toInt() == kind && ui->cmbContent->itemData(i, IdRole) == id)
      idx = i;
  }
  ui->cmbContent->setCurrentIndex(idx);
}

QStringList
ShareImageDialog::contentHtml(int from, int to, QString& name)
{
  const int surah = m_verse.surah();
  const QString id = ui->cmbContent->currentData(IdRole).toString();
  QStringList content;

  // the services share a single current tafsir/translation with the rest of the app, restore it after reading
  switch (ui->cmbContent->currentData(KindRole).toInt()) {
    case TafsirContent: {
      const ::Tafsir prev = m_tafsirService->currTafsir();
      const ::Tafsir tafsir = Tafsir::findById(id).value();
      if (!m_tafsirService->setCurrentTafsir(tafsir))
        return content;
      for (int i = from; i <= to; i++) {
        content.append(contentToHtml(m_tafsirService->getTafsir(surah, i), tafsir.isText()));
      }
      m_tafsirService->setCurrentTafsir(prev);
      name = tafsir.displayName();
      break;
    }
    case TranslationContent: {
      const ::Translation prev = m_translationService->currTranslation();
      const ::Translation translation = Translation::findById(id).value();
      if (!m_translationService->setCurrentTranslation(translation))
        return content;
      for (int i = from; i <= to; i++)
        content.append(contentToHtml(m_translationService->getTranslation(surah, i), true));
      m_translationService->setCurrentTranslation(prev);
      // translation names are prefixed with their language, e.g. "English - Clear Quran"
      name = translation.displayName();
      if (int sep = name.indexOf(" - "); sep >= 0)
        name = name.mid(sep + 3);
      break;
    }
  }

  return content;
}

void
ShareImageDialog::updatePreview()
{
  int from = ui->cmbFrom->currentIndex() + 1;
  int to = ui->cmbTo->currentIndex() + 1;
  bool valid = from > 0 && to >= from;
  ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(valid);
  if (!valid) {
    m_doc.clear();
    ui->lbPreview->setText(tr("The selected verse range is invalid"));
    return;
  }

  const int surah = m_verse.surah();
  ConfigurationSchema::VerseType type = Configuration::getInstance().verseType();
  // QCF glyphs are page-specific, plain text is rendered with the Hafs font instead
  if (type == ConfigurationSchema::Qcf)
    type = ConfigurationSchema::Hafs;
  QString verseFont = FontManager::getInstance().verseFontname(type, 0);

  QString contentName;
  const QStringList content = contentHtml(from, to, contentName);
  ui->spnContentSize->setEnabled(ui->cmbContent->currentData(KindRole).toInt() != NoContent);

  // the canonical mushaf surah header, in the theme text color
  const QPalette palette = StyleManager::themePalette(ui->cmbTheme->currentIndex());
  m_doc.addResource(QTextDocument::ImageResource, QUrl("surah-frame"), tintedSurahFrame(surah, palette.color(QPalette::Text)));
  // the header spans the content width at the default verse size (40px) and scales with it, up to the full width
  const int contentWidth = IMAGE_WIDTH - 2 * IMAGE_MARGIN;
  const int headerWidth = qMin(contentWidth, contentWidth * ui->spnVerseSize->value() / 40);
  QString html = QString("<p align='center'><img src='surah-frame' width='%1'></p>").arg(headerWidth);
  if (content.isEmpty())
    html += "<p class='verse' align='center'>";
  for (int i = from; i <= to; i++) {
    QString verse = m_quranService->verseText(surah, i).toHtmlEscaped();
    if (content.isEmpty())
      html += verse + ' ';
    else
      html += "<p class='verse' align='center'>" + verse + "</p><p>" + content.at(i - from) + "</p><hr>";
  }
  if (content.isEmpty())
    html += "</p>";

  // the citation follows the UI language, including its digits
  const QLocale locale(Configuration::getInstance().language());
  QString citation = m_quranService->surahName(surah) + ' ' + locale.toString(from);
  if (to > from)
    citation += '-' + locale.toString(to);
  if (!contentName.isEmpty())
    citation += " - " + contentName;
  html += "<p align='center' class='info'>" + citation.toHtmlEscaped() + "</p>";
  html += "<p align='center' class='watermark'><img src=':/resources/tray.png' width='32' height='32' "
          "style='vertical-align: middle'>&nbsp;&nbsp;" +
          tr("Quran Companion").toHtmlEscaped() + "</p>";

  const QString muted = palette.color(QPalette::PlaceholderText).name();
  // the content has no class of its own, as tafsir HTML brings its own paragraphs, so it uses the default font
  QFont contentFont = m_doc.defaultFont();
  contentFont.setPixelSize(ui->spnContentSize->value());
  m_doc.setDefaultFont(contentFont);
  m_doc.setDefaultStyleSheet(QString(".verse { font-family: '%1'; font-size: %2px; }"
                                     ".info { color: %3; font-size: 18px; }"
                                     ".watermark { color: %3; font-size: 16px; margin-top: 24px; }")
                               .arg(verseFont)
                               .arg(ui->spnVerseSize->value())
                               .arg(muted));
  m_doc.setHtml(html);
  showPreview();
}

QImage
ShareImageDialog::renderImage(qreal scale) const
{
  // colors follow the selected app theme: Base is the background of the reader, like in the main window
  QAbstractTextDocumentLayout::PaintContext ctx;
  ctx.palette = StyleManager::themePalette(ui->cmbTheme->currentIndex());

  // ponytail: a single image, very long ranges with tafsir produce very tall images
  QImage img((m_doc.size() * scale).toSize(), QImage::Format_RGB32);
  img.fill(ctx.palette.color(QPalette::Base));
  QPainter painter(&img);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  // text is drawn as vectors at the target size instead of scaling a bitmap
  painter.scale(scale, scale);
  m_doc.documentLayout()->draw(&painter, ctx);
  return img;
}

void
ShareImageDialog::showPreview()
{
  if (m_doc.isEmpty())
    return;

  const qreal dpr = devicePixelRatioF();
  const int frame = 2 * ui->scrlPreview->frameWidth();
  // the preview grows with the image to keep the dialog compact, and scrolls once it would outgrow the screen
  const int maxHeight = screen()->availableGeometry().height() * 0.85 - (height() - ui->scrlPreview->height());
  int width = ui->scrlPreview->width() - frame;
  const bool scroll = m_doc.size().height() * width / IMAGE_WIDTH + frame > maxHeight;
  if (scroll)
    width -= ui->scrlPreview->verticalScrollBar()->sizeHint().width();
  ui->scrlPreview->setVerticalScrollBarPolicy(scroll ? Qt::ScrollBarAlwaysOn : Qt::ScrollBarAlwaysOff);

  QImage img = renderImage(width * dpr / IMAGE_WIDTH);
  img.setDevicePixelRatio(dpr);
  ui->lbPreview->setPixmap(QPixmap::fromImage(img));
  ui->scrlPreview->setFixedHeight(qMin(qCeil(img.height() / dpr) + frame, maxHeight));
  resize(this->width(), minimumSizeHint().height());
}

void
ShareImageDialog::saveImage()
{
  int from = ui->cmbFrom->currentIndex() + 1;
  int to = ui->cmbTo->currentIndex() + 1;
  QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) +
                        QString("/quran-%1-%2-%3.png").arg(m_verse.surah()).arg(from).arg(to);
  QString path = QFileDialog::getSaveFileName(this, tr("Save image"), defaultPath, "PNG (*.png)");
  if (path.isEmpty())
    return;
  if (!renderImage(1).save(path, "PNG"))
    QMessageBox::warning(this, tr("Error"), tr("Couldn't save the image"));
}

void
ShareImageDialog::show()
{
  showVerse(m_currVerse);
}

void
ShareImageDialog::showVerse(const Verse& verse)
{
  m_verse = verse;
  ui->lbSurahName->setText(m_quranService->surahName(m_verse.surah()));

  // block signals to render the preview once after populating
  const QList<QComboBox*> combos = { ui->cmbFrom, ui->cmbTo, ui->cmbContent };
  for (QComboBox* cmb : combos)
    cmb->blockSignals(true);
  ui->cmbFrom->clear();
  ui->cmbTo->clear();
  for (int i = 1; i <= m_verse.surahCount(); i++) {
    ui->cmbFrom->addItem(QString::number(i));
    ui->cmbTo->addItem(QString::number(i));
  }

  int idx = m_verse.number() ? m_verse.number() - 1 : 0;
  ui->cmbFrom->setCurrentIndex(idx);
  ui->cmbTo->setCurrentIndex(idx);
  populateContent();
  for (QComboBox* cmb : combos)
    cmb->blockSignals(false);

  QDialog::show();
  updatePreview();
}

void
ShareImageDialog::activeVerseChanged()
{
  if (!isVisible())
    return;
  // follow the highlighted verse, unless a range was picked in the same surah
  if (m_currVerse.surah() != m_verse.surah() || ui->cmbFrom->currentIndex() == ui->cmbTo->currentIndex())
    showVerse(m_currVerse);
}

void
ShareImageDialog::resizeEvent(QResizeEvent* event)
{
  QDialog::resizeEvent(event);
  showPreview();
}

ShareImageDialog::~ShareImageDialog()
{
  Navigator::getInstance().removeObserver(this);
  delete ui;
}
