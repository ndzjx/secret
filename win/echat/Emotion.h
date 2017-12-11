#ifndef EMOTION_H
#define EMOTION_H
#include <qdialog.h>
#include "myemotionwindow.h"

class Emotion : public QDialog
{
public:
    Emotion(QWidget *parent);
private:
    MyEmotionWindow *m_pemotion = nullptr;

public slots:
    void setVisible();
    void sendimage(QString &path);
};

#endif // EMOTION_H
