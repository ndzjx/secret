#ifndef CADDEMAILDIALOG_H
#define CADDEMAILDIALOG_H

#include <QDialog>
#include <QLineEdit>

class CAddEmailDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CAddEmailDialog(QDialog *parent = 0);

    QString acnt() { return m_pLedtAcnt->text(); }
    QString pwd() { return m_pLedtPwd->text(); }
    QString smtp() { return m_pLedtSmtp->text(); }
    QString pop3() { return m_pLedtPop3->text(); }

private:
    QLineEdit *m_pLedtAcnt;
    QLineEdit *m_pLedtPwd;
    QLineEdit *m_pLedtSmtp;
    QLineEdit *m_pLedtPop3;
};

#endif // CADDEMAILDIALOG_H
