#ifndef MYEMOTIONITEMWIDGET_H
#define MYEMOTIONITEMWIDGET_H

#include <QLabel>
#include <QMouseEvent>

class MyEmotionItemWidget : public QLabel
{
	Q_OBJECT

public:
	MyEmotionItemWidget(QString fileName, QSize emotionMoiveSize);
	~MyEmotionItemWidget();

private:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *ev);
signals:
    void hideWin();
    void sendpath(QString &path);
private:
	
};

#endif // MYEMOTIONITEMWIDGET_H
