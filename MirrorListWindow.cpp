#include "MirrorListWindow.h"
#include <QNetworkRequest>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QTextStream>

MirrorListWindow::MirrorListWindow(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Mirror List"));
    setModal(true);
    resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);

    scrollArea = new QScrollArea(this);
    scrollContent = new QWidget(scrollArea);
    scrollLayout = new QGridLayout(scrollContent);
    scrollArea->setWidget(scrollContent);
    scrollArea->setWidgetResizable(true);
    layout->addWidget(scrollArea);

    loadMirrorList();
}

void MirrorListWindow::loadMirrorList()
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MirrorListWindow::onMirrorListLoaded);

    QUrl url("https://archlinux.org/mirrorlist/all/");
    QNetworkRequest request(url);
    manager->get(request);
}

void MirrorListWindow::onMirrorListLoaded(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray data = reply->readAll();
        QString mirrorList = QString::fromUtf8(data);

        QStringList lines = mirrorList.split("\n");

        // 跳过前 4 行（文件头）
        for (int i = 4; i < lines.size(); ++i)
        {
            QString line = lines[i].trimmed();
            if (line.startsWith("## ")) // 提取国家
            {
                QString country = line.mid(3).trimmed(); // 去掉 "## " 前缀
                QStringList mirrors;

                // 提取当前国家的镜像 URL
                for (int j = i + 1; j < lines.size(); ++j)
                {
                    QString mirrorLine = lines[j].trimmed();
                    if (mirrorLine.startsWith("#Server = "))
                    {
                        QString mirrorUrl = mirrorLine.mid(9).trimmed(); // 去掉 "#Server = " 前缀
                        mirrors.append(mirrorUrl);
                    }
                    else if (mirrorLine.startsWith("## "))
                    {
                        // 遇到下一个国家，停止提取
                        break;
                    }
                }

                // 将国家和镜像列表存入 map
                if (!country.isEmpty() && !mirrors.isEmpty())
                {
                    countryMirrorsMap[country] = mirrors;
                }
            }
        }

        int row = 0;
        int col = 0;
        const int columnsPerRow = 3;

        for (const QString &country : countryMirrorsMap.keys())
        {
            QPushButton *button = new QPushButton(country, scrollContent);
            connect(button, &QPushButton::clicked, this, [this, country]() {
                onCountryButtonClicked(country);
            });
            scrollLayout->addWidget(button, row, col);
            col++;
            if (col >= columnsPerRow)
            {
                col = 0;
                row++;
            }
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load mirror list: ") + reply->errorString());
    }

    reply->deleteLater();
}

void MirrorListWindow::showMirrorsForCountry(const QString &country)
{
    QStringList mirrors = countryMirrorsMap[country];

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Mirrors in " + country);
    dialog->setModal(true);
    dialog->resize(800, 600);

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QPlainTextEdit *mirrorText = new QPlainTextEdit(dialog);
    mirrorText->setReadOnly(true);
    layout->addWidget(mirrorText);

    QPushButton *saveButton = new QPushButton(tr("Save to \"/etc/pacman.d/mirrorlist\""), dialog);
    layout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &MirrorListWindow::onSaveButtonClicked);

    runRateMirrors(mirrors, mirrorText);

    dialog->setLayout(layout);

    // 确保对话框关闭后被销毁
    connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);

    dialog->exec();
}

void MirrorListWindow::runRateMirrors(const QStringList &mirrors, QPlainTextEdit *textEdit)
{
    QString tempFilePath = "/tmp/mirrors.txt";
    QFile tempFile(tempFilePath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to create temporary file."));
        return;
    }

    QTextStream out(&tempFile);
    for (const QString &mirror : mirrors)
    {
        QString prefix = mirror.split("$repo").first();
        out << prefix << "\n";
    }
    tempFile.close();

    QProcess *process = new QProcess(this);

    connect(process, &QProcess::readyReadStandardOutput, this, [process, textEdit]() {
        QByteArray output = process->readAllStandardOutput();
        textEdit->appendPlainText(QString::fromUtf8(output));
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [process, textEdit](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::CrashExit || exitCode != 0)
        {
            QMessageBox::critical(nullptr, tr("Error"), tr("rate-mirrors command failed."));
        }
        process->deleteLater();
    });

    QStringList arguments;
    arguments << "stdin"
              << "--path-to-test=core/os/x86_64/core.db"
              << "--path-to-return=$repo/os/$arch"
              << "--comment-prefix=# "
              << "--output-prefix=Server = ";

    // 启动 rate-mirrors
    process->setStandardInputFile(tempFilePath);
    process->start("rate-mirrors", arguments);

    if (!process->waitForStarted())
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to start rate-mirrors command."));
        return;
    }
}

void MirrorListWindow::onCountryButtonClicked(const QString &country)
{
    showMirrorsForCountry(country);
}

void MirrorListWindow::onSaveButtonClicked()
{
    QDialog *dialog = qobject_cast<QDialog *>(sender()->parent());
    QPlainTextEdit *mirrorText = dialog->findChild<QPlainTextEdit *>();

    if (!mirrorText)
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to find QPlainTextEdit."));
        return;
    }

    QString content = mirrorText->toPlainText();

    QStringList lines = content.split("\n");
    QStringList validLines;
    for (const QString &line : lines)
    {
        if (!line.trimmed().startsWith("#") && !line.trimmed().isEmpty())
        {
            validLines.append(line);
        }
    }

    if (validLines.size() > 5)
    {
        validLines = validLines.mid(0, 5);
    }

    QString tempFilePath = "/tmp/mirrorlist.tmp";
    QFile tempFile(tempFilePath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to create temporary file."));
        return;
    }

    QTextStream out(&tempFile);
    for (const QString &line : validLines)
    {
        out << line << "\n";
    }
    tempFile.close();

    QProcess process;
    QStringList arguments;
    arguments << "cp" << tempFilePath << "/etc/pacman.d/mirrorlist";

    process.start("pkexec", arguments);

    if (!process.waitForFinished())
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to run pkexec command."));
        return;
    }

    if (process.exitCode() != 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Failed to update mirror list: ") + process.readAllStandardError());
        return;
    }

    QMessageBox::information(this, tr("Success"), tr("Mirror list updated successfully."));
}
