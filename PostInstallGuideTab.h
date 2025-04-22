#ifndef POSTINSTALLGUIDETAB_H
#define POSTINSTALLGUIDETAB_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>

class PostInstallGuideTab : public QWidget
{
    Q_OBJECT

public:
    explicit PostInstallGuideTab(QWidget *parent = nullptr);

private:
    QLabel *postInstallGuideLabel;
    QVBoxLayout *layout;
    QGridLayout *gridLayout;
    QPushButton *pacmanButton;
    QPushButton *logButton;
    QPushButton *nvidiaButton;
    QPushButton *mirrorButton;
    QPushButton *chooseButton;
    QPushButton *updateAURButton;
    QPushButton *packageCleanButton;
    QPushButton *updateButton;

private slots:
    void onPacmanButtonClicked();
};

#endif // POSTINSTALLGUIDETAB_H
