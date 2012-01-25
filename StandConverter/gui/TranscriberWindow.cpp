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
    TranscriberWidget *w = new TranscriberWidget(ui->SettingTabs);
    ui->SettingTabs->addTab(w, tr("Base"));

    connect(w, SIGNAL(changed(QWidget*)), this, SLOT(settingChanged()));
    connect(this, SIGNAL(mappingChanged(QVector<stand::gui::MappingView::Map>&)), ui->MappingView, SLOT(setMapping(QVector<stand::gui::MappingView::Map>&)));

    isAnalyzing = false;
    current = NULL;
    settingChanged();
    _setItemEnabled();
}

TranscriberWindow::~TranscriberWindow()
{
    disconnect();
    delete ui;
}

void TranscriberWindow::settingChanged()
{
    QVector<stand::gui::MappingView::Map> mapping;
    stand::gui::MappingView::Map m;
    for(int i = 0; i < ui->SettingTabs->count(); i++)
    {
        TranscriberWidget *w = dynamic_cast<TranscriberWidget *>(ui->SettingTabs->widget(i));
        m.brightness = w->bri();
        m.note = w->note();
        m.color = QColor(w->colorName());
        mapping.push_back(m);
    }
    emit mappingChanged(mapping);
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

    TranscriberWidget *w = new TranscriberWidget(ui->SettingTabs, index);
    ui->SettingTabs->addTab(w, tr("Opt. ") + num);
    connect(w, SIGNAL(changed(QWidget*)), this, SLOT(settingChanged()));
    connect(this, SIGNAL(mappingChanged(QVector<stand::gui::MappingView::Map>&)), ui->MappingView, SLOT(setMapping(QVector<stand::gui::MappingView::Map>&)));
    settingChanged();
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
    settingChanged();
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
    ui->ProgressBar->setMaximum(s.base.lib->size());
    current = new stand::synthesis::Transcriber(&s, this);
    connect(this, SIGNAL(sendCancelToTranscriber()), current, SLOT(cancel()));
    connect(current, SIGNAL(progressChanged(int)), ui->ProgressBar, SLOT(setValue(int)));
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
    s.base.lib = new stand::io::UtauLibrary();

    TranscriberWidget *w = dynamic_cast<TranscriberWidget *>(ui->SettingTabs->widget(0));
    QString filename = w->dir() + QDir::separator() + "oto.ini";
    if(!s.base.lib->readFromOtoIni(filename, w->codec()))
    {
        QMessageBox::critical(this, tr("Error"), tr("Base file path is invalid\n") + filename);
        delete s.base.lib;
        return false;
    }
    s.base.brightness = w->bri();
    s.base.note = w->note();

    for(int i = 0; i < ui->SettingTabs->count() - 1; i++)
    {
        stand::synthesis::Transcriber::TranscriberItem item;
        w = dynamic_cast<TranscriberWidget *>(ui->SettingTabs->widget(i + 1));
        QString filename = w->dir() + QDir::separator() + "oto.ini";
        stand::io::UtauLibrary *lib = new stand::io::UtauLibrary();
        if(lib->readFromOtoIni(filename, w->codec()))
        {
            item.lib = lib;
            item.brightness = w->bri();
            item.note = w->note();
            s.optionals.push_back(item);
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), tr("Invalid file path\n") + filename);
            delete lib;
            delete s.base.lib;
            for(int i = 0; i < s.optionals.size(); i++)
            {
                delete s.optionals.at(i).lib;
            }
            return false;
        }
    }
    // ToDo::そーーーーと！！
    for(int i = 0; i < s.optionals.size(); i++)
    {
        int minimum = s.optionals.at(i).brightness;
        int index = i;
        for(int j = i + 1; j < s.optionals.size(); j++)
        {
            if(minimum < s.optionals.at(j).brightness)
            {
                minimum = s.optionals.at(j).brightness;
                index = j;
            }
        }
        stand::synthesis::Transcriber::TranscriberItem item, tmp;
        item = s.optionals.at(index);
        tmp = s.optionals.at(i);
        s.optionals.replace(i, item);
        s.optionals.replace(index, tmp);
    }

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
