#ifndef CPIOINFLATER_H
#define CPIOINFLATER_H


/** Currently only supports newc format
 * @brief The CPIOInflater class
 */

#include "QByteArray"
#include "QList"
#include "cpio_entry.h"


class CPIOInflater
{
private:
    QByteArray m_uncompressedData;
    QByteArray m_compressedData;
    QList<CPIO_Entry> m_fileList;

public:

    CPIOInflater();
    bool inflate(QByteArray compressedData);
    CPIO_Entry & getEntry(int i);
    CPIO_Entry & getEntry(QString path);

    int getCount(){return m_fileList.size();}
    QList<CPIO_Entry> getList(){return m_fileList;}

    QByteArray compressEntries();
};

#endif // CPIOINFLATER_H
