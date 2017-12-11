/***************************************************************************************************

  ×÷Õß: lilc-a 2017
  ±¸×¢:
  ÉóºË:

  Copyright (c) 1994-2017 Glodon Corporation

****************************************************************************************************/
#ifndef GHYTITLEBAR_H
#define GHYTITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QBoxLayout>

class GHYTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit GHYTitleBar(QWidget *parent);
    ~GHYTitleBar();

    void init(int style);
    QLayout *leftTool() { return m_pLeftToolLayout; }
    QLayout *rightTool() { return m_pRightToolLayout; }

private slots:
    void onClicked();

private:
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    void updateMaximize();

    QLayout *m_pLeftToolLayout;
    QLayout *m_pRightToolLayout;
    QLabel *m_pIconLabel;
    QPushButton *m_pMinimizeButton;
    QPushButton *m_pMaximizeButton;
    QPushButton *m_pCloseButton;
};

#endif // GHYTITLEBAR_H
