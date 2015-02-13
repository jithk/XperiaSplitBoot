#ifndef UTILS_H
#define UTILS_H


#include "QByteArray"
#include "QString"

#include "zlib.h"

#define GZIP_WINDOWS_BIT  (15 + 16)
#define GZIP_CHUNK_SIZE   (32 * 1024)


class Utils
{
    static void *m_parent;

public:
    Utils();
    ~Utils();

    static QByteArray loadFile(const QString fileName);
    static bool saveFile(const char *data, long size, QString fileName);
    static bool saveFile(const QByteArray data, QString fileName);
    static bool gzipCompress(const QByteArray input, QByteArray &output, int level = -1);
    static bool gzipDecompress(const QByteArray input, QByteArray &output);

    static void addLog(const char *format,...);
    static void appendLog(QString log);
    static void setParent(void* parent){m_parent = parent;}
};

#endif // UTILS_H
