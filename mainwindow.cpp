#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib>
#include <QProcess>
#include <QRegularExpression>
#include <iostream>
#include <QMovie>
#include <QSize>
#include <QTimer>
#include <QThread>

void CommandWorker::executeCommand(std::string cmd_string) {
    std::string command = cmd_string + std::to_string(contrast);
    std::cout << "Executing command: " << command << std::endl;
    std::system(command.c_str());
    emit commandFinished();
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    int initialBrightness = MainWindow::getCurrentBrightness();
    ui->brightSpinBox->setValue(initialBrightness);
    ui->brightnessSlider->setValue(initialBrightness);

    int initialContrast = MainWindow::getCurrentContrast();
    ui->contrastSpinBox->setValue(initialContrast);
    ui->contrastSlider->setValue(initialContrast);

    // Update spinbox as the slider moves, but do not set brightness
    connect(ui->brightnessSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->brightSpinBox->setValue(value);
    });

    // Set brightness only when the slider is released
    connect(ui->brightnessSlider, &QSlider::sliderReleased, this, &MainWindow::setBrightness);

    // Sync slider with spinbox changes, but only update brightness on slider release
    connect(ui->brightSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        ui->brightnessSlider->setValue(value);
    });

    // Update spinbox as the slider moves, but do not set brightness
    connect(ui->contrastSlider, &QSlider::valueChanged, this, [this](int value) {
        ui->contrastSpinBox->setValue(value);
    });

    // Set brightness only when the slider is released
    connect(ui->contrastSlider, &QSlider::sliderReleased, this, &MainWindow::setContrast);

    // Sync slider with spinbox changes, but only update brightness on slider release
    connect(ui->contrastSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int value) {
        ui->contrastSlider->setValue(value);
    });
}

int MainWindow::getCurrentBrightness() {
    QProcess process;
    QString program = "ddcutil";
    QStringList arguments;
    arguments << "getvcp" << "10";

    process.start(program, arguments);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QRegularExpression regex("current value =\\s*(\\d+)");
    QRegularExpressionMatch match = regex.match(output);

    if (match.hasMatch()) {
        int currentBrightness = match.captured(1).toInt();
        std::cout << "Current Brightness: " << currentBrightness << std::endl;
        return currentBrightness;
    }
    return 50;
}

void MainWindow::setBrightness() {
    // Load the GIF using QMovie
    QMovie *movie = new QMovie("://loading.gif");
    if (!movie->isValid()) {
        qWarning("GIF could not be loaded!");
    }
    QSize newSize(50, 50); // Width and height in pixels
    movie->setScaledSize(newSize);
    // Set the QMovie to the QLabel
    ui->loading_lbl->setMovie(movie);
    // Start the GIF animation
    movie->start();
    int brightness = ui->brightnessSlider->value();
    std::string cmd_string = "ddcutil setvcp 10 ";
    // Create the worker and a new thread
    CommandWorker *worker = new CommandWorker(brightness);
    QThread *thread = new QThread;

    // Move the worker to the new thread
    worker->moveToThread(thread);

    // Use lambda to pass cmd_string when the thread starts
    connect(thread, &QThread::started, [worker, cmd_string]() {
        worker->executeCommand(cmd_string);
    });
    // Stop the loading animation and clean up when command finishes
    connect(worker, &CommandWorker::commandFinished, this, [this, movie, thread, worker]() {
        movie->stop();                // Stop the loading animation
        ui->loading_lbl->clear();    // Clear the movie from the QLabel
        thread->quit();               // End the thread
        thread->wait();               // Wait for the thread to finish
        worker->deleteLater();        // Delete worker
        thread->deleteLater();        // Delete thread
    });

    // Start the thread
    thread->start();
}

void MainWindow::setContrast(){
    // Load the GIF using QMovie
    QMovie *movie = new QMovie("://loading.gif");
    if (!movie->isValid()) {
        qWarning("GIF could not be loaded!");
    }
    QSize newSize(50, 50); // Width and height in pixels
    movie->setScaledSize(newSize);
    // Set the QMovie to the QLabel
    ui->loading_lbl->setMovie(movie);
    // Start the GIF animation
    movie->start();
    int contrast = ui->contrastSlider->value();
    std::string cmd_string = "ddcutil setvcp 12 ";
    // Create the worker and a new thread
    CommandWorker *worker = new CommandWorker(contrast);
    QThread *thread = new QThread;

    // Move the worker to the new thread
    worker->moveToThread(thread);

    // Use lambda to pass cmd_string when the thread starts
    connect(thread, &QThread::started, [worker, cmd_string]() {
        worker->executeCommand(cmd_string);
    });
    // Stop the loading animation and clean up when command finishes
    connect(worker, &CommandWorker::commandFinished, this, [this, movie, thread, worker]() {
        movie->stop();                // Stop the loading animation
        ui->loading_lbl->clear();    // Clear the movie from the QLabel
        thread->quit();               // End the thread
        thread->wait();               // Wait for the thread to finish
        worker->deleteLater();        // Delete worker
        thread->deleteLater();        // Delete thread
    });

    // Start the thread
    thread->start();
}

int MainWindow::getCurrentContrast(){
    QProcess process;
    QString program = "ddcutil";
    QStringList arguments;
    arguments << "getvcp" << "12";

    process.start(program, arguments);
    process.waitForFinished();

    QString output = process.readAllStandardOutput();
    QRegularExpression regex("current value =\\s*(\\d+)");
    QRegularExpressionMatch match = regex.match(output);

    if (match.hasMatch()) {
        int currentContrast = match.captured(1).toInt();
        std::cout << "Current Contrast: " << currentContrast << std::endl;
        return currentContrast;
    }
    return 50;
}

MainWindow::~MainWindow()
{
    delete ui;
}
