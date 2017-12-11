#include "Emotion.h"
#include <QVBoxLayout>
#include "Widget.h"

Emotion::Emotion(QWidget *parent):
    QDialog(parent)
{
    QVBoxLayout *layout  = new QVBoxLayout(this);
    m_pemotion = new MyEmotionWindow(this);
    layout->addWidget(m_pemotion);
    setLayout(layout);
}

void Emotion::setVisible()
{
    this->hide();
}

void Emotion::sendimage(QString &path)
{
    (dynamic_cast<Widget *>(parent()))->sendImage(path);
}
