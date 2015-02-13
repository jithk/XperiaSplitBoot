#include "splitbootimagedialog.h"
#include "ui_splitbootimagedialog.h"

#include "QFile"
#include "bootimage.h"
#include "QDir"

#include "utils.h"

#include <elfio/elfio.hpp>
using namespace ELFIO;

#include "QFileDialog"




SplitBootImageDialog::SplitBootImageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SplitBootImageDialog)
{
    ui->setupUi(this);

    ui->destFolderName->setText(MainWindow::getApplicationSettings(LAST_OUTPUT_FOLDER,"").toString());
    ui->fileName->setText(MainWindow::getApplicationSettings(LAST_BOOT_IMAGE,"").toString());
}

SplitBootImageDialog::~SplitBootImageDialog()
{
    MainWindow::setApplicationSettings(LAST_BOOT_IMAGE,ui->fileName->text());
    MainWindow::setApplicationSettings(LAST_OUTPUT_FOLDER,ui->destFolderName->text());
    delete ui;
}


void SplitBootImageDialog::on_openSource_clicked()
{
    QString fileName  = QFileDialog::getOpenFileName(this,"Open Boot Image");
    ui->fileName->setText(fileName);

    if(ui->destFolderName->text().isEmpty())
    {
        QFile file(fileName);
        QFileInfo info(file);
        ui->destFolderName->setText(info.dir().absolutePath());
    }


}


QString SplitBootImageDialog::getDestFileName()
{
    QString fileName = ui->fileName->text();
    QString dirName = ui->destFolderName->text();

    if(fileName.isEmpty() || dirName.isEmpty())
        return QString("unknown");

    QFileInfo info(fileName);
    QDir destInfo(dirName);

    return destInfo.absoluteFilePath(info.baseName());

}

QString SplitBootImageDialog::getKernelFileName()
{
    return getDestFileName()+"-kernel";
}

QString SplitBootImageDialog::getRamdiskFileName()
{
    return getDestFileName()+"-ramdisk.cpio.gz";
}

QString SplitBootImageDialog::getDTreeFileName()
{
    return getDestFileName()+"-dtree";
}


void SplitBootImageDialog::on_destFolderSelect_clicked()
{
    QString fileName = QFileDialog::getExistingDirectory(this,"Extract files into");
    ui->destFolderName->setText(fileName);
}




void SplitBootImageDialog::on_extractButton_clicked()
{
    QString fileName = ui->fileName->text();

    QString dest = ui->destFolderName->text();
    QDir destFolder(dest);
    if(destFolder.exists() == false)
        destFolder.mkpath(".");

    BootImageManager manager;

    if(!manager.Open(fileName))
    {
        Utils::appendLog(manager.get_errorString());
        return ;
    }

    if(manager.get_kernelData().isEmpty() == false)
    {
        Utils::saveFile(manager.get_kernelData(),getKernelFileName());
    }

    if(manager.get_ramdiskData().isEmpty() == false)
    {
        Utils::saveFile(manager.get_ramdiskData(),getRamdiskFileName());
    }

    if(manager.get_secondData().isEmpty() == false)
    {
        Utils::saveFile(manager.get_secondData(),getDestFileName()+"_extra");
    }

    if(manager.get_deviceTreeData().isEmpty() == false)
    {
        Utils::saveFile(manager.get_deviceTreeData(),getDTreeFileName());
    }

    if(manager.get_commandLine().isEmpty() == false)
    {
        Utils::saveFile(manager.get_commandLine().toLocal8Bit(),getDestFileName()+"_commandline");
    }



    accept();



}
