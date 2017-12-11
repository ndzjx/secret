#include "CTreeAnalysis.h"
#include "ecloud.h"
#include <QString>
#include <QtAlgorithms>
#include <QStringList>

inline bool comp(CTreeAnalysis *left, CTreeAnalysis *right)
{
    CTreeAnalysis::eFileType oLType = left->fileType();
    CTreeAnalysis::eFileType oRType = right->fileType();
    if (oLType == oRType)
    {
        return left->fileName() <= right->fileName();
    }
    else if (oLType == CTreeAnalysis::eDir)
    {
        return true ;
    }
    else
    {
        return false;
    }
}

CTreeAnalysis::CTreeAnalysis():
    m_sFilePath(),
    m_sFileName(),
    m_sFileMD5(),
    m_eType(eUnknown),
    m_oFileList(),
    m_pParent(NULL)
{

}

void CTreeAnalysis::init(const QString &value)
{
    QString sTmp = value;
    QStringList sList = sTmp.split("\r\n");

    QVector<CTreeAnalysis*> vecLevel;
    vecLevel.resize(100);

    for ( int i = 0; i < sList.size(); ++i)
    {
        QString sValue = sList[i].trimmed();
        if (sValue.isEmpty())
        {
            continue;
        }
        CTreeAnalysis *pTreeNode = new CTreeAnalysis;
        if(sValue.startsWith(".dir"))
        {
            // д©б╪
            pTreeNode->setFileType(eDir);
        }
        else
        {
            // нд╪Ч
            pTreeNode->setFileType(eFile);
            QString sMD5 = sValue.mid(0, sList[i].indexOf(","));
            pTreeNode->setFileMD5(sMD5);
            if (!global_cloudfile_exist(sMD5.toLocal8Bit().data()))
            {
                continue;
            }
        }

        pTreeNode->setFileName(sValue.mid(sValue.lastIndexOf("/") + 1));
        sValue = sValue.mid(sList[i].indexOf(",") + 2);
        int nSub = sValue.lastIndexOf("/");
        pTreeNode->setFilePath(sValue.mid(0, -1 == nSub ? 0 : nSub));
        int nCount = sValue.count("/");
        if (0 == nCount)
        {
            pTreeNode->setParent(this);
            m_oFileList.push_back(pTreeNode);
            sort(m_oFileList.begin(), m_oFileList.end(), comp);
        }
        else
        {
            pTreeNode->setParent(vecLevel[nCount - 1]);
            vecLevel[nCount - 1]->fileList().push_back(pTreeNode);
            sort(vecLevel[nCount - 1]->fileList().begin(), vecLevel[nCount - 1]->fileList().end(), comp);
        }
        vecLevel[nCount] = pTreeNode;
    }
}

void CTreeAnalysis::setFileName(const QString &filename)
{
    m_sFileName = filename;
}

const QString &CTreeAnalysis::fileName()
{
    return m_sFileName;
}

void CTreeAnalysis::setFileMD5(const QString &fileMD5)
{
    m_sFileMD5 = fileMD5;
}

const QString &CTreeAnalysis::fileMD5()
{
    return m_sFileMD5;
}

void CTreeAnalysis::setFileType(CTreeAnalysis::eFileType type)
{
    m_eType = type;
}

CTreeAnalysis::eFileType CTreeAnalysis::fileType()
{
    return m_eType;
}

QVector<CTreeAnalysis *> &CTreeAnalysis::fileList()
{
    return m_oFileList;
}

void CTreeAnalysis::setParent(CTreeAnalysis *parent)
{
    m_pParent = parent;
}

CTreeAnalysis *CTreeAnalysis::parent()
{
    return m_pParent;
}

void CTreeAnalysis::setFilePath(const QString &filePath)
{
    m_sFilePath = filePath;
}

const QString &CTreeAnalysis::filePath()
{
    return m_sFilePath;
}

