#include "InstallAppsTab.h"

InstallAppsTab::InstallAppsTab(QWidget *parent)
    : QWidget(parent)
{
    // 创建布局和控件
    layout = new QVBoxLayout(this);
    installAppsLabel = new QLabel(tr("Expand your software collection."), this);

    // 添加控件到布局
    layout->addWidget(installAppsLabel);

    // 设置布局
    setLayout(layout);
}
