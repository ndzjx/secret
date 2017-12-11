#ifndef CTREEANALYSIS_H
#define CTREEANALYSIS_H

#include <QString>
#include <QVector>

class CTreeAnalysis
{
public:
    explicit CTreeAnalysis();
    void init(const QString &value);

    enum eFileType
    {
        eUnknown,
        eFile,
        eDir
    };

    void setFileName(const QString &filename);
    const QString &fileName();

    void setFileMD5(const QString &fileMD5);
    const QString &fileMD5();

    void setFileType(CTreeAnalysis::eFileType type);
    CTreeAnalysis::eFileType fileType();

    QVector<CTreeAnalysis*> &fileList();

    void setParent(CTreeAnalysis *parent);
    CTreeAnalysis *parent();

    void setFilePath(const QString &filePath);
    const QString &filePath();

private:

    QString m_sFilePath;
    QString m_sFileName;
    QString m_sFileMD5;
    eFileType m_eType;
    QVector<CTreeAnalysis*> m_oFileList;
    CTreeAnalysis *m_pParent;
};

#endif // CTREEANALYSIS_H
