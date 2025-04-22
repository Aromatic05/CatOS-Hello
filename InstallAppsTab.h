#ifndef INSTALLAPPSTAB_H
#define INSTALLAPPSTAB_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class InstallAppsTab : public QWidget
{
    Q_OBJECT

public:
    explicit InstallAppsTab(QWidget *parent = nullptr);

private:
    QLabel *installAppsLabel;
    QVBoxLayout *layout;
};

#endif // INSTALLAPPSTAB_H
