#include "TipsTab.h"

TipsTab::TipsTab(QWidget *parent)
    : QWidget(parent)
{
    // 创建布局和控件
    layout = new QVBoxLayout(this);
    tipsLabel = new QLabel(tr("Useful tips for better experience."), this);

    // 添加控件到布局
    layout->addWidget(tipsLabel);

    // 设置布局
    setLayout(layout);
}
