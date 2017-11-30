#ifndef CBROWSERWIDGET_H
#define CBROWSERWIDGET_H

#include <QWidget>

class QTableWidget;

class CBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CBrowserWidget(QWidget *parent = 0);

signals:

public slots:
private:
    QTableWidget *m_pTblWidgetFile;
};

#endif // CBROWSERWIDGET_H
