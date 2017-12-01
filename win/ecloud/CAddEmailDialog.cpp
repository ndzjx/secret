#include "CAddEmailDialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>

CAddEmailDialog::CAddEmailDialog(QDialog *parent) :
    QDialog(parent),
    m_pLedtAcnt(NULL),
    m_pLedtPwd(NULL),
    m_pLedtSmtp(NULL),
    m_pLedtPop3(NULL)
{
    m_pLedtAcnt = new QLineEdit(this);
    m_pLedtPwd = new QLineEdit(this);
    m_pLedtSmtp = new QLineEdit(this);
    m_pLedtPop3 = new QLineEdit(this);

    QGridLayout *pGrilInfo = new QGridLayout(this);
    pGrilInfo->setColumnStretch(1, 1);
    pGrilInfo->addWidget(new QLabel(QStringLiteral("用户名:")), 0, 0);
    pGrilInfo->addWidget(m_pLedtAcnt, 0, 1);
    pGrilInfo->addWidget(new QLabel(QStringLiteral("密码:")), 1, 0);
    pGrilInfo->addWidget(m_pLedtPwd, 1, 1);
    pGrilInfo->addWidget(new QLabel(QStringLiteral("smtp服务器:")), 2, 0);
    pGrilInfo->addWidget(m_pLedtSmtp, 2, 1);
    pGrilInfo->addWidget(new QLabel(QStringLiteral("pop3服务器:")), 3, 0);
    pGrilInfo->addWidget(m_pLedtPop3, 3, 1);
    QWidget *pWidgetTop = new QWidget(this);
    pWidgetTop->setLayout(pGrilInfo);

    QPushButton *pBtnOk = new QPushButton(QStringLiteral("确定"), this);
    QPushButton *pBtnCancel = new QPushButton(QStringLiteral("取消"), this);
    QHBoxLayout *pHblBottom = new QHBoxLayout(this);
    pHblBottom->addStretch();
    pHblBottom->addWidget(pBtnOk);
    pHblBottom->addWidget(pBtnCancel);
    QWidget *pWidgetBottom = new QWidget(this);
    pWidgetBottom->setLayout(pHblBottom);

    QVBoxLayout *pVblCenter = new QVBoxLayout(this);
    pVblCenter->addWidget(pWidgetTop);
    pVblCenter->addWidget(pWidgetBottom);

    setLayout(pVblCenter);
    setFocusPolicy(Qt::NoFocus);

    connect(pBtnOk, &QPushButton::clicked, [this]{
        if (m_pLedtAcnt->text().isEmpty() ||
            m_pLedtPwd->text().isEmpty() ||
            m_pLedtSmtp->text().isEmpty() ||
            m_pLedtPop3->text().isEmpty())
        {
            QMessageBox::warning(this, QStringLiteral("错误"), QStringLiteral("内容不能为空！"),
                                 QStringLiteral("确定"));
            return;
        }
        accept();
    });
    connect(pBtnCancel, &QPushButton::clicked, [this]{
        reject();
    });

}
