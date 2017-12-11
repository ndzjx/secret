
#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>

class eChatWidget ;

class MainForm : public QWidget
{
    Q_OBJECT
public:
    explicit MainForm(QWidget *parent = 0);

signals:

public slots:

private:
	void mousePressEvent( QMouseEvent* event ) override ;

public:
	eChatWidget* m_pChatWidget = nullptr ;
};

#endif // MAINFORM_H
