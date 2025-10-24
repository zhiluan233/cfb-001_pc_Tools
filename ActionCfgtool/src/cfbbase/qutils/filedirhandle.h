


#ifndef CFILEDIRHANDLE_H
#define CFILEDIRHANDLE_H

#include "qutils_global.h"
#include <QByteArray>
#include <QStringList>
#include <QDir>

const QString DIR_SPLIT_CHAR = QDir::separator(); //QDir::separator() "/"


class QUTILS_EXPORT CFileDirHandle
{
public:
    CFileDirHandle();
    ~CFileDirHandle();

public:

    
    static QStringList getRootDirPaths(const QString& rootPath);
    static QStringList getRootFiles(const QString& rootPath);
    //get the files with filescondition in top dir of rootpath
    static QStringList getRootFiles(const QString& rootPath, const QString& filesCondition);
    static QStringList getRootDirAndFiles(const QString& rootPath);

    //get the file parent dir Path（//返回父亲文件夹路径）
    static QString getFileDirPath(const QString& filePath);


    
    static void clearDir(const QString &fullPath, bool delMainDir);

    static bool createDir(const QString& strDirPath);

    static bool mkpath(const QString& strPath);

    static bool isDirExist(const QString& strDir);
    
    static bool saveFile(const QString& filePath, const QByteArray& data);
    //save data to file of filePath
    static bool saveFile(const QString& filePath, const char *const pData, const int nDataLen);

    static qint64 read(const QString& filePath, char** pFileData, qint64 maxlen = 0);

    //file name with suffix
    static QString getFileName(const QString& filePath);
    //file base without suffix
    static QString getFileBaseName(const QString& filePath);
    //获取文件后缀, with out "."
    static QString getFileSuffix(const QString &filePath, bool bPostfixToLower);
    static QString getFileSuffix(const QString &filePath);


    //generat the new file name by new file base name(change the file base name)
    static QString generateFileNameByBaseName(const QString& oldFileName, const QString& newFileBaseName);
    //change the file suffix
    static QString generateFileNameBySuffix(const QString& oldFileName, const QString& newFileSuffix);

    //
    static bool copyFile(const QString& sourceFile, const QString& desFile, bool bCovered);

    //the file select format to QFileDialog
    static QString getFileSelectFormat(const QString& strFmtTitle, const QString& strFmtFilter);
    //generate the file path
    static QString getFilePath(const QString& fileDir, const QString& fileName);


    //for get the upload file byteArr to upload format
    static bool getFileByteArrayForUpload(const QString& filePath, QByteArray& bytesArr);


    //get file size
    static qint64 getFileSize(const QString strFilePath);
    //file size dB To KB
    static qint64 dBToKB(const qint64 nSize);
    //file size dB To MB
    static qint64 dBToMB(const qint64 nSize);
    //file size dB To GB
    static qint64 dBToGB(const qint64 nSize);

};

#endif // CFILEDIRHANDLE_H
