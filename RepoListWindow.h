#ifndef REPOLISTWINDOW_H
#define REPOLISTWINDOW_H

#include <QDialog>
#include <QList>
#include <QStringList>

class QListWidget;
class QLineEdit;
class QPlainTextEdit;
class QComboBox;
class QPushButton;

struct Repo {
    QString name;           // 当前仓库名称
    QString include;        // 当前 Include 路径
    QStringList servers;    // 当前镜像服务器列表
    QString sigLevel;       // 当前签名验证级别

    QString origName;       // 原始仓库名称
    QString origInclude;   // 原始 Include 路径
    QStringList origServers; // 原始镜像服务器列表
    QString origSigLevel;  // 原始签名验证级别
};

class RepoListWindow : public QDialog {
    Q_OBJECT

public:
    explicit RepoListWindow(QWidget *parent = nullptr);

private:
    void setupUI();                      // 初始化 UI
    void loadConfig();                   // 加载配置文件
    bool hasChanges() const;             // 检查是否有未保存的修改
    void saveConfig();                   // 保存配置文件
    bool backupConfigFile();
    bool writeConfigFile();
    void updateWindowTitle();            // 更新窗口标题

    // UI 控件
    QListWidget *repoList{};
    QLineEdit *repoNameEdit{};
    QLineEdit *includeEdit{};
    QPlainTextEdit *serversEdit{};
    QComboBox *sigLevelCombo{};

    // 数据
    QList<Repo> repos;                   // 当前仓库列表
    QList<Repo> origRepos;               // 原始仓库列表
    QStringList nonRepoLines;            // 非仓库段配置内容
    QString baseWindowTitle;             // 基础窗口标题

private slots:
    void onRepoSelected(int row);        // 仓库选择事件
    void onRepoNameEdited(const QString &text); // 仓库名称编辑事件
    void onIncludeEdited(const QString &text);  // Include 路径编辑事件
    void onServersEdited();              // 镜像服务器编辑事件
    void onSigLevelChanged(int index);   // 签名验证级别更改事件
    void addRepo();                      // 添加仓库
    void deleteRepo();                   // 删除仓库
};

#endif // REPOLISTWINDOW_H
