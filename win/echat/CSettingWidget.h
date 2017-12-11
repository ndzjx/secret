#ifndef CSETTINGWIDGET_H
#define CSETTINGWIDGET_H

#include <QWidget>

class QTableWidget;

class CSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSettingWidget(QWidget *parent = 0);

signals:

public slots:

private:
    void addEmailtoWidget(const QString &acnt, const QString &pwd, const QString &smtp, const QString &pop3);

    QTableWidget *m_pTblWidgetEmail;
};

#endif // CSETTINGWIDGET_H
