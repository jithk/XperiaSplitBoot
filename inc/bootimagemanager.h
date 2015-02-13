#ifndef BOOTIMAGEUNPACKER_H
#define BOOTIMAGEUNPACKER_H

#include "QString"
#include "QByteArray"

#define ENABLE_GET_SET(a,b) a get_##b(){return m_##b;} void set_##b(a _##b){m_##b=_##b;}
#define ENABLE_GET(a,b) a get_##b(){return m_##b;}

class BootImageManager
{

private:

    QByteArray m_kernelData;
    QByteArray m_ramdiskData;
    QByteArray m_secondData;
    QByteArray m_deviceTreeData;
    QString m_commandLine;



    quint32     m_pageSize;
    quint32     m_baseAddress;
    quint32     m_kernelAddress;
    quint32     m_ramdiskAddress;
    quint32     m_tagsOffset;
    quint32     m_secondAddress;


    QString m_errorString;


/* private functions */
private:

    bool openBootImageFormat(QString fileName);
    bool openElfFormat(QString fileName);

public:
    BootImageManager();
    ~BootImageManager();

    bool Open(const QString fileName);

    ENABLE_GET_SET(QByteArray,kernelData)
    ENABLE_GET_SET(QByteArray,ramdiskData)
    ENABLE_GET_SET(QByteArray,secondData)
    ENABLE_GET_SET(QByteArray,deviceTreeData)
    ENABLE_GET_SET(QString,commandLine)

    ENABLE_GET_SET(quint32,pageSize)
    ENABLE_GET_SET(quint32,kernelAddress)
    ENABLE_GET_SET(quint32,ramdiskAddress)
    ENABLE_GET_SET(quint32,tagsOffset)
    ENABLE_GET_SET(quint32,secondAddress)
    ENABLE_GET_SET(quint32,baseAddress)

    ENABLE_GET_SET(QString,errorString)

    QByteArray packBootImage();

    BootImageManager &operator=(const BootImageManager b);

};


#undef ENABLE_GET
#undef ENABLE_GET_SET

#endif // BOOTIMAGEUNPACKER_H
