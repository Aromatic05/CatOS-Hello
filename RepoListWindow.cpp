#include "RepoListWindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QProcess>
#include <QTemporaryFile>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QListWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QInputDialog>
#include <QDateTime>

RepoListWindow::RepoListWindow(QWidget *parent) : QDialog(parent) {
    setupUI();
    loadConfig();
    baseWindowTitle = windowTitle(); // 保存基础标题
    updateWindowTitle();
}

void RepoListWindow::setupUI() {
    // 初始化控件
    repoList = new QListWidget(this);
    repoNameEdit = new QLineEdit(this);
    includeEdit = new QLineEdit(this);
    serversEdit = new QPlainTextEdit(this);
    sigLevelCombo = new QComboBox(this);
    sigLevelCombo->addItems({"Never", "Optional", "Required"});
    QPushButton *addRepoBtn = new QPushButton(tr("Add Repo"), this);
    QPushButton *deleteRepoBtn = new QPushButton(tr("Delete Repo"), this);
    QPushButton *saveBtn = new QPushButton(tr("Save"), this);

    // 布局管理
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *leftButtonLayout = new QVBoxLayout;
    leftButtonLayout->addWidget(addRepoBtn);
    leftButtonLayout->addWidget(deleteRepoBtn);
    leftButtonLayout->addStretch();

    QHBoxLayout *repoListLayout = new QHBoxLayout;
    repoListLayout->addWidget(repoList);
    repoListLayout->addLayout(leftButtonLayout);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("Repo Name:"), repoNameEdit);
    formLayout->addRow(tr("Include Path:"), includeEdit);
    formLayout->addRow(tr("Mirror Server:"), serversEdit);
    formLayout->addRow(tr("SigLevel:"), sigLevelCombo);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addLayout(formLayout);
    rightLayout->addStretch();
    rightLayout->addWidget(saveBtn);
    mainLayout->addLayout(repoListLayout, 1);
    mainLayout->addLayout(rightLayout, 2);

    setLayout(mainLayout);

    // 信号槽连接
    connect(repoList, &QListWidget::currentRowChanged, this, &RepoListWindow::onRepoSelected);
    connect(repoNameEdit, &QLineEdit::textEdited, this, &RepoListWindow::onRepoNameEdited);
    connect(includeEdit, &QLineEdit::textEdited, this, &RepoListWindow::onIncludeEdited);
    connect(serversEdit, &QPlainTextEdit::textChanged, this, &RepoListWindow::onServersEdited);
    connect(sigLevelCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RepoListWindow::onSigLevelChanged);
    connect(addRepoBtn, &QPushButton::clicked, this, &RepoListWindow::addRepo);
    connect(deleteRepoBtn, &QPushButton::clicked, this, &RepoListWindow::deleteRepo);
    connect(saveBtn, &QPushButton::clicked, this, &RepoListWindow::saveConfig);
}

void RepoListWindow::loadConfig() {
    QFile file("/etc/pacman.conf");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Fail to open configuration file"));
        return;
    }

    QTextStream in(&file);
    QStringList originalFileLines = in.readAll().split('\n');
    file.close();

    nonRepoLines.clear();
    origRepos.clear();
    repos.clear();

    Repo currentRepo;
    bool inRepoSection = false;

    auto saveCurrentRepo = [&]() {
        if (inRepoSection && !currentRepo.name.isEmpty()) {
            currentRepo.servers = currentRepo.origServers; // 同步服务器列表
            origRepos.append(currentRepo);
        }
    };

    for (const QString &line : originalFileLines) {
        QString trimmed = line.trimmed();

        if (trimmed.startsWith('[') && trimmed.endsWith(']')) {
            saveCurrentRepo(); // 保存上一个仓库

            QString sectionName = trimmed.mid(1, trimmed.length() - 2);
            if (sectionName == "options") {
                inRepoSection = false;
                nonRepoLines.append(line);
            } else {
                inRepoSection = true;
                currentRepo = Repo(); // 开始新的仓库
                currentRepo.name = sectionName;
                currentRepo.origName = sectionName;
            }
        } else if (inRepoSection) {
            if (trimmed.isEmpty() || trimmed.startsWith('#')) {
                continue; // 跳过仓库块内的空行和注释
            }

            if (trimmed.contains('=')) {
                QString key = trimmed.section('=', 0, 0).trimmed();
                QString value = trimmed.section('=', 1).trimmed();

                if (key == "Include") {
                    currentRepo.include = value;
                    currentRepo.origInclude = value;
                } else if (key == "Server") {
                    currentRepo.origServers.append(value);
                } else if (key == "SigLevel") {
                    currentRepo.sigLevel = value;
                    currentRepo.origSigLevel = value;
                }
            }
        } else {
            nonRepoLines.append(line);
        }
    }

    saveCurrentRepo(); // 保存文件中的最后一个仓库

    repos = origRepos; // 初始时，工作副本与原始副本相同

    // 更新UI列表
    repoList->clear();
    for (const Repo &repo : repos) {
        repoList->addItem(repo.name);
    }
    updateWindowTitle(); // 更新标题
}

bool RepoListWindow::hasChanges() const {
    if (repos.size() != origRepos.size()) return true;

    for (int i = 0; i < repos.size(); ++i) {
        const Repo &curr = repos[i];
        const Repo &orig = origRepos[i];

        if (curr.name != orig.origName ||
            curr.include != orig.origInclude ||
            curr.servers != orig.origServers ||
            curr.sigLevel != orig.origSigLevel) {
            return true;
        }
    }
    return false;
}

bool RepoListWindow::backupConfigFile() {
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString backupFile = QString("/etc/pacman.conf.bak_%1").arg(timestamp);

    QProcess process;
    process.start("pkexec", {"cp", "/etc/pacman.conf", backupFile});
    return process.waitForFinished() && process.exitCode() == 0;
}

bool RepoListWindow::writeConfigFile() {
    QFile file("/etc/pacman.conf");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream out(&file);
    out << nonRepoLines.join('\n') << '\n'; // 写入非仓库段

    for (const Repo &repo : repos) {
        out << "[" << repo.name << "]\n";
        if (!repo.sigLevel.isEmpty())
            out << "SigLevel = " << repo.sigLevel << '\n';
        if (!repo.include.isEmpty())
            out << "Include = " << repo.include << '\n';

        for (const QString &server : repo.servers) {
            out << "Server = " << server << '\n';
        }
        out << '\n'; // 段间分隔
    }

    file.close();
    return true;
}

void RepoListWindow::saveConfig() {
    if (!hasChanges()) {
        QMessageBox::information(this, tr("Note"), tr("No Change to Save"));
        return;
    }

    // 创建备份文件
    if (!backupConfigFile()) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to create backup file"));
        return;
    }

    // 保存新配置
    if (!writeConfigFile()) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save configuration file"));
        return;
    }

    origRepos = repos; // 更新原始数据
    updateWindowTitle(); // 更新标题
    QMessageBox::information(this, tr("Success"), tr("Configuration has been updated"));
}

void RepoListWindow::onRepoSelected(int row) {
    if (row < 0 || row >= repos.size()) return;

    const Repo &repo = repos[row];
    repoNameEdit->setText(repo.name);
    includeEdit->setText(repo.include);
    serversEdit->setPlainText(repo.servers.join('\n'));

    int comboIndex = sigLevelCombo->findText(repo.sigLevel);
    sigLevelCombo->setCurrentIndex(comboIndex >= 0 ? comboIndex : 1);
}

void RepoListWindow::onRepoNameEdited(const QString &text) {
    int row = repoList->currentRow();
    if (row < 0 || row >= repos.size()) return;

    repos[row].name = text;
    repoList->item(row)->setText(text);
    updateWindowTitle();
}

void RepoListWindow::onIncludeEdited(const QString &text) {
    int row = repoList->currentRow();
    if (row >= 0) repos[row].include = text;
    updateWindowTitle();
}

void RepoListWindow::onServersEdited() {
    int row = repoList->currentRow();
    if (row >= 0) repos[row].servers = serversEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
    updateWindowTitle();
}

void RepoListWindow::onSigLevelChanged(int index) {
    int row = repoList->currentRow();
    if (row >= 0) repos[row].sigLevel = sigLevelCombo->itemText(index);
    updateWindowTitle();
}

void RepoListWindow::addRepo() {
    bool ok;
    QString name = QInputDialog::getText(this, tr("Add Repo"), tr("Enter the Repo name:"), QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    if (name.contains(' ') || name.contains('[') || name.contains(']')) {
        QMessageBox::warning(this, tr("Error"), tr("Repo name contains invalid chars"));
        return;
    }

    Repo newRepo;
    newRepo.name = name;
    newRepo.origName = name;
    repos.append(newRepo);
    repoList->addItem(name);
    repoList->setCurrentRow(repos.size() - 1);
    updateWindowTitle();
}

void RepoListWindow::deleteRepo() {
    int row = repoList->currentRow();
    if (row < 0 || row >= repos.size()) return;

    repos.removeAt(row);
    delete repoList->takeItem(row);
    updateWindowTitle();
}

void RepoListWindow::updateWindowTitle() {
    if (hasChanges()) {
        setWindowTitle(baseWindowTitle + " *");
    } else {
        setWindowTitle(baseWindowTitle);
    }
}
