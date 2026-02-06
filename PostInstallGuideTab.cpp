#include "PostInstallGuideTab.h"

#include <QDesktopServices>
#include <QProcess>

PostInstallGuideTab::PostInstallGuideTab(QWidget *parent)
    : QWidget(parent)
{
    // 创建布局和控件
    layout = new QVBoxLayout(this);
    postInstallGuideLabel = new QLabel(tr("You can get help from these manuals."), this);
    postInstallGuideLabel->setSizePolicy(postInstallGuideLabel->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);

    gridLayout = new QGridLayout;
    pacmanButton = new QPushButton(tr("Correspondence between package managers"), this);
    logButton = new QPushButton(tr("Troubleshooting Log"), this);
    mirrorButton = new QPushButton(tr("Modify Mirror List"), this);
    updateAURButton = new QPushButton(tr("Update Native & AUR Packages"), this);
    packageCleanButton = new QPushButton(tr("Enable Automatic Package Cache Cleanup"), this);
    updateButton = new QPushButton(tr("Update Native Packages"), this);
    driverConfigButton = new QPushButton(tr("Driver Configuration"), this);

    connect(pacmanButton, SIGNAL(clicked()), this, SLOT(onPacmanButtonClicked()));
    connect(driverConfigButton, SIGNAL(clicked()), this, SLOT(onDriverConfigButtonClicked()));

    // 添加控件到布局
    gridLayout->addWidget(mirrorButton, 0, 0);
    gridLayout->addWidget(pacmanButton, 0, 1);
    gridLayout->addWidget(updateButton, 1, 1);
    gridLayout->addWidget(updateAURButton, 2, 0);
    gridLayout->addWidget(packageCleanButton, 2, 1);
    gridLayout->addWidget(driverConfigButton, 3, 0);
    gridLayout->addWidget(logButton, 3, 1);

    layout->addWidget(postInstallGuideLabel, 0);
    layout->addLayout(gridLayout);
    layout->addWidget(new QLabel("", this)); // 空白标签

    // 设置布局
    setLayout(layout);
}

void PostInstallGuideTab::onPacmanButtonClicked()
{
    QDesktopServices::openUrl(QUrl("https://wiki.archlinuxcn.org/wiki/Pacman/%E5%90%84%E8%BD%AF%E4%BB%B6%E5%8C%85%E7%AE%A1%E7%90%86%E5%99%A8%E5%91%BD%E4%BB%A4%E5%AF%B9%E5%BA%94%E5%85%B3%E7%B3%BB"));
}

void PostInstallGuideTab::onDriverConfigButtonClicked()
{
    QString command = "sudo chwd -a";
    QString prompt = tr("Install and configure drivers");
    QStringList args;
    args << "--prompt" << prompt << command;
    QProcess::startDetached("RunInTerminal", args);
}
