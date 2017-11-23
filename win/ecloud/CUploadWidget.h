
#ifndef CUPLOADWIDGET_H
#define CUPLOADWIDGET_H

#include <QWidget>

class CUploadWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CUploadWidget(QWidget *parent = 0);

signals:

public slots:

protected:  
    void dragEnterEvent( QDragEnterEvent* e ) ;
    void dropEvent( QDropEvent* e ) ;
};

#endif // CUPLOADWIDGET_H
