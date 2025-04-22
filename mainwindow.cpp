#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QSpacerItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("CatOS-Hello");

    // 创建中央控件
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout;

    // 创建欢迎标签
    QLabel *welcomeLabel = new QLabel(tr("Welcome to CatOS!"), this);
    mainLayout->addWidget(welcomeLabel);

    // 创建Tab控件
    QTabWidget *mainTabWidget = new QTabWidget(this);

    // 添加各个 Tab
    mainTabWidget->addTab(new GeneralNewsTab(this), tr("General News"));
    mainTabWidget->addTab(new PostInstallGuideTab(this), tr("Post-Install Guide"));
    mainTabWidget->addTab(new AssistantTab(this), tr("Assistant"));
    mainTabWidget->addTab(new TipsTab(this), tr("Tips"));
    mainTabWidget->addTab(new InstallAppsTab(this), tr("Install More Apps"));

    // 将Tab控件添加到布局中
    mainLayout->addWidget(mainTabWidget);

    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QSpacerItem *horizontalSpacer = new QSpacerItem(200, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
    buttonLayout->addItem(horizontalSpacer);

    QPushButton *softwareNewsButton = new QPushButton(tr("Software News"), this);
    QPushButton *viewLogsButton = new QPushButton(tr("View Update Logs"), this);
    QPushButton *noShowButton = new QPushButton(tr("Stop Showing"), this);
    QPushButton *exitButton = new QPushButton(tr("Exit"), this);

    buttonLayout->addWidget(softwareNewsButton);
    buttonLayout->addWidget(viewLogsButton);
    buttonLayout->addWidget(noShowButton);
    buttonLayout->addWidget(exitButton);

    // 将按钮布局添加到主布局
    mainLayout->addLayout(buttonLayout);

    // 设置中央控件和布局
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
