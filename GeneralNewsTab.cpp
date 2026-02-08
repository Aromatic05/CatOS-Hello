#include "GeneralNewsTab.h"
#include <QApplication>
#include <QLocale>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QDebug>

GeneralNewsTab::GeneralNewsTab(QWidget *parent)
    : QWidget(parent)
{
    bool inLiveCd = isLiveCd();
    layout = new QVBoxLayout(this);
    generalNewsLabel = new QLabel(tr("General News"), this);
    generalNewsLabel->setSizePolicy(generalNewsLabel->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);

    gridLayout = new QGridLayout();

    websiteButton = new QPushButton(tr("Visit Website"));
    archButton = new QPushButton(tr("Visit ArchLinux Website"));
    wikiButton = new QPushButton(tr("Visit ArchLinux Wiki"));
    wikicnButton = new QPushButton(tr("Visit ArchLinuxCN Wiki"));
    supportButton = new QPushButton(tr("Support us on Distrowatch.com"));
    languageComboBox = new QComboBox();
    applyButton = new QPushButton(tr("Apply"));
    mirrorButton = new QPushButton(tr("Modify Mirror List"));
    repoButton = new QPushButton(tr("Modify Repo List"));

    if (inLiveCd) {
        installCatOSButton = new QPushButton(tr("Install CatOS"));
        installCatOSNetButton = new QPushButton(tr("Install CatOS Net Install"));
        getLogButton = new QPushButton(tr("Get Install Log"));
    }

    // 加载语言
    loadLanguages();

    // 连接信号与槽
    connect(applyButton, SIGNAL(clicked()), this, SLOT(onApplyButtonClicked()));
    connect(websiteButton, SIGNAL(clicked()), this, SLOT(onWebsiteButtonClicked()));
    connect(archButton, SIGNAL(clicked()), this, SLOT(onArchWebsiteButtonClicked()));
    connect(wikiButton, SIGNAL(clicked()), this, SLOT(onWikiWebsiteButtonClicked()));
    connect(wikicnButton, SIGNAL(clicked()), this, SLOT(onWikicnWebsiteButtonClicked()));
    connect(mirrorButton, SIGNAL(clicked()), this, SLOT(onMirrorButtonClicked()));
    connect(repoButton, SIGNAL(clicked()), this, SLOT(onRepoButtonClicked()));
    connect(supportButton, SIGNAL(clicked()), this, SLOT(onSupportButtonClicked()));
    if (inLiveCd) {
        connect(installCatOSButton, SIGNAL(clicked()), this, SLOT(onInstallCatOSButtonClicked()));
        connect(installCatOSNetButton, SIGNAL(clicked()), this, SLOT(onInstallCatOSNetButtonClicked()));
        connect(getLogButton, SIGNAL(clicked()), this, SLOT(onGetLogButtonClicked()));
    }
    // 布局
    gridLayout->addWidget(languageComboBox, 0, 0);
    gridLayout->addWidget(applyButton, 0, 1);
    gridLayout->addWidget(websiteButton, 1, 0);
    gridLayout->addWidget(archButton, 1, 1);
    gridLayout->addWidget(wikicnButton, 2, 0);
    gridLayout->addWidget(wikiButton, 2, 1);
    gridLayout->addWidget(mirrorButton, 3, 0);
    gridLayout->addWidget(repoButton, 3, 1);
    gridLayout->addWidget(supportButton, 4, 0);
    if (inLiveCd) {
        gridLayout->addWidget(installCatOSButton, 5, 0);
        gridLayout->addWidget(installCatOSNetButton, 5, 1);
        gridLayout->addWidget(getLogButton, 6, 0);
    }

    layout->addWidget(generalNewsLabel, 0);
    layout->addLayout(gridLayout);
    layout->addWidget(new QLabel("", this)); // 空白标签
}

void GeneralNewsTab::loadLanguages()
{
    qInfo() << "GeneralNewsTab: loadLanguages start";
    // 定义语言列表，包括地区信息
    QList<QLocale> locales = {
        QLocale(QLocale::English), QLocale(QLocale::Chinese, QLocale::China),
        QLocale(QLocale::Spanish), QLocale(QLocale::French),
    };

    // 向下拉框中添加语言
    for (const QLocale &locale : locales) {
        QString nativeName = locale.nativeLanguageName();
        QString countryName = locale.nativeTerritoryName();
        QString languageCode = locale.name();
        QString displayText = nativeName + " (" + countryName + ")";
        languageComboBox->addItem(displayText, languageCode);
    }

    // 设置默认值为当前语言
    int currentIndex = languageComboBox->findData(lang);
    if (currentIndex != -1) {
        languageComboBox->setCurrentIndex(currentIndex);
    }
    qInfo() << "GeneralNewsTab: loadLanguages done";
}

void GeneralNewsTab::applyLanguageChange(const QString &langCode)
{
    qInfo() << "GeneralNewsTab: applyLanguageChange" << langCode;
    restartApplication(langCode);
}

bool GeneralNewsTab::isLiveCd() const
{
    const bool liveCd = QFile::exists("/bin/calamares");
    qInfo() << "GeneralNewsTab: isLiveCd" << liveCd;
    return liveCd;
}

void GeneralNewsTab::restartApplication(const QString &langCode)
{
    qInfo() << "GeneralNewsTab: restartApplication" << langCode;
    QString program = qApp->arguments().first();
    QStringList arguments;
    arguments << langCode;
    QProcess::startDetached(program, arguments);
    QApplication::quit();
}

void GeneralNewsTab::onApplyButtonClicked()
{
    QString langCode = languageComboBox->currentData().toString();
    qInfo() << "GeneralNewsTab: onApplyButtonClicked" << langCode;
    applyLanguageChange(langCode);
}

void GeneralNewsTab::onWebsiteButtonClicked()
{
    qInfo() << "GeneralNewsTab: open website";
    QDesktopServices::openUrl(QUrl("https://www.catos.info/"));
}

void GeneralNewsTab::onArchWebsiteButtonClicked()
{
    qInfo() << "GeneralNewsTab: open ArchLinux website";
    QDesktopServices::openUrl(QUrl("https://archlinux.org/"));
}

void GeneralNewsTab::onWikiWebsiteButtonClicked()
{
    qInfo() << "GeneralNewsTab: open ArchLinux wiki";
    QDesktopServices::openUrl(QUrl("https://wiki.archlinux.org/"));
}

void GeneralNewsTab::onWikicnWebsiteButtonClicked()
{
    qInfo() << "GeneralNewsTab: open ArchLinuxCN wiki";
    QDesktopServices::openUrl(QUrl("https://wiki.archlinuxcn.org/"));
}

void GeneralNewsTab::onSupportButtonClicked()
{
    qInfo() << "GeneralNewsTab: open Distrowatch support";
    QDesktopServices::openUrl(QUrl("https://distrowatch.com/dwres-mobile.php?resource=links#new"));
}

void GeneralNewsTab::onMirrorButtonClicked()
{
    qInfo() << "GeneralNewsTab: open MirrorListWindow";
    // 创建并显示 MirrorListWindow
    QScopedPointer mirrorWindow(new MirrorListWindow(this));
    mirrorWindow->exec();
}

void GeneralNewsTab::onRepoButtonClicked()
{
    qInfo() << "GeneralNewsTab: open RepoListWindow";
    QScopedPointer repoWindow(new RepoListWindow(this));
    repoWindow->exec();
}

void GeneralNewsTab::onInstallCatOSButtonClicked()
{
    qInfo() << "GeneralNewsTab: start calamares_polkit";
    QProcess::startDetached("calamares_polkit");
}

void GeneralNewsTab::onInstallCatOSNetButtonClicked()
{
    qInfo() << "GeneralNewsTab: start calamares_polkit net install";
    QProcess::startDetached("calamares_polkit", {"--config", "/usr/share/calamares-advanced"});
}

void GeneralNewsTab::onGetLogButtonClicked()
{
    qInfo() << "GeneralNewsTab: get calamares logs";
    const QString destDir = QStringLiteral("/tmp/calamares-log");
    const QString copyCmd = QStringLiteral("rm -rf %1 && cp -r /root/.cache/calamares %1").arg(destDir);

    const int copyExit = QProcess::execute("pkexec", {"bash", "-c", copyCmd});
    if (copyExit == 0) {
        qInfo() << "GeneralNewsTab: calamares logs copied" << destDir;
        QProcess::startDetached("bash", {"-c", QStringLiteral("kate %1/*").arg(destDir)});
    } else {
        qWarning() << "GeneralNewsTab: failed to copy calamares logs" << copyExit;
        QMessageBox::critical(this, tr("Error"), tr("Failed to copy calamares logs"));
    }
}
