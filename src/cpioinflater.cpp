#include "cpioinflater.h"
#include "QString"
#include "QByteArray"

#include "QDebug"
#include "utils.h"

CPIOInflater::CPIOInflater()
{

}

bool CPIOInflater::inflate(QByteArray compressedData)
{

    if(compressedData.isEmpty())
        return false;

    m_fileList.clear();

    m_compressedData = compressedData;

    Utils::gzipDecompress(compressedData,m_uncompressedData);// ;


    long offset = 0;

    do
    {
        CPIO_Entry e(m_uncompressedData,offset);

        if(e.getTotalLength() == 0)
            break;

        offset += e.getTotalLength();

        m_fileList.append(e);

    }while(offset < m_uncompressedData.size());

    qDebug()<<"entriesList has "<<m_fileList.size()<<" entries";


    return true;
}

CPIO_Entry & CPIOInflater::getEntry(int i)
{
    static    CPIO_Entry ret;
    if(m_fileList.isEmpty())
        return ret;

    return m_fileList[i];
}

CPIO_Entry & CPIOInflater::getEntry(QString path)
{
    static     CPIO_Entry ret;

    for(int i =0;i< m_fileList.size();i++)
    {
        if(m_fileList[i].getFileName().compare(path) == 0)
            return m_fileList[i];
    }

    return ret;
}



QByteArray CPIOInflater::compressEntries()
{
    int count = getCount();
    QByteArray out;
    for(int i =0;i<count;i++)
    {
        CPIO_Entry e = getEntry(i);

        out.append(e.getHeader());
        out.append(e.getData());
    }

    QByteArray compressed;
    Utils::gzipCompress(out,compressed);

    return compressed;
}
