#ifndef CPIO_ENTRY_H
#define CPIO_ENTRY_H

#include "QByteArray"
#include "QString"

#define CPIO_MASK_FILE 		0170000  //This masks the file type bits.
#define CPIO_MODE_SOCKET    0140000  //File type value for sockets.
#define CPIO_MODE_LINK		0120000  //File type value for symbolic links.  For symbolic links,  the link body is stored as file data.
#define CPIO_MODE_REGULAR	0100000  //File type value for regular files.
#define CPIO_MODE_BLOCK		0060000  //File type value for block special devices.
#define CPIO_MODE_DIR		0040000  //File type value for directories.
#define CPIO_MODE_CHAR		0020000  //File type value for character special devices.
#define CPIO_MODE_FIFO		0010000  //File type value for named pipes or FIFOs.
#define CPIO_MODE_SUID		0004000  //SUID bit.
#define CPIO_MODE_SGID		0002000  //SGID bit.
#define CPIO_MASK_STICKY    0001000  //Sticky bit.  On some systems, this modifies the behavior      of executables and/or directories.
#define CPIO_MASK_PERM		0000777  //The lower 9 bits specify read/write/execute permissions for world, group, and user following standard POSIX conventions.



struct cpio_newc_header {
    char    c_magic[6];
    char    c_ino[8];
    char    c_mode[8];
    char    c_uid[8];
    char    c_gid[8];
    char    c_nlink[8];
    char    c_mtime[8];
    char    c_filesize[8];
    char    c_devmajor[8];
    char    c_devminor[8];
    char    c_rdevmajor[8];
    char    c_rdevminor[8];
    char    c_namesize[8];
    char    c_check[8];
};


class CPIO_Entry
{
private:
    QByteArray m_data;
    QByteArray m_header;
    QString m_fileName;

    quint32 m_fileSize;
    quint32 m_fileMode;

    bool m_valid;

    quint32 readFromStream(QByteArray stream, const quint32 offset);
    static QString toString(char *v);

    long calculateTotalLength();
public:
    CPIO_Entry();
    CPIO_Entry(QByteArray stream, quint32 offset);

    CPIO_Entry &operator =(const CPIO_Entry &entry);


    bool isValid(){return m_valid;}
    QString getFileName(){return m_fileName;}
    quint32 getTotalLength(){return m_header.size() + m_data.size();}

    QByteArray getHeader(){return m_header; }
    QByteArray getData(){return m_data;}


    bool updateData(QByteArray newData);
};

#endif // CPIO_ENTRY_H
