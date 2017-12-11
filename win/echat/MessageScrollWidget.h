#ifndef MESSAGESCROLLWIDGET_H
#define MESSAGESCROLLWIDGET_H

#include "MessageWidget.h"
#include <QScrollArea>
#include <QWidget>
#include <QScrollBar>

class MessageScrollWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageScrollWidget(QWidget *parent = 0);

private:
    void initUI();


signals:
	void sigAddImageMsg(QPixmap icon, QString nickName, QPixmap msg);
    void sigAddTextMsg(QPixmap icon, QString nickName, QString msg);
    void sigAddVoiceMsg(QPixmap icon, QString nickName, QString msg);

public slots:
	void slotAddImageMsg(QPixmap icon, QString nickName, QPixmap msg);
    void slotAddTextMsg(QPixmap icon, QString nickName, QString msg);
    void slotAddVoiceMsg(QPixmap icon, QString nickName, QString msg);

	void addImageMsg(QPixmap icon, QString nickName, QPixmap msg);
    void addTextMsg(QPixmap icon, QString nickName, QString msg);
    void addVoiceMsg(QPixmap icon, QString nickName, QString msg);
    void onAutoScroll(int min, int max);

public:

private:
    MessageWidget *m_pmwMessage;
    QScrollArea *m_psaMessage;
};

#endif // MESSAGESCROLLWIDGET_H
