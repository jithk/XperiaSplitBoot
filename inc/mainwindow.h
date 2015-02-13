#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QVariant"

#include "bootimagemanager.h"

namespace Ui {
class MainWindow;
}

#define DEFINE_SETTING_KEY(a)   static const QString a(#a)

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addLog(QString text);
    void addLog(const char *format,...);


    static QVariant getApplicationSettings(QString key, QVariant defaultValue);
    static void setApplicationSettings(QString key, QVariant value);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    bool openBootImageFile(QString fileName);

    bool changeSplashImage(QString imageFileName);
private slots:

    void on_actionSplit_BootImage_triggered();

    void on_actionMake_Boot_Image_triggered();

    void on_actionOpen_Boot_Image_triggered();

    void on_actionExtract_Components_triggered();

    void on_actionSave_Boot_Image_triggered();

private:
    Ui::MainWindow *ui;

    BootImageManager m_manager;


};

#endif // MAINWINDOW_H
