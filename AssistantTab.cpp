#include "AssistantTab.h"
#include <QTimer>

AssistantTab::AssistantTab(QWidget *parent)
    : QWidget(parent)
{
    // 创建布局和控件
    layout = new QVBoxLayout(this);
    assistantLabel = new QLabel(tr("Get help from the assistant."), this);
    assistantLabel->setSizePolicy(assistantLabel->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);

    gridLayout = new QGridLayout;
    mirrorButton = new QPushButton(tr("Modify Mirror List"));
    repoButton = new QPushButton(tr("Modify Repo List"));
    updateButton = new QPushButton(tr("Update Native Packages"));
    updateAURButton = new QPushButton(tr("Update Native Packages && AUR Packages"));
    resetButton = new QPushButton(tr("Reset Arch GPG keys"));
    cleanButton = new QPushButton(tr("Clean up all local packages caches"));
    reduceButton = new QPushButton(tr("Clean local caches, except for the most recent three"));
    cleanAURButton = new QPushButton(tr("Clean up all local packages and AUR caches"));
    uninstallButton = new QPushButton(tr("Uninstall unused packages"));
    reinstallButton = new QPushButton(tr("Reinstall all packages"));

    // 连接信号和槽
    connect(mirrorButton, SIGNAL(clicked()), this, SLOT(onMirrorButtonClicked()));
    connect(repoButton, SIGNAL(clicked()), this, SLOT(onRepoButtonClicked()));
    connect(updateButton, SIGNAL(clicked()), this, SLOT(onUpdateButtonClicked()));
    connect(updateAURButton, SIGNAL(clicked()), this, SLOT(onUpdateAURButtonClicked()));
    connect(resetButton, SIGNAL(clicked()), this, SLOT(onResetButtonClicked()));
    connect(cleanButton, SIGNAL(clicked()), this, SLOT(onCleanButtonClicked()));
    connect(reduceButton, SIGNAL(clicked()), this, SLOT(onReduceButtonClicked()));
    connect(cleanAURButton, SIGNAL(clicked()), this, SLOT(onCleanAURButtonClicked()));
    connect(uninstallButton, SIGNAL(clicked()), this, SLOT(onUninstallButtonClicked()));
    connect(reinstallButton, SIGNAL(clicked()), this, SLOT(onReinstallButtonClicked()));

    // 添加控件到布局
    gridLayout->addWidget(mirrorButton, 0, 0);
    gridLayout->addWidget(repoButton, 0, 1);
    gridLayout->addWidget(updateButton, 1, 0);
    gridLayout->addWidget(updateAURButton, 1, 1);
    gridLayout->addWidget(cleanButton, 2, 0);
    gridLayout->addWidget(reduceButton, 2, 1);
    gridLayout->addWidget(cleanAURButton, 3, 0);
    gridLayout->addWidget(uninstallButton, 3, 1);
    gridLayout->addWidget(reinstallButton, 4, 0);
    gridLayout->addWidget(resetButton, 4, 1);
    layout->addWidget(assistantLabel, 0);
    layout->addLayout(gridLayout);
    layout->addWidget(new QLabel("", this)); // 空白标签

    // 设置布局
    setLayout(layout);
}

void AssistantTab::onMirrorButtonClicked()
{
    QScopedPointer mirrorWindow(new MirrorListWindow(this));
    mirrorWindow->exec();
}

void AssistantTab::onRepoButtonClicked()
{
    QScopedPointer repoWindow(new RepoListWindow(this));
    repoWindow->exec();
}

void AssistantTab::onUpdateButtonClicked() {
    QString command = "sudo pacman -Syu";
    QString prompt = tr("Update Native Packages");
    QStringList args;
    args << "--prompt" << prompt << command;
    QProcess::startDetached("/home/aromatic/Application/RunInTerminal", args);
}

void AssistantTab::onUpdateAURButtonClicked() {
    QString command = "yay";
    QString prompt = tr("Update Native Packages && AUR Packages");
    QStringList args;
    args << "--prompt" << prompt << command;
    QProcess::startDetached("/home/aromatic/Application/RunInTerminal", args);
}

void AssistantTab::onResetButtonClicked() {
    // 创建临时脚本文件
    QString scriptFilePath = QDir::tempPath() + "/reset_keyring.sh";
    QFile scriptFile(scriptFilePath);

    if (!scriptFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", "Failed to create script file.");
        return;
    }

    // 写入脚本内容
    QTextStream stream(&scriptFile);
    stream << "#!/bin/bash\n";
    stream << "echo 'Resetting Arch Linux keyring...'\n";
    stream << "sudo rm -rf /etc/pacman.d/gnupg\n";
    stream << "sudo pacman-key --init\n";
    stream << "sudo pacman-key --populate archlinux\n";
    stream << "sudo pacman-key --refresh-keys\n";
    stream << "echo 'Keyring reset completed.'\n";
    stream.flush();
    scriptFile.close();

    QFile::setPermissions(scriptFile.fileName(), QFile::ExeUser | QFile::ReadUser | QFile::WriteUser);
    QStringList args;
    args << "--prompt" << "Reset Arch Keyring" << scriptFilePath;

    QProcess::startDetached("/home/aromatic/Application/RunInTerminal", args);
}

void AssistantTab::onCleanButtonClicked() {
    QString command = "sudo pacman -Scc";
    QString prompt = tr("Clean up all local packages caches");
    QStringList args;
    args << "--prompt" << prompt << command;
    QProcess::startDetached("/home/aromatic/Application/RunInTerminal", args);
}

void AssistantTab::onReduceButtonClicked() {
    QString command = "sudo paccache -rk3";
    QString prompt = tr("Clean local packages caches, except for the most recent three");
    QStringList args;
    args << "--prompt" << prompt << command;
    QProcess::startDetached("/home/aromatic/Application/RunInTerminal", args);
}

void AssistantTab::onCleanAURButtonClicked() {
    QString command = "yay -Scc";
    QString prompt = tr("Clean up all local packages and AUR caches");
    QStringList args;
    args << "--prompt" << prompt << command;
    QProcess::startDetached("/home/aromatic/Application/RunInTerminal", args);
}

void AssistantTab::onUninstallButtonClicked() {
    QString command = "sudo pacman -Rns $(pacman -Qtdq)";
    QString prompt = tr("Uninstall unused packages");
    QStringList args;
    args << "--prompt" << prompt << command;
    QProcess::startDetached("/home/aromatic/Application/RunInTerminal", args);
}

void AssistantTab::onReinstallButtonClicked() {
    QString command = "pacman -Qqn | sudo pacman --overwrite=* -S -";
    QString prompt = tr("Reinstall all packages");
    QStringList args;
    args << "--prompt" << prompt << command;
    QProcess::startDetached("/home/aromatic/Application/RunInTerminal", args);
}