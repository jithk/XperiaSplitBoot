#ifndef MAKEBOOTIMAGE_H
#define MAKEBOOTIMAGE_H

#include <QDialog>
#include "bootimagemanager.h"
#include "mainwindow.h"

#include "QLineEdit"

DEFINE_SETTING_KEY(LAST_KERNEL_NAME);
DEFINE_SETTING_KEY(LAST_RAMDISK_NAME);
DEFINE_SETTING_KEY(LAST_DTREE_NAME);
DEFINE_SETTING_KEY(LAST_SECOND_NAME);
DEFINE_SETTING_KEY(LAST_COMMANDLINE);

DEFINE_SETTING_KEY(LAST_OUPUT_FILE);

static const QString defaultCommandLine = "androidboot.hardware=qcom "
        "user_debug=31 maxcpus=2 msm_rtb.filter=0x3F "
        "ehci-hcd.park=3 msm_rtb.enable=0 lpj=192598 "
        "dwc3.maximum_speed=high dwc3_msm.prop_chg_detect=Y";



namespace Ui {
class MakeBootImage;
}

class MakeBootImage : public QDialog
{
    Q_OBJECT

public:
    explicit MakeBootImage(QWidget *parent = 0);
    ~MakeBootImage();

private slots:
    void on_kernelSelect_clicked();

    void on_ramdiskSelect_clicked();

    void on_deviceTreeSelect_clicked();

    void on_secondImageSelect_clicked();

    void on_makeButton_clicked();

    void on_outputFileSelect_clicked();

    void on_pageSize_editingFinished();

    void on_baseAddress_editingFinished();

    void on_kernelOffset_editingFinished();

    void on_ramdiskOffset_editingFinished();

    void on_tagOffset_editingFinished();

//    void on_ramdiskUncompress_clicked();

private:
    Ui::MakeBootImage *ui;
    bool validateParameters();

    BootImageManager m_imageManager;
    void validateTextEntry(QLineEdit *entry);
    bool loadComponents();
};

#endif // MAKEBOOTIMAGE_H
