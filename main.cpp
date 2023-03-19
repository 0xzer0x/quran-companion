#include "headers/interfaces/mainwindow.h"

#include <QApplication>
#include <QFontDatabase>
#include <QLocale>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("Quran Companion");
    QApplication::setOrganizationName("Fursan Team");

    qApp->setStyle(QStyleFactory::create("Fusion"));
    QSettings appSettings("qc-config.ini", QSettings::Format::IniFormat, &a);

    if (!appSettings.contains("Language")) {
        appSettings.setValue("Language", "English");
        appSettings.setValue("Theme", "Light");

        appSettings.beginGroup("Reader");

        appSettings.setValue("Page", 1);
        appSettings.setValue("Surah", 1);
        appSettings.setValue("Verse", 1);

        appSettings.setValue("QuranFontSize", 22);
        appSettings.setValue("SideContentFont", QFont("Calibri", 14));
        appSettings.setValue("CopyVerseOnClick", true);

        appSettings.endGroup();
    }
    if (appSettings.value("Theme").toString() == "Dark") {
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
        darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::Dark, QColor(35, 35, 35));
        darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
        darkPalette.setColor(QPalette::HighlightedText, Qt::white);
        darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));

        qApp->setPalette(darkPalette);
    }

    // add required fonts
    QDir fontsDir = QApplication::applicationDirPath() + QDir::separator() + "assets"
                    + QDir::separator() + "fonts";
    QFontDatabase::addApplicationFont(fontsDir.filePath("QCF_BSML.ttf"));
    QFontDatabase::addApplicationFont(fontsDir.filePath("Amiri.ttf"));

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

    // uncomment to switch to Arabic

    QTranslator trs;

    if (appSettings.value("Language").toString() != "English") {
        if (trs.load(":/i18n/quran_companion_ar")) {
            qInfo() << "Tr" << trs.language() << "loaded";
            a.installTranslator(&trs);
        } else {
            qWarning() << "AR Translation not loaded!";
        }
    }

    MainWindow w(nullptr, &appSettings);
    w.show();
    return a.exec();
}
