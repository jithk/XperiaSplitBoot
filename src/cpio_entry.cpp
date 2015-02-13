#include "cpio_entry.h"

CPIO_Entry::CPIO_Entry()
{
    m_valid = false;
}

CPIO_Entry::CPIO_Entry(QByteArray stream, quint32 offset)
{
    m_valid = false;
    readFromStream(stream,offset) ;

}

CPIO_Entry & CPIO_Entry::operator =(const CPIO_Entry &entry)
{
    m_data = entry.m_data;
    m_header = entry.m_header;
    m_fileName = entry.m_fileName;
    m_fileMode = entry.m_fileMode;
    m_valid = entry.m_valid;
    m_fileSize = entry.m_fileSize;

    return *this;
}



QString CPIO_Entry::toString(char *v)
{
    QString result;
    result.sprintf("0x%c%c%c%c%c%c%c%c",v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7]);

    return result;
}

bool CPIO_Entry::updateData(QByteArray newData)
{
    if(!m_valid)
        return false;

    m_fileSize = newData.size();

    QString size = QString().sprintf("%08x",newData.size());

    cpio_newc_header * header;

    header = (cpio_newc_header*)m_header.data();

    memcpy(header->c_filesize,size.toLocal8Bit().constData(),sizeof(header->c_filesize));

    m_data.clear();
    m_data = newData;

    /* add a trailing 0 to make size multiple of 4*/
    if((m_fileSize & 3) > 0)
    {
        int padding =  (4 - (m_fileSize & 3));
        QByteArray pad(padding,0);
        m_data.append(pad);
    }

    return true;

}


quint32 CPIO_Entry::readFromStream(QByteArray stream, const quint32 offset)
{


    cpio_newc_header * header;
    const char * data = stream.data() + offset ;


    header = (cpio_newc_header*)data;

    if(strncmp(header->c_magic,"07070",sizeof(header->c_magic) - 1)!= 0)
        return 0;

    const char *filename = data + sizeof(cpio_newc_header);

    bool ok = true;

    m_fileSize = toString(header->c_filesize).toLong(&ok,16);
    m_fileMode = toString(header->c_mode).toLong(&ok,16);

    int fileNameSize = toString(header->c_namesize).toLong(&ok,16);

    if(((fileNameSize + sizeof(cpio_newc_header)) & 3) > 0)
        fileNameSize += 4 - ((fileNameSize + sizeof(cpio_newc_header)) & 3);

    m_fileName = QString(filename);

    long processed  = m_fileSize + fileNameSize + sizeof(cpio_newc_header);

    /* add a trailing 0 to make size multiple of 4*/
    if((m_fileSize & 3) > 0)
        processed += (4 - (m_fileSize & 3));

    m_header = QByteArray(data, sizeof(cpio_newc_header) + fileNameSize);
    m_data = QByteArray(data + m_header.size(),
                        processed - m_header.size());


    m_valid = true;

    return processed + offset;

}

long CPIO_Entry::calculateTotalLength()
{
    long length = 0;
    length += sizeof(cpio_newc_header) + m_fileName.length() +1;

    if(length%4 > 0)
    {
        length = length + (4 - length%4);
    }

    length += m_fileSize;
    if(m_fileSize%4 > 0)
    {
        length += 4 - (m_fileSize%4);
    }


    return length;


}


