#include "TranscriberWindow.h"
#include "ui_TranscriberWindow.h"

#include <QMessageBox>

using namespace stand::gui;

TranscriberWindow::TranscriberWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TranscriberWindow)
{
    ui->setupUi(this);

    while(ui->SettingTabs->count() != 0)
    {
        QWidget *p = ui->SettingTabs->widget(0);
        ui->SettingTabs->removeTab(0);
        delete p;
    }
    ui->SettingTabs->addTab(new TranscribeWidget(this), tr("Base"));
}

TranscriberWindow::~TranscriberWindow()
{
    delete ui;
}

void TranscriberWindow::closeEvent(QCloseEvent *e)
{

}

void TranscriberWindow::addTab()
{
    int index = ui->SettingTabs->count();
    QString num;
    num.setNum(index);

    ui->SettingTabs->addTab(new TranscribeWidget(this, index), tr("Opt. ") + num);
}

void TranscriberWindow::removeTab()
{
    int index = ui->SettingTabs->currentIndex();
    // Base tab cannot be removed.
    if(index == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("You can not remove base tab."), QMessageBox::Ok);
        return;
    }

    //
    int val = QMessageBox::warning(this, tr("Confirm"), tr("Do you really want to remove current tab.\nYou can not undo this action."), QMessageBox::Yes, QMessageBox::No);
    if(val != QMessageBox::Yes)
    {
        return;
    }
    QWidget *p = ui->SettingTabs->widget(index);
    ui->SettingTabs->removeTab(index);
    delete p;
}
