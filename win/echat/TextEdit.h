#ifndef TEXTEDIT_H
#define TEXTEDIT_H
#include <QTextEdit>
#include <QMimeData>
#include <QFileInfo>
#include <QImageReader>
#include <QPushButton>

class TextEdit : public QTextEdit
{
	Q_OBJECT

public:
    TextEdit(QWidget *parent);

signals:
	void sigEnter() ;
    void sigUpdate() ;

public slots:
    void slotUpdate();

public:
    bool canInsertFromMimeData(const QMimeData* source) const;
    void insertFromMimeData(const QMimeData* source);
    void insertImage(const QString &path, const QMimeData* source);
private:
    void dropImage(const QUrl& url, const QImage& image);
    void dropTextFile(const QUrl& url);
	bool eventFilter(QObject *obj, QEvent *e); 
};


#endif // TEXTEDIT_H
