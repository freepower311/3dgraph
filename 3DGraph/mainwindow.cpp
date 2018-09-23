#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer.setInterval(16);  //62.5 fps
    timer.start();
    connect(&timer, SIGNAL(timeout()), this, SLOT(updateScene()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void  MainWindow::updateScene(){
    ui->GLScene->update();
}
