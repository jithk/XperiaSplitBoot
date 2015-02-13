#ifndef SPLITBOOTIMAGEDIALOG_H
#define SPLITBOOTIMAGEDIALOG_H

#include <QDialog>
#include "mainwindow.h"

DEFINE_SETTING_KEY(LAST_OUTPUT_FOLDER);
DEFINE_SETTING_KEY(LAST_BOOT_IMAGE);


namespace Ui {
class SplitBootImageDialog;
}

class SplitBootImageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SplitBootImageDialog(QWidget *parent = 0);
    ~SplitBootImageDialog();

#if 0
    bool processElfFile(QString fileName);
    void on_toolButton_clicked();
    bool openBootImg(QByteArray data);
    bool processBootImage(QString fileName);
#endif
    QString getDTreeFileName();
    QString getRamdiskFileName();
    QString getKernelFileName();
    QString getDestFileName();


private slots:
    void on_destFolderSelect_clicked();

    void on_openSource_clicked();

    void on_extractButton_clicked();

private:
    Ui::SplitBootImageDialog *ui;

};

#endif // SPLITBOOTIMAGEDIALOG_H
