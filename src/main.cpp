#include <QApplication>
#include <QFontDatabase>
#include <QLocale>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>
#include "core/mainwindow.h"
#include "utils/logger.h"

void setTheme(int themeIdx);

void addFonts(int qcfVersion);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Quran Companion");
    QApplication::setOrganizationName("0xzer0x");

    Logger::startLogger();
    Logger::attach();

    qApp->setStyle(QStyleFactory::create("Fusion"));
    QSettings appSettings("qc-config.ini", QSettings::Format::IniFormat, &a);

    if (!appSettings.contains("Language")) {
        appSettings.setValue("Language", "English");
        appSettings.setValue("Theme", 0);

        appSettings.beginGroup("Reader");
        appSettings.setValue("Page", 1);
        appSettings.setValue("Surah", 1);
        appSettings.setValue("Verse", 1);

        appSettings.setValue("QCF1Size", 22);
        appSettings.setValue("QCF2Size", 20);
        appSettings.setValue("QCF", 1);
        appSettings.setValue("SideContentFont", QFont("Droid Sans Arabic", 14));
        appSettings.setValue("CopyVerseOnClick", true);
        appSettings.endGroup();
    }

    setTheme(appSettings.value("Theme").toInt());
    addFonts(appSettings.value("Reader/QCF").toInt());
    // add translation
    QTranslator trs, qtTranslation;
    QDir baseQtTr = QApplication::applicationDirPath() + QDir::separator() + "translations"
                    + QDir::separator();

    if (appSettings.value("Language").toString() == "العربية") {
        if (trs.load(":/i18n/quran_companion_ar.qm")) {
            qInfo() << "Tr" << trs.language() << "loaded";
            qInfo() << "Qt tr status: " << qtTranslation.load(baseQtTr.filePath("qt_ar.qm"));

            a.installTranslator(&trs);
            a.installTranslator(&qtTranslation);

            qApp->setFont(QFont("Droid Sans Arabic", qApp->font().pointSize()));
        } else {
            qWarning() << "AR Translation not loaded!";
        }
    }

    MainWindow w(nullptr, &appSettings);
    w.show();

    int ret = a.exec();
    Logger::stopLogger();

    return ret;
}

void addFonts(int qcfVersion)
{
    QDir fontsDir;
    QString fontBase;
    fontsDir = QApplication::applicationDirPath() + QDir::separator() + "assets" + QDir::separator()
               + "fonts";

    QFontDatabase::addApplicationFont(fontsDir.filePath("Amiri.ttf"));
    QFontDatabase::addApplicationFont(fontsDir.filePath("droid-sans-org.ttf"));

    switch (qcfVersion) {
    case 1:
        fontsDir.cd("QCFV1");
        fontBase = "QCF_P";
        QFontDatabase::addApplicationFont(fontsDir.filePath("QCF_BSML.ttf"));
        break;

    case 2:
        fontsDir.cd("QCFV2");
        fontBase = "QCF2";
        QFontDatabase::addApplicationFont(fontsDir.filePath("QCF2BSML.ttf"));
        break;
    }

    // add required fonts
    for (int i = 1; i < 605; i++) {
        QString fontName = fontBase;
        fontName.append(QString::number(i).rightJustified(3, '0'));
        fontName.append(".ttf");

        QFontDatabase::addApplicationFont(fontsDir.filePath(fontName));
    }
}

void setTheme(int themeIdx)
{
    QPalette themeColors;
    switch (themeIdx) {
    case 0: // light
        themeColors.setColor(QPalette::Window, QColor(240, 240, 240));
        themeColors.setColor(QPalette::WindowText, Qt::black);
        themeColors.setColor(QPalette::Disabled, QPalette::WindowText, QColor(120, 120, 120));
        themeColors.setColor(QPalette::Base, QColor(255, 255, 255));
        themeColors.setColor(QPalette::AlternateBase, QColor(233, 231, 227));
        themeColors.setColor(QPalette::ToolTipBase, Qt::black);
        themeColors.setColor(QPalette::ToolTipText, Qt::black);
        themeColors.setColor(QPalette::Text, Qt::black);
        themeColors.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));
        themeColors.setColor(QPalette::Dark, QColor(160, 160, 160));
        themeColors.setColor(QPalette::Shadow, QColor(105, 105, 105));
        themeColors.setColor(QPalette::Button, QColor(240, 240, 240));
        themeColors.setColor(QPalette::ButtonText, Qt::black);
        themeColors.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(120, 120, 120));
        themeColors.setColor(QPalette::BrightText, Qt::red);
        themeColors.setColor(QPalette::Link, QColor(0, 0, 255));
        themeColors.setColor(QPalette::Highlight, QColor(0, 120, 215));
        themeColors.setColor(QPalette::Disabled, QPalette::Highlight, QColor(0, 120, 215));
        themeColors.setColor(QPalette::HighlightedText, Qt::black);
        themeColors.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(255, 255, 255));
        qApp->setPalette(themeColors);
        break;

    case 1: // dark
        themeColors.setColor(QPalette::Window, QColor(53, 53, 53));
        themeColors.setColor(QPalette::WindowText, Qt::white);
        themeColors.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        themeColors.setColor(QPalette::Base, QColor(42, 42, 42));
        themeColors.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        themeColors.setColor(QPalette::ToolTipBase, Qt::white);
        themeColors.setColor(QPalette::ToolTipText, Qt::white);
        themeColors.setColor(QPalette::Text, Qt::white);
        themeColors.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        themeColors.setColor(QPalette::Dark, QColor(35, 35, 35));
        themeColors.setColor(QPalette::Shadow, QColor(20, 20, 20));
        themeColors.setColor(QPalette::Button, QColor(53, 53, 53));
        themeColors.setColor(QPalette::ButtonText, Qt::white);
        themeColors.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        themeColors.setColor(QPalette::BrightText, Qt::red);
        themeColors.setColor(QPalette::Link, QColor(42, 130, 218));
        themeColors.setColor(QPalette::Highlight, QColor(42, 130, 218));
        themeColors.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
        themeColors.setColor(QPalette::HighlightedText, Qt::white);
        themeColors.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
        qApp->setPalette(themeColors);
        break;

    default:
        break;
    }
}
