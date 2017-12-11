#ifndef ECHATWIDGET_H
#define ECHATWIDGET_H

#include <QWidget>
#include "MessageScrollWidget.h"
#include "Widget.h"

class eChatWidget: public QWidget
{
public:
    eChatWidget(QWidget *parent);

public:
    MessageScrollWidget *m_pscrollWidget;
    Widget *m_pwidget;

};

#endif // ECHATWIDGET_H
