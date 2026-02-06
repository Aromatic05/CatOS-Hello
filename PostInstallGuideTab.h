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
    QPushButton *mirrorButton;
    QPushButton *chooseButton;
    QPushButton *updateAURButton;
    QPushButton *packageCleanButton;
    QPushButton *updateButton;
    QPushButton *driverConfigButton;

private slots:
    void onPacmanButtonClicked();
    void onDriverConfigButtonClicked();
};

#endif // POSTINSTALLGUIDETAB_H
