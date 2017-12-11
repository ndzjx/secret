#ifndef MYEMOTIONWINDOW_H
#define MYEMOTIONWINDOW_H

#include <QWidget>
#include "ui_myemotionwindow.h"
#include <QLabel>

class MyEmotionWidget;
class MyEmotionWindow : public QWidget
{
	Q_OBJECT

public:
	MyEmotionWindow(QWidget *parent = 0);
	~MyEmotionWindow();

	void showSmallEmotion(QPoint point);
	void showNormalEmotion(QPoint point);

private:
	void initWindow();
	void initEmotion();

private:
	Ui::MyEmotionWindowClass ui;

	MyEmotionWidget* m_smallEmotionWidget;
	MyEmotionWidget* m_normalEmotionWidget;
	bool m_isNeedHide;
	QLabel* m_lableTitle;
};

#endif // MYEMOTIONWINDOW_H
