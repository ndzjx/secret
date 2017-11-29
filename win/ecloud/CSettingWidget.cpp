#include "CSettingWidget.h"
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QHeaderView>
#include "CAddEmailDialog.h"
#include "ecloud.h"

CSettingWidget::CSettingWidget(QWidget *parent) : QWidget(parent)
{
    auto pLblEmail = new QLabel(QString::fromLocal8Bit("邮件列表:"), this);
    auto pTblWidgetEmail = new QTableWidget(0, 4, this);
    pTblWidgetEmail->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    pTblWidgetEmail->setEditTriggers(QTableWidget::NoEditTriggers);
    pTblWidgetEmail->setSelectionBehavior(QTableWidget::SelectRows);
    pTblWidgetEmail->setSelectionMode(QTableWidget::SingleSelection);
    pTblWidgetEmail->setHorizontalHeaderLabels( QStringList()
        << QStringLiteral( "账号" )
        << QStringLiteral( "密码" )
        << QStringLiteral( "smtp服务器" )
        << QStringLiteral( "pop3服务器") );

    auto pVblLeft = new QVBoxLayout(this);
    pVblLeft->addWidget(pLblEmail);
    pVblLeft->addWidget(pTblWidgetEmail);
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

    connect(pBtnAdd, &QPushButton::clicked, [this, pTblWidgetEmail](){
        CAddEmailDialog *pInputDialog = new CAddEmailDialog(NULL);
        if (pInputDialog->exec())
        {
            auto row = pTblWidgetEmail->rowCount() ;
            pTblWidgetEmail->setRowCount( row + 1 ) ;
            pTblWidgetEmail->setItem(row, 0, new QTableWidgetItem(pInputDialog->acnt()));
            pTblWidgetEmail->setItem(row, 1, new QTableWidgetItem(pInputDialog->pwd()));
            pTblWidgetEmail->setItem(row, 2, new QTableWidgetItem(pInputDialog->smtp()));
            pTblWidgetEmail->setItem(row, 3, new QTableWidgetItem(pInputDialog->pop3()));

            service_meta model;
            model.user = pInputDialog->acnt().toLocal8Bit().data();
            model.pawd = pInputDialog->pwd().toLocal8Bit().data();
            model.smtp = pInputDialog->smtp().toLocal8Bit().data();
            model.pop3 = pInputDialog->pop3().toLocal8Bit().data();
            global_insert_cloudnode(model);
        }
    });
    connect(pBtnDel, &QPushButton::clicked, [pTblWidgetEmail](){
        if (pTblWidgetEmail->currentIndex().isValid())
        {
            pTblWidgetEmail->removeRow(pTblWidgetEmail->currentRow());
        }
    });

    setLayout(pHblCenter);
    resize(800, 600);
}

