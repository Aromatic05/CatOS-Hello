#ifndef TIPSTAB_H
#define TIPSTAB_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class TipsTab : public QWidget
{
    Q_OBJECT

public:
    explicit TipsTab(QWidget *parent = nullptr);

private:
    QLabel *tipsLabel;
    QVBoxLayout *layout;
};

#endif // TIPSTAB_H
