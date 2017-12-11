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
    m_pLedtPwd->setEchoMode(QLineEdit::Password);
    m_pLedtSmtp = new QLineEdit(this);
    m_pLedtPop3 = new QLineEdit(this);

    QGridLayout *pGrilInfo = new QGridLayout(this);
    pGrilInfo->setColumnStretch(1, 1);
    pGrilInfo->addWidget(new QLabel(QStringLiteral("�˺�:")), 0, 0);
    pGrilInfo->addWidget(m_pLedtAcnt, 0, 1);
    pGrilInfo->addWidget(new QLabel(QStringLiteral("����:")), 1, 0);
    pGrilInfo->addWidget(m_pLedtPwd, 1, 1);
    pGrilInfo->addWidget(new QLabel(QStringLiteral("smtp������:")), 2, 0);
    pGrilInfo->addWidget(m_pLedtSmtp, 2, 1);
    pGrilInfo->addWidget(new QLabel(QStringLiteral("pop3������:")), 3, 0);
    pGrilInfo->addWidget(m_pLedtPop3, 3, 1);
    QWidget *pWidgetTop = new QWidget(this);
    pWidgetTop->setLayout(pGrilInfo);

    QLabel *pLblTip = new QLabel(QString("<span style=\" color:red;\">%1</span>").
                                 arg(QStringLiteral("����������ͬʱΪ��")), this);
    pLblTip->setFont(QFont());
    QHBoxLayout *pHblMidTip = new QHBoxLayout(this);
    pHblMidTip->addStretch();
    pHblMidTip->addWidget(pLblTip);
    QWidget *pWidgetMidTip = new QWidget(this);
    pWidgetMidTip->setLayout(pHblMidTip);

    QPushButton *pBtnOk = new QPushButton(QStringLiteral("ȷ��"), this);
    QPushButton *pBtnCancel = new QPushButton(QStringLiteral("ȡ��"), this);
    QHBoxLayout *pHblBottom = new QHBoxLayout(this);
    pHblBottom->addStretch();
    pHblBottom->addWidget(pBtnOk);
    pHblBottom->addWidget(pBtnCancel);
    QWidget *pWidgetBottom = new QWidget(this);
    pWidgetBottom->setLayout(pHblBottom);

    QVBoxLayout *pVblCenter = new QVBoxLayout(this);
    pVblCenter->addWidget(pWidgetTop);
    pVblCenter->addWidget(pWidgetMidTip);
    pVblCenter->addWidget(pWidgetBottom);

    setLayout(pVblCenter);
    setFocusPolicy(Qt::NoFocus);

    connect(pBtnOk, &QPushButton::clicked, [this]{
        if (m_pLedtAcnt->text().trimmed().isEmpty())
        {
            QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("�û�������Ϊ�գ�"),
                                 QStringLiteral("ȷ��"));
            m_pLedtAcnt->setFocus();
            return;
        }
        else if (m_pLedtPwd->text().trimmed().isEmpty())
        {
            QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("���벻��Ϊ�գ�"),
                                 QStringLiteral("ȷ��"));
            m_pLedtPwd->setFocus();
            return;
        }
        else if (m_pLedtSmtp->text().trimmed().isEmpty() && m_pLedtPop3->text().trimmed().isEmpty())
        {
            QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("����������ͬʱΪ�գ�"),
                                 QStringLiteral("ȷ��"));
            m_pLedtSmtp->setFocus();
            return;
        }
        accept();
    });
    connect(pBtnCancel, &QPushButton::clicked, [this]{
        reject();
    });

	setWindowTitle( QStringLiteral( "��ӽڵ�" ) ) ;
}
