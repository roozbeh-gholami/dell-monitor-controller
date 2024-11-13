#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMovie>
#include <QThread>
#include <ddcutil_c_api.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CommandWorker : public QObject {
    Q_OBJECT

public:
    explicit CommandWorker(int contrast) : contrast(contrast) {}

public slots:
    void executeCommand(std::string cmd_string);

signals:
    void commandFinished();

private:
    int contrast;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void setBrightness();
    int getCurrentBrightness();
    void setContrast();
    int getCurrentContrast();

private:
    Ui::MainWindow *ui;
    DDCA_Vcp_Feature_Code feature_handle;
    void setupMonitor();
    void cleanupMonitor();
};
#endif // MAINWINDOW_H
