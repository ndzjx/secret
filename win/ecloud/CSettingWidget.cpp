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

CSettingWidget::CSettingWidget(QWidget *parent) :
    QWidget(parent),
    m_pTblWidgetEmail(NULL)
{
    auto pLblEmail = new QLabel(QString::fromLocal8Bit("邮件列表:"), this);
    m_pTblWidgetEmail = new QTableWidget(0, 4, this);
    m_pTblWidgetEmail->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_pTblWidgetEmail->setEditTriggers(QTableWidget::NoEditTriggers);
    m_pTblWidgetEmail->setSelectionBehavior(QTableWidget::SelectRows);
    m_pTblWidgetEmail->setSelectionMode(QTableWidget::SingleSelection);
    m_pTblWidgetEmail->setHorizontalHeaderLabels( QStringList()
        << QStringLiteral( "账号" )
        << QStringLiteral( "密码" )
        << QStringLiteral( "smtp服务器" )
        << QStringLiteral( "pop3服务器") );
    m_pTblWidgetEmail->setCurrentCell(-1, -1);

    auto pVblLeft = new QVBoxLayout(this);
    pVblLeft->addWidget(pLblEmail);
    pVblLeft->addWidget(m_pTblWidgetEmail);
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

    connect(pBtnAdd, &QPushButton::clicked, [this](){
        CAddEmailDialog *pInputDialog = new CAddEmailDialog(NULL);
        if (pInputDialog->exec())
        {
            addEmailtoWidget(pInputDialog->acnt(), pInputDialog->pwd(), pInputDialog->smtp(), pInputDialog->pop3());

            service_meta model;
            model.user = pInputDialog->acnt().toLocal8Bit().data();
            model.pawd = pInputDialog->pwd().toLocal8Bit().data();
            model.smtp = pInputDialog->smtp().toLocal8Bit().data();
            model.pop3 = pInputDialog->pop3().toLocal8Bit().data();

            dbmeta_cloudnode cloudnode;
            cloudnode.from_meta(model);
            global_cloudnode_add(cloudnode);
        }
    });
    connect(pBtnDel, &QPushButton::clicked, [this](){
        if (m_pTblWidgetEmail->currentIndex().isValid())
        {
            auto row = m_pTblWidgetEmail->currentRow();
            service_meta model;

            model.user = m_pTblWidgetEmail->item(row, 0)->text().toLocal8Bit().data();
            model.pawd = m_pTblWidgetEmail->item(row, 1)->text().toLocal8Bit().data();
            model.smtp = m_pTblWidgetEmail->item(row, 2)->text().toLocal8Bit().data();
            model.pop3 = m_pTblWidgetEmail->item(row, 3)->text().toLocal8Bit().data();

            dbmeta_cloudnode cloudnode;
            cloudnode.from_meta(model);
            global_cloudnode_del(cloudnode);

            m_pTblWidgetEmail->removeRow(m_pTblWidgetEmail->currentRow());
        }
    });

    // 添加数据
    vector<dbmeta_cloudnode> vecEmails;
    vecEmails = global_cloudnodes_all();
    for (int i = 0; i < vecEmails.size(); ++i)
    {
        service_meta meta = vecEmails[i].to_meta();
        addEmailtoWidget(QString::fromLocal8Bit(meta.user.c_str()),
                         QString::fromLocal8Bit(meta.pawd.c_str()),
                         QString::fromLocal8Bit(meta.smtp.c_str()),
                         QString::fromLocal8Bit(meta.pop3.c_str()) );
    }
    setLayout(pHblCenter);
    resize(800, 600);
}

void CSettingWidget::addEmailtoWidget(const QString &acnt, const QString &pwd, const QString &smtp, const QString &pop3)
{
    auto row = m_pTblWidgetEmail->rowCount() ;
    m_pTblWidgetEmail->setRowCount( row + 1 ) ;
    m_pTblWidgetEmail->setItem(row, 0, new QTableWidgetItem(acnt));
    m_pTblWidgetEmail->setItem(row, 1, new QTableWidgetItem(pwd));
    m_pTblWidgetEmail->setItem(row, 2, new QTableWidgetItem(smtp));
    m_pTblWidgetEmail->setItem(row, 3, new QTableWidgetItem(pop3));
}
