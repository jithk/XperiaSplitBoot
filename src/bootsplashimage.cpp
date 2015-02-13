#include "bootsplashimage.h"
#include "qfile.h"
#include "QPainter"
#include "QImage"

#include "QDebug"

#define to565(r,g,b)    ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))


#define from565_r(x) ((((x) >> 11) & 0x1f) * 255 / 31)
#define from565_g(x) ((((x) >> 5) & 0x3f) * 255 / 63)
#define from565_b(x) (((x) & 0x1f) * 255 / 31)

typedef struct
{
    int width;
    int height;
} resolution_pair;

resolution_pair   resolutionValues[] = {  {720,1280},                                       
                                       {480,800},
                                       {480,854} ,
                                       {960,1440},
                                       {1200,1920},
                                       {1400,2560},
                                       {1536,2048},
                                       {1080,1920}};


BootSplashImage::BootSplashImage()
{

}

BootSplashImage::~BootSplashImage()
{

}




bool BootSplashImage::Open(QString fileName)
{

    QByteArray data = loadFile(fileName);
    if(data.isEmpty())
        return false;

    return m_image.loadFromData(data);
}

bool BootSplashImage::Save(QString fileName)
{
    if(m_image.isNull() == false && m_image.width() > 0 && m_image.height() > 0)
    {
        return m_image.save(fileName);
    }

    return false;
}


QByteArray BootSplashImage::toRLE()
{
    if(m_image.isNull())
        return QByteArray();

    int width = m_image.width();
    int height = m_image.height();

    if(width == 0 || height == 0)
        return QByteArray();

    quint16 prevColor = 0, times = 0;

    quint16 color = 0;
    QRgb *scanLine;
    QByteArray result;

    for(int i =0; i <height;i++)
    {
        scanLine = (QRgb *)m_image.scanLine(i);
        for(int j = 0; j < width; j++)
        {
            color = to565(qRed(scanLine[j]),qGreen(scanLine[j]),qBlue(scanLine[j]));
            if(color != prevColor && times > 0)
            {
                result.append((char*)&times,sizeof(quint16));
                result.append((char*)&prevColor,sizeof(quint16));
                times  = 0;
            }
            prevColor = color;
            times ++;
            if(times >= 0xFFFF)
            {
                result.append((char*)&times,sizeof(quint16));
                result.append((char*)&prevColor,sizeof(quint16));
                times  = 0;
            }
        }
    }
    if(times > 0)
    {
        result.append((char*)&times,sizeof(quint16));
        result.append((char*)&prevColor,sizeof(quint16));
        times  = 0;
    }

    return result;

}

bool BootSplashImage::saveAsRLE(QString fileName)
{


    QByteArray outData = toRLE();
    if(outData.isEmpty())
        return false;

    QFile outFile(fileName);
    outFile.open(QFile::WriteOnly | QFile::Truncate);
    if(outFile.isWritable() == false)
        return false;

    outFile.write(outData);

    return true;

}

bool BootSplashImage::fromRLEData(QByteArray rawData,int width, int height)
{
    if(rawData.isEmpty())
        return false;

    quint16 *data = (quint16*)rawData.data();
    quint16 times = 0, color = 0;
    long currentOffset = 0;

    QRgb outColor;

    m_image  = QImage(width,height,QImage::Format_ARGB32_Premultiplied);

    if(m_image.isNull())
        return false;

    QRgb *scanLine;
    for(int i =0;i<height;i++)
    {
        scanLine = (QRgb*)m_image.scanLine(i);

        for(int j=0;j<width;j++)
        {
            if(times == 0)
            {
                times = data[currentOffset++];
                color = data[currentOffset++];

                outColor = qRgb(from565_r(color),from565_g(color),from565_b(color));

            }
            scanLine[j]=outColor;
            times--;
        }

    }

    return true;

}


/**
  Calculates the approximate width and height of the image
 */
bool BootSplashImage::approximateSize(QByteArray rawData, int *width, int *height)
{
    if(rawData.isEmpty())
        return false;

    quint16 *data = (quint16*)rawData.constData();
    quint16 times = 0;
    long total = 0;
    long pairCount = rawData.size()/2;

    for(int j=0;j<pairCount;j+=2)
    {
        times = data[j];
        total+=times;
    }

    long resolution;
    for(int i=0;i<sizeof(resolutionValues);i++)
    {
        resolution = resolutionValues[i].height * resolutionValues[i].width;
        if(resolution == total)
        {
            *height = resolutionValues[i].height;
            *width = resolutionValues[i].width;
            return true;
        }

    }

    return false;

}

bool BootSplashImage::fromRLEFile(QString fileName, int width, int height)
{
    QByteArray fileData = loadFile(fileName);

    return fromRLEData(fileData,width,height);

}

QByteArray BootSplashImage::loadFile(QString fileName)
{
    QFile f(fileName);
    f.open(QFile::ReadOnly);

    if(f.isReadable() == false)
        return QByteArray();

    return f.readAll();

}


