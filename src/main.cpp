#include <QApplication>
#include <QFontDatabase>
#include <QLocale>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>
#include "core/mainwindow.h"
#include "utils/logger.h"

void setTheme(int themeIdx);

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

        appSettings.setValue("QuranFontSize", 22);
        appSettings.setValue("SideContentFont", QFont("Droid Sans Arabic", 14));
        appSettings.setValue("CopyVerseOnClick", true);
        appSettings.endGroup();
    }

    setTheme(appSettings.value("Theme").toInt());

    // add required fonts
    QDir fontsDir = QApplication::applicationDirPath() + QDir::separator() + "assets"
                    + QDir::separator() + "fonts";
    QFontDatabase::addApplicationFont(fontsDir.filePath("QCF_BSML.ttf"));
    QFontDatabase::addApplicationFont(fontsDir.filePath("Amiri.ttf"));
    QFontDatabase::addApplicationFont(fontsDir.filePath("droid-sans-org.ttf"));
    for (int i = 1; i < 605; i++) {
        QString font = "QCF_P";
        if (i < 10)
            font.append("00");
        else if (i < 100)
            font.append("0");

        font.append(QString::number(i));
        font.append(".ttf");

        QFontDatabase::addApplicationFont(fontsDir.filePath(font));
    }

    // add translation
    QTranslator trs, qtTranslation;
    QString baseQtTr = QApplication::applicationDirPath() + QDir::separator() + "translations"
                       + QDir::separator();

    if (appSettings.value("Language").toString() != "English") {
        if (trs.load(":/i18n/quran_companion_ar.qm")) {
            qInfo() << "Tr" << trs.language() << "loaded";
            qInfo() << qtTranslation.load(baseQtTr + "qt_ar");

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

void setTheme(int themeIdx)
{
    QPalette themeColors;
    switch (themeIdx) {
    case 0:
        break;

    case 1:
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
