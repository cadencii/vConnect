#include "TranscriberWindow.h"
#include "ui_TranscriberWindow.h"

TranscriberWindow::TranscriberWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TranscriberWindow)
{
    ui->setupUi(this);
}

TranscriberWindow::~TranscriberWindow()
{
    delete ui;
}
