#ifndef BOOTSPLASHIMAGE_H
#define BOOTSPLASHIMAGE_H

#include "QString"
#include "QImage"

class BootSplashImage
{

private:
    QImage m_image;

public:
    BootSplashImage();
    ~BootSplashImage();



public:
    bool Open(QString fileName);
    bool Save(QString fileName);

    QImage getImage(){return m_image;}
    void setImage(QImage img){m_image = img;}

    bool fromRLEFile(QString fileName,int width, int height);

    static QByteArray loadFile(QString fileName);


    bool saveAsRLE(QString fileName);
    QByteArray toRLE();
    bool fromRLEData(QByteArray rawData, int width, int height);

    bool static approximateSize(QByteArray rawData, int*width, int*height);
};

#endif // BOOTSPLASHIMAGE_H
