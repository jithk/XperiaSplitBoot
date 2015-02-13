#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QString"
#include "QFile"
#include "QDebug"
#include "QSettings"


#include "QDragEnterEvent"
#include "QDropEvent"
#include "QMimeData"

#include "stdarg.h"

#include "splitbootimagedialog.h"
#include "makebootimage.h"

#include "utils.h"

DEFINE_SETTING_KEY(CONFIRM_RESOLUTION);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAcceptDrops(true);
    Utils::setParent((void*)this);

    ui->actionAlways_Confirm_Device_Resolution->setChecked(
                getApplicationSettings(CONFIRM_RESOLUTION,false).toBool());
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> list  = event->mimeData()->urls();

        /* donot support dragging more than one file*/
        if(list.size() != 1)
            return;

        if(list[0].url().endsWith(".png",Qt::CaseInsensitive)
                || list[0].url().endsWith(".jpg",Qt::CaseInsensitive)
                || list[0].url().endsWith(".img",Qt::CaseInsensitive)
                || list[0].url().endsWith(".raw",Qt::CaseInsensitive)
                || list[0].url().contains('.',Qt::CaseInsensitive) == false)
            event->acceptProposedAction();

    }



}


void MainWindow::dropEvent(QDropEvent *event)
{

    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> list  = event->mimeData()->urls();

        /* donot support dragging more than one file*/
        if(list.size() != 1)
            return;

        if(list[0].url().endsWith(".png",Qt::CaseInsensitive) ||
                          list[0].url().endsWith(".jpg",Qt::CaseInsensitive))
        {
            changeSplashImage(list[0].path());
            event->acceptProposedAction();
        }

        if(list[0].url().endsWith(".raw",Qt::CaseInsensitive) ||
                          list[0].url().endsWith(".img",Qt::CaseInsensitive) ||
                          list[0].url().contains('.',Qt::CaseInsensitive) == false
                )
        {
            openBootImageFile(list[0].path());
            event->acceptProposedAction();
        }

    }

    event->acceptProposedAction();
}

MainWindow::~MainWindow()
{
    setApplicationSettings(CONFIRM_RESOLUTION,ui->actionAlways_Confirm_Device_Resolution->isChecked());
    delete ui;
}


void MainWindow::addLog(QString text)
{
    ui->statusLog->appendHtml(text.replace("\n","<br>"));
}


void MainWindow::addLog(const char* format,...)
{

    QString log;
    va_list va;
    va_start(va,format);
    log.vsprintf(format,va);
    va_end(va);

    addLog(log);


}

QVariant MainWindow::getApplicationSettings(QString key,QVariant defaultValue)
{
     QSettings settings("XperiaTools", "XperiaToolsApplication");
     return settings.value(key,defaultValue);
}


void MainWindow::setApplicationSettings(QString key,QVariant value)
{
     QSettings settings("XperiaTools", "XperiaToolsApplication");
     settings.setValue(key,value );
}



void MainWindow::on_actionSplit_BootImage_triggered()
{
    SplitBootImageDialog splitter(this);
    splitter.exec();
}

void MainWindow::on_actionMake_Boot_Image_triggered()
{

    MakeBootImage mk(this);


    mk.exec();

}

#include "bootimagemanager.h"
#include "cpioinflater.h"
#include "cpio_entry.h"
#include "QDebug"
#include "bootsplashimage.h"
#include "QInputDialog"
#include "QStringList"
#include "QString"

bool MainWindow::openBootImageFile(QString fileName)
{
    m_manager = BootImageManager();
    if(m_manager.Open(fileName) == false)
        return false;

    if(m_manager.get_ramdiskData().isEmpty())
        return false;

    CPIOInflater inflater;
    inflater.inflate(m_manager.get_ramdiskData());

    qDebug()<<"Found ramdisk with "<<inflater.getCount()<<" entries";

    CPIO_Entry &e = inflater.getEntry("logo.rle");
    if(e.isValid() == false)
    {
        qDebug()<<"Cant find logo.rle";
        return true;
    }

    BootSplashImage img;
    int _width = 0,_height = 0;
    if(BootSplashImage::approximateSize(e.getData(),&_width,&_height) == true
            && ui->actionAlways_Confirm_Device_Resolution->isChecked() == false)
    {
        qDebug()<<"Width  = "<<_width<<" Height ="<<_height;
    }
    else
    {
        QString currentRes;

        if(_width && _height)
             currentRes = QString("%1x%2").arg(_height).arg(_width);

        QString resolutionString = QInputDialog::getText(this,
                                                         "Device Resolution",
                                                         "Please enter the device resolution\n"
                                                         "Example: 1920x1080 ",
                                                         QLineEdit::Normal,
                                                         currentRes);
        if(resolutionString.isEmpty())
            return false;

        QStringList l = resolutionString.split('x',QString::SkipEmptyParts,Qt::CaseInsensitive);
        if(l.size() != 2)
            return false;

        _width = l[1].toInt();
        _height = l[0].toInt();
    }

    if(img.fromRLEData(e.getData(),_width,_height) == false)
    {
        return true;
    }

    QImage scaledImage = img.getImage().scaledToHeight(height()*.8,Qt::SmoothTransformation);

    ui->bootImage->setPixmap(QPixmap::fromImage(scaledImage));

    return true;

}

bool MainWindow::changeSplashImage(QString imageFileName)
{
    if(m_manager.get_ramdiskData().isEmpty())
        return false;

    CPIOInflater inflater;
    inflater.inflate(m_manager.get_ramdiskData());

    qDebug()<<"Found ramdisk with "<<inflater.getCount()<<" entries";

    CPIO_Entry &e = inflater.getEntry("logo.rle");
    if(e.isValid() == false)
    {
        qDebug()<<"Cant find logo.rle";
        return true;
    }

    BootSplashImage img;
    int _width,_height;
    if(BootSplashImage::approximateSize(e.getData(),&_width,&_height) == true)
    {
        qDebug()<<"Width  = "<<_width<<" Height ="<<_height;
    }

    if(img.Open(imageFileName) == false)
        return false;

    QImage scaledImage = img.getImage().scaled(_width,_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

    img.setImage(scaledImage);

    ui->bootImage->setPixmap(QPixmap::fromImage(scaledImage.scaledToHeight(height()*.80,Qt::SmoothTransformation)));

    e.updateData(img.toRLE());

    m_manager.set_ramdiskData(inflater.compressEntries());


//    Utils::saveFile(inflater.compressEntries(),"/home/jith/tmp/boot/k.cpio.gz");

    return true;

}

#include "QFileDialog"
#include "QMessageBox"

void MainWindow::on_actionOpen_Boot_Image_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open Boot Image");
    if(fileName.isEmpty())
        return;

    if(openBootImageFile(fileName) == false)
    {
        QMessageBox::critical(this,"Error","Error while loading the boot image");
    }

    return;


}

void MainWindow::on_actionExtract_Components_triggered()
{
    if(m_manager.get_kernelData().isEmpty())
    {
        QMessageBox::warning(this,"Load boot image first","Please open a boot image first. ");
        return;
    }

    QString lastDir = getApplicationSettings(LAST_OUTPUT_FOLDER,"").toString();
    QString dirName = QFileDialog::getExistingDirectory(this,"Save components into...",lastDir);

    if(dirName.isEmpty())
        return;

    QDir saveDir(dirName);
    Utils::saveFile(m_manager.get_kernelData(),saveDir.absoluteFilePath("kernel") );

    if(m_manager.get_ramdiskData().isEmpty() == false)
    {
        Utils::saveFile(m_manager.get_ramdiskData(),saveDir.absoluteFilePath("ramdisk.cpio.gz") );
    }


    if(m_manager.get_secondData().isEmpty() == false)
    {
        Utils::saveFile(m_manager.get_secondData(),saveDir.absoluteFilePath("second.img") );
    }

    if(m_manager.get_deviceTreeData().isEmpty() == false)
    {
        Utils::saveFile(m_manager.get_deviceTreeData(),saveDir.absoluteFilePath("dtree") );
    }

    if(m_manager.get_commandLine().isEmpty() == false)
    {
        Utils::saveFile(m_manager.get_commandLine().toLocal8Bit().data(),
                        saveDir.absoluteFilePath("commandline") );
    }

    setApplicationSettings(LAST_OUTPUT_FOLDER,dirName);

}

void MainWindow::on_actionSave_Boot_Image_triggered()
{
    if(m_manager.get_kernelData().isEmpty())
    {
        QMessageBox::warning(this,"Load boot image first","Please open a boot image first. ");
        return;
    }

    QString lastDir = getApplicationSettings(LAST_OUTPUT_FOLDER,"").toString();

    QString fileName = QFileDialog::getSaveFileName(this,"Save File as",lastDir,"Boot Image Files (*.img) ;; All files (*.*) ");

    if(fileName.isEmpty())
        return;

    Utils::saveFile(m_manager.packBootImage(),fileName);


}
