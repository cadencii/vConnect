#include "TranscriberWindow.h"
#include "ui_TranscriberWindow.h"

#include "TranscriberWidget.h"
#include "../synthesis/Transcriber.h"
#include "../synthesis/TranscriberSetting.h"
#include "../io/UtauLibrary.h"

#include <QMessageBox>
#include <QCloseEvent>

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
    ui->SettingTabs->addTab(new TranscriberWidget(this), tr("Base"));

    isAnalyzing = false;
    current = NULL;
    _setItemEnabled();
}

TranscriberWindow::~TranscriberWindow()
{
    delete ui;
}

void TranscriberWindow::closeEvent(QCloseEvent *e)
{
    QMessageBox::StandardButton val = QMessageBox::Yes;

    // 分析中ならユーザにお伺いを立てる．
    if(isAnalyzing)
    {
        val = (QMessageBox::StandardButton)(QMessageBox::warning(
                    this,
                    tr("Confirmation"),
                    tr("Transcriber is still running. Do you want to exit anyway?"),
                    QMessageBox::Yes,
                    QMessageBox::No));
    }

    // ユーザの希望の処理をする．
    if(val == QMessageBox::Yes)
    {
        e->accept();
    }
    else
    {
        e->ignore();
    }
}

void TranscriberWindow::addTab()
{
    int index = ui->SettingTabs->count();
    QString num;
    num.setNum(index);

    ui->SettingTabs->addTab(new TranscriberWidget(this, index), tr("Opt. ") + num);
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

void TranscriberWindow::_setItemEnabled()
{
    ui->AnalyzeButton->setText((isAnalyzing)?tr("Cancel"):tr("Analyze"));

    ui->AddButton->setEnabled(!isAnalyzing);
    ui->MappingView->setEnabled(!isAnalyzing);
    ui->NumThreads->setEnabled(!isAnalyzing);
    if(!isAnalyzing)
    {
        ui->ProgressBar->setValue(0);
    }
    ui->ProgressBar->setEnabled(isAnalyzing);
    ui->RemoveButton->setEnabled(!isAnalyzing);
    ui->SettingTabs->setEnabled(!isAnalyzing);
}

void TranscriberWindow::pushAnalyze()
{
    if(!isAnalyzing && !current)
    {
        _beginAnalyze();
    }
    else
    {
        _cancelAnalyze();
    }
}

void TranscriberWindow::_beginAnalyze()
{
    stand::synthesis::TranscriberSetting s;
    // ToDo::Create Setting

    if(!_createSetting(s))
    {
        QMessageBox::critical(this, tr("Error"), tr("Some settings are invalid."));
        return;
    }
    current = new stand::synthesis::Transcriber(s, this);
    connect(this, SIGNAL(sendCancelToTranscriber()), current, SLOT(cancel()));
    connect(current, SIGNAL(finish(bool)), SLOT(transcriptionFinished(bool)));

    current->start();
    isAnalyzing = true;
    _setItemEnabled();
}

void TranscriberWindow::_cancelAnalyze()
{
    QMessageBox::StandardButton ret;
    ret = (QMessageBox::StandardButton)(QMessageBox::warning(
                                            this,
                                            tr("Confirm"),
                                            tr("Do you really want to cancel transcription?"),
                                            QMessageBox::Yes,
                                            QMessageBox::No));
    if(ret != QMessageBox::Yes)
    {
        return;
    }
    emit sendCancelToTranscriber();
}

bool TranscriberWindow::_createSetting(stand::synthesis::TranscriberSetting &s)
{
    s.numThreads = ui->NumThreads->value();
    s.base = new stand::io::UtauLibrary();

    TranscriberWidget *w = dynamic_cast<TranscriberWidget *>(ui->SettingTabs->widget(0));
    QString filename = w->dir() + QDir::separator() + "oto.ini";
    if(!s.base->readFromOtoIni(filename, w->codec()))
    {
        QMessageBox::critical(this, tr("Error"), tr("Base file path is invalid\n") + filename);
        delete s.base;
        return false;
    }
    s.optionals.clear();

    for(int i = 0; i < ui->SettingTabs->count() - 1; i++)
    {
        w = dynamic_cast<TranscriberWidget *>(ui->SettingTabs->widget(i + 1));
        QString filename = w->dir() + QDir::separator() + "oto.ini";
        stand::io::UtauLibrary *lib = new stand::io::UtauLibrary();
        if(lib->readFromOtoIni(filename, w->codec()))
        {
            s.optionals.push_back(lib);
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid file path\n") + filename);
            delete lib;
            delete s.base;
            for(int i = 0; i < s.optionals.size(); i++)
            {
                delete s.optionals.at(i);
            }
            return false;
        }
    }
    // ToDo::ばぶるそーーーーと！！

    return true;
}

void TranscriberWindow::transcriptionFinished(bool)
{
    current->disconnect();
    current->wait();
    delete current;
    current = NULL;
    isAnalyzing = false;
    _setItemEnabled();
}
