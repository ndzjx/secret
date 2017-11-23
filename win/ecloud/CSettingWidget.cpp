#include "CSettingWidget.h"
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>

CSettingWidget::CSettingWidget(QWidget *parent) : QWidget(parent)
{
    auto pLblEmail = new QLabel(QString::fromLocal8Bit("邮件列表:"), this);
    auto pLstEmail = new QListWidget(this);
    auto pVblLeft = new QVBoxLayout(this);
    pVblLeft->addWidget(pLblEmail);
    pVblLeft->addWidget(pLstEmail);
    auto pLeftWidget = new QWidget(this);
    pLeftWidget->setLayout(pVblLeft);

    auto pBtnAdd = new QPushButton(QString::fromLocal8Bit("添加"));
    auto pBtnDel = new QPushButton(QString::fromLocal8Bit("删除"));
    auto pVblRight = new QVBoxLayout(this);
    pVblRight->addStretch(2);
    pVblRight->addWidget(pBtnAdd);
    pVblRight->addStretch(1);
    pVblRight->addWidget(pBtnDel);
    pVblRight->addStretch(2);
    auto pRightWidget = new QWidget(this);
    pRightWidget->setLayout(pVblRight);

    auto pHblCenter = new QHBoxLayout(this);
    pHblCenter->addWidget(pLeftWidget);
    pHblCenter->addWidget(pRightWidget);

    connect(pBtnAdd, &QPushButton::clicked, [this, pLstEmail](){
        QString sEmail = QInputDialog::getText(this, "Input", "Email:");
        if (!sEmail.isEmpty())
        {
            pLstEmail->addItem(sEmail);
        }
    });
    connect(pBtnDel, &QPushButton::clicked, [pLstEmail](){
        if (pLstEmail->currentIndex().isValid())
        {
            auto pItem = pLstEmail->currentItem();
            pLstEmail->removeItemWidget(pItem);
            delete pItem;
        }
    });

    setLayout(pHblCenter);
    resize(800, 600);
}

