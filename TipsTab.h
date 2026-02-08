#ifndef TIPSTAB_H
#define TIPSTAB_H

#include <QWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QVBoxLayout>

class TipsTab : public QWidget
{
    Q_OBJECT

public:
    explicit TipsTab(QWidget *parent = nullptr);

private:
    QTextBrowser *tipsBrowser;
    QVBoxLayout *layout;
};

#endif // TIPSTAB_H
