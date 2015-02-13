#include "makebootimage.h"
#include "ui_makebootimage.h"
#include "QFileDialog"
#include "QMessageBox"


#include "mainwindow.h"
#include "QPalette"


MakeBootImage::MakeBootImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MakeBootImage)
{
    ui->setupUi(this);

    ui->baseAddress->setText(QString().sprintf("0x%08X",m_imageManager.get_baseAddress()));
    ui->pageSize->setText(QString("").sprintf("0x%08X",m_imageManager.get_pageSize()));
    ui->kernelOffset->setText(QString("").sprintf("0x%08X",m_imageManager.get_kernelAddress()));
    ui->ramdiskOffset->setText(QString("").sprintf("0x%08X",m_imageManager.get_ramdiskAddress()));
    ui->tagOffset->setText(QString("").sprintf("0x%08X",m_imageManager.get_tagsOffset()));

    ui->kernelFileName->setText(MainWindow::getApplicationSettings(LAST_KERNEL_NAME,"").toString());
    ui->deviceTreeFileName->setText(MainWindow::getApplicationSettings(LAST_DTREE_NAME,"").toString());
    ui->ramdiskFileName->setText(MainWindow::getApplicationSettings(LAST_RAMDISK_NAME,"").toString());
    ui->secondFileName->setText(MainWindow::getApplicationSettings(LAST_SECOND_NAME,"").toString());

    ui->outputFileName->setText(MainWindow::getApplicationSettings(LAST_OUPUT_FILE,"").toString());

    ui->commandLine->setPlainText(MainWindow::getApplicationSettings(LAST_COMMANDLINE,
                                                                     QString(defaultCommandLine))
                                  .toString());


}

MakeBootImage::~MakeBootImage()
{
    MainWindow::setApplicationSettings(LAST_KERNEL_NAME,ui->kernelFileName->text());
    MainWindow::setApplicationSettings(LAST_DTREE_NAME,ui->deviceTreeFileName->text());
    MainWindow::setApplicationSettings(LAST_RAMDISK_NAME,ui->ramdiskFileName->text());
    MainWindow::setApplicationSettings(LAST_SECOND_NAME,ui->secondFileName->text());
    MainWindow::setApplicationSettings(LAST_OUPUT_FILE,ui->outputFileName->text());

    MainWindow::setApplicationSettings(LAST_COMMANDLINE,
                                       ui->commandLine->toPlainText().replace('\n',' '));

    delete ui;
}

void MakeBootImage::on_kernelSelect_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open Kernel");
    if(fileName.isEmpty() == false)
        ui->kernelFileName->setText(fileName);

}

void MakeBootImage::on_ramdiskSelect_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open Ramdisk");
    if(fileName.isEmpty() == false)
        ui->ramdiskFileName->setText(fileName);

}

void MakeBootImage::on_deviceTreeSelect_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open DeviceTree");
    if(fileName.isEmpty() == false)
        ui->deviceTreeFileName->setText(fileName);

}

void MakeBootImage::on_secondImageSelect_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Open Second Image");
    if(fileName.isEmpty() == false)
        ui->secondFileName->setText(fileName);

}


void MakeBootImage::on_outputFileSelect_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,"Save boot image as");
    if(fileName.isEmpty() == false)
        ui->outputFileName->setText(fileName);

}


#include "utils.h"

bool MakeBootImage::validateParameters()
{

    QString kernel = ui->kernelFileName->text();
    QString ramdisk = ui->ramdiskFileName->text();
    QString cmdLine = ui->commandLine->toPlainText();

    if(kernel.isEmpty() ||
            ramdisk.isEmpty() ||            
            ui->outputFileName->text().isEmpty() ||
            cmdLine.isEmpty() )
        return false;

    bool ok = false;



    if(ui->baseAddress->text().isEmpty() ||
            ui->kernelOffset->text().isEmpty() ||
            ui->tagOffset->text().isEmpty() ||
            ui->ramdiskOffset->text().isEmpty() ||
            ui->pageSize->text().isEmpty())
        return false;

    unsigned val = 0;

    val = ui->baseAddress->text().toInt(&ok,16);
    if(!ok)return false;
    m_imageManager.set_baseAddress(val);

    val = ui->kernelOffset->text().toInt(&ok,16);
    if(!ok)return false;
    m_imageManager.set_kernelAddress(val);

    val = ui->tagOffset->text().toInt(&ok,16);
    if(!ok)return false;
    m_imageManager.set_tagsOffset(val);

    val = ui->ramdiskOffset->text().toInt(&ok,16);
    if(!ok)return false;
    m_imageManager.set_ramdiskAddress(val);

    val = ui->pageSize->text().toInt(&ok,16);
    if(!ok)return false;
    m_imageManager.set_pageSize(val);


    /* replace all newline*/
    m_imageManager.set_commandLine(cmdLine.replace('\n',' '));



    return true;
}

bool MakeBootImage::loadComponents()
{

    QString kernel = ui->kernelFileName->text();
    QString ramdisk = ui->ramdiskFileName->text();
    QString devTree = ui->deviceTreeFileName->text();


    m_imageManager.set_kernelData(Utils::loadFile(kernel));

    if(m_imageManager.get_kernelData().isEmpty())
        return false;

    m_imageManager.set_ramdiskData(Utils::loadFile(ramdisk));

    if(m_imageManager.get_ramdiskData().isEmpty())
        return false;

    m_imageManager.set_deviceTreeData(Utils::loadFile(devTree));

    m_imageManager.set_secondData(Utils::loadFile(ui->secondFileName->text()));

    return true;
}



void MakeBootImage::on_makeButton_clicked()
{
    if(validateParameters() == false)
    {
        QMessageBox::warning(this,"Invalid file names","Please specify all file names correctly");
        return;
    }


    if(loadComponents() == false )
    {
        QMessageBox::critical(this,"Error ","Error loading component files\n"+m_imageManager.get_errorString());
        return;
    }

    QByteArray bootImage = m_imageManager.packBootImage();
    QString out = ui->outputFileName->text();
    if(bootImage.isEmpty() )
    {
        QMessageBox::critical(this,"Error ","Error while making boot image\n"+m_imageManager.get_errorString());
        return;
    }

    if(!Utils::saveFile(bootImage,out))
    {
        QMessageBox::critical(this,"Error ","Failed to write to output file: "+out);
        return;
    }

    accept();

}




void MakeBootImage::validateTextEntry(QLineEdit *entry)
{
    QString text = entry->text().trimmed();

    QPalette palette;

    bool ok = false;
    quint32 val = text.toInt(&ok,0);
    if(!ok)
    {

        palette.setColor(QPalette::Text,Qt::red);
        entry->setPalette(palette);

        return;
    }

    palette.setColor(QPalette::Text,Qt::black);
    entry->setPalette(palette);

    text.sprintf("0x%08X",val);

    entry->setText(text);

}

void MakeBootImage::on_pageSize_editingFinished()
{
    validateTextEntry(ui->pageSize);
}

void MakeBootImage::on_baseAddress_editingFinished()
{
    validateTextEntry(ui->baseAddress);
}

void MakeBootImage::on_kernelOffset_editingFinished()
{
    validateTextEntry(ui->kernelOffset);
}

void MakeBootImage::on_ramdiskOffset_editingFinished()
{
    validateTextEntry(ui->ramdiskOffset);
}

void MakeBootImage::on_tagOffset_editingFinished()
{
    validateTextEntry(ui->tagOffset);
}

#if 0

#include "cpio_entry.h"
#include "cpioinflater.h"
#include "QDebug"
#include "utils.h"
void MakeBootImage::on_ramdiskUncompress_clicked()
{
    QString ramDiskFn  = ui->ramdiskFileName->text();
    if(ramDiskFn.isEmpty())
        return;

    QByteArray data = Utils::loadFile(ramDiskFn);

    CPIOInflater inf;
    inf.inflate(data);

    int count = inf.getCount();
    QByteArray out;
    for(int i =0;i<count;i++)
    {
        CPIO_Entry e = inf.getEntry(i);

        ((MainWindow*)parent())->addLog("Item : %s  [%d bytes]",
                                        e.getFileName().toLocal8Bit().data(),
                                        e.getTotalLength());

#if 0
        if(e.getFileName().compare("logo.rle") == 0)
        {
            ((MainWindow*)parent())->addLog("Found Logo");
            QByteArray d = e.toByteArray();
            CPIO_Entry m(d,0);
            ((MainWindow*)parent())->addLog("Item : %s  [%d bytes]",
                                            m.getFileName().toLocal8Bit().data(),
                                            m.getTotalLength());
        }
#endif

//        out.append(e.toByteArray());
        out.append(e.getHeader());
        out.append(e.getData());
    }

    QByteArray compressed;
    Utils::gzipCompress(out,compressed);

    QFile of("/home/jith/tmp/out.cpio.gz");
    of.open(QFile::WriteOnly | QFile::Truncate);

    of.write(compressed);

}

#endif
