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
    QString currentSection;

    for (int i = 0; i < originalFileLines.size(); ++i) {
        QString line = originalFileLines[i];
        QString trimmed = line.trimmed();

        if (trimmed.startsWith('[')) {
            if (inRepoSection) {
                // 保存上一个仓库的原始数据
                origRepos.append(currentRepo);
                currentRepo = Repo();
            }

            currentSection = trimmed.mid(1, trimmed.indexOf(']') - 1);
            if (currentSection != "options") {
                currentRepo.origName = currentSection;
                currentRepo.name = currentSection;
                inRepoSection = true;
                continue; // 关键修改：跳过仓库起始行，不加入 nonRepoLines
            } else {
                nonRepoLines.append(line); // [options] 段保留
            }
        } else if (inRepoSection) {
            // 解析仓库配置字段
            if (trimmed.startsWith("Include")) {
                currentRepo.origInclude = trimmed.section('=', 1).trimmed();
                currentRepo.include = currentRepo.origInclude;
            } else if (trimmed.startsWith("Server")) {
                QString value = trimmed.section('=', 1).trimmed();
                currentRepo.origServers.append(value);
                currentRepo.servers = currentRepo.origServers;  // 同步数据
            } else if (trimmed.startsWith("SigLevel")) {
                currentRepo.origSigLevel = trimmed.section('=', 1).trimmed();
                currentRepo.sigLevel = currentRepo.origSigLevel;
            }
        } else {
            nonRepoLines.append(line); // 非仓库内容保留
        }
    }

    if (inRepoSection) origRepos.append(currentRepo);
    repos = origRepos;

    // 更新仓库列表显示
    repoList->clear();
    for (const Repo &repo : repos) {
        repoList->addItem(repo.name);
    }
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

void RepoListWindow::saveConfig() {
    if (!hasChanges()) {
        QMessageBox::information(this, tr("Note"), tr("No Change to Save"));
        return;
    }

    QTemporaryFile tempFile;
    if (!tempFile.open()) {
        QMessageBox::critical(this, tr("Error"), tr("Fail to Create Temp File"));
        return;
    }

    QTextStream out(&tempFile);
    out << nonRepoLines.join('\n') << '\n'; // 写入非仓库段

    // 完全重新生成所有仓库配置
    for (const Repo &repo : repos) {
        out << "[" << repo.name << "]\n";
        if (!repo.sigLevel.isEmpty())
            out << "SigLevel = " << repo.sigLevel << '\n';
        if (!repo.include.isEmpty())
            out << "Include = " << repo.include << '\n';

        // 强制遍历 servers 列表
        for (const QString &server : repo.servers) {
            out << "Server = " << server << '\n';
        }
        out << '\n';  // 段间分隔
    }

    tempFile.close();

    QProcess process;
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString backupFile = QString("/etc/pacman.conf.bak_%1").arg(timestamp);

    process.start("pkexec", {"sh", "-c",
                             QString("cp -f /etc/pacman.conf %1 && mv %2 /etc/pacman.conf")
                                 .arg(backupFile)
                                 .arg(tempFile.fileName())});

    if (process.waitForFinished() && process.exitCode() == 0) {
        origRepos = repos;
        QMessageBox::information(this, tr("Sucess"), tr("Configuration has been updated"));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Fail to save:") + process.errorString());
    }
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
}

void RepoListWindow::onIncludeEdited(const QString &text) {
    int row = repoList->currentRow();
    if (row >= 0) repos[row].include = text;
}

void RepoListWindow::onServersEdited() {
    int row = repoList->currentRow();
    if (row >= 0) repos[row].servers = serversEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
}

void RepoListWindow::onSigLevelChanged(int index) {
    int row = repoList->currentRow();
    if (row >= 0) repos[row].sigLevel = sigLevelCombo->itemText(index);
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
}

void RepoListWindow::deleteRepo() {
    int row = repoList->currentRow();
    if (row < 0 || row >= repos.size()) return;

    repos.removeAt(row);
    delete repoList->takeItem(row);
    QMessageBox::information(this, tr("Note"), tr("Repo has been changed, please save the change."));
}
