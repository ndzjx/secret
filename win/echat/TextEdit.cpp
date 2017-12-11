#include <QVBoxLayout>
#include<QToolBar>
#include <QPushButton>
#include<QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include "TextEdit.h"
#include "Widget.h"

TextEdit::TextEdit(QWidget *parent):
    QTextEdit(parent)
{
   // this->setFixedSize(720, 450);
	installEventFilter(this);
	this->setFixedHeight( 200 ) ;
}

void TextEdit::slotUpdate()
{
    QTextCursor cursor = this->textCursor();
    if(!cursor.atEnd())
    {
    cursor.movePosition(QTextCursor::End,QTextCursor::MoveAnchor,1);
    this->setTextCursor(cursor);
    }
    cursor.insertText("strfdsafasdfsf");
}
bool TextEdit::canInsertFromMimeData(const QMimeData* source) const
{
    return source->hasImage() || source->hasUrls() ||
        QTextEdit::canInsertFromMimeData(source);
}

void TextEdit::insertFromMimeData(const QMimeData* source)
{
    if (source->hasImage())
    {
        static int i = 1;
        QUrl url(QString("dropped_image_%1").arg(i++));
        dropImage(url, qvariant_cast<QImage>(source->imageData()));
    }
    else if (source->hasUrls())
    {
        foreach (QUrl url, source->urls())
        {
            QFileInfo info(url.toLocalFile());
            if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
            {
                dropImage(url, QImage(info.filePath()));
                bool bisSend = false;
                while(!bisSend)
                {
                    Widget *pwidget = dynamic_cast<Widget *>(parent());
                    if(pwidget != nullptr)
                    {   clear();
                        bisSend = pwidget->sendImage(info.filePath());
                    }
                }
            }
            else
                dropTextFile(url);
        }
    }
    else
    {
        QTextEdit::insertFromMimeData(source);
    }
}

void TextEdit::insertImage(const QString &path, const QMimeData* source)
{
    if (!path.isEmpty())
    {
        static int i = 1;
        QUrl url;
        url.setPath("/" + path);
        url.setScheme("file");
        QString str = url.toString();
        QFileInfo info(url.toLocalFile());
        //if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
        QTextImageFormat imageFormat;   //保存图片格式对象
            imageFormat.setName(path);
            //QTextCursor cursor;
            //编辑指针标
            //emit sigUpdate();
            textCursor().insertImage(imageFormat);   //通过编辑指针表把图片格式的文件插入到资源中
        //dropImage(url, qvariant_cast<QImage>(source->imageData()));
    }
}

void TextEdit::dropImage(const QUrl& url, const QImage& image)
{
    if (!image.isNull())
    {
        document()->addResource(QTextDocument::ImageResource, url, image);
        textCursor().insertImage(url.toString());
    }
}

void TextEdit::dropTextFile(const QUrl& url)
{
    QFile file(url.toLocalFile());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        textCursor().insertText(file.readAll());
}

bool TextEdit::eventFilter(QObject * obj, QEvent * e)
{
    if (e->type() == QEvent::KeyPress)  
    {  
        QKeyEvent *event = dynamic_cast<QKeyEvent*>(e);  
        if (event && event->key() == Qt::Key_Return && (event->modifiers() & Qt::ControlModifier))  
        {  
			emit sigEnter() ;
            return true;  
        }  
    }  
    return false; 
}
