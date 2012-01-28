#include "TranscriberWindow.h"
#include "ui_TranscriberWindow.h"

#include "TranscriberWidget.h"
#include "../synthesis/Transcriber.h"
#include "../synthesis/TranscriberSetting.h"
#include "../io/UtauLibrary.h"
#include "../utility/Utility.h"
#include "ConverterWindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>

using namespace stand::gui;

TranscriberWindow::TranscriberWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TranscriberWindow)
{
    ui->setupUi(this);

    // TabWidget の中身を空にする．
    // 今のとこ必要なんだけど， ui の設定でどうにかならないのかな？
    while(ui->SettingTabs->count() != 0)
    {
        QWidget *p = ui->SettingTabs->widget(0);
        ui->SettingTabs->removeTab(0);
        delete p;
    }
    TranscriberWidget *w = new TranscriberWidget(ui->SettingTabs);
    ui->SettingTabs->addTab(w, tr("Lib. 1"));

    connect(w, SIGNAL(changed(QWidget*)), this, SLOT(settingChanged()));
    connect(this, SIGNAL(mappingChanged(QVector<stand::gui::MappingView::Item>&)), ui->MappingView, SLOT(setMapping(QVector<stand::gui::MappingView::Item>&)));

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
    QVector<stand::gui::MappingView::Item> mapping;
    stand::gui::MappingView::Item m;
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
    num.setNum(index + 1);

    TranscriberWidget *w = new TranscriberWidget(ui->SettingTabs, index);
    ui->SettingTabs->addTab(w, tr("Lib. ") + num);
    connect(w, SIGNAL(changed(QWidget*)), this, SLOT(settingChanged()));
    connect(this, SIGNAL(mappingChanged(QVector<stand::gui::MappingView::Item>&)), ui->MappingView, SLOT(setMapping(QVector<stand::gui::MappingView::Item>&)));
    settingChanged();
}

void TranscriberWindow::removeTab()
{
    int index = ui->SettingTabs->currentIndex();
    // 少なくとも一つのタブは必要である．
    if(ui->SettingTabs->count() == 1)
    {
        QMessageBox::critical(this, tr("Error"), tr("You need at least one lib."), QMessageBox::Ok);
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
    ui->RootDirectory->setEnabled(!isAnalyzing);
    ui->RootSelector->setEnabled(!isAnalyzing);
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

void TranscriberWindow::pushRootDir()
{
    QString dirName = QFileDialog::getExistingDirectory(
                this,
                tr("Select connect root directory."),
                tr("")
                );
    ui->RootDirectory->setText(dirName);
}

void TranscriberWindow::_beginAnalyze()
{
    stand::synthesis::TranscriberSetting s;
    // Directory の存在チェック．なければ作成するなどの処理．
    if(!stand::utility::makeDirectory(this, ui->RootDirectory->text(), true))
    {
        return;
    }
    // 設定がおかしい．
    if(!_checkSettingAvailability())
    {
        return;
    }
    // コンバータが使える設定に変換する．できなければ帰る．
    if(!_createSetting(s))
    {
        QMessageBox::critical(this, tr("Error"), tr("Some settings are invalid."));
        return;
    }
    ui->ProgressBar->setMaximum(s.libraries.at(0).body->size());
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

    // 指定されたライブラリを全部読み込む．
    for(int i = 0; i < ui->SettingTabs->count(); i++)
    {
        stand::synthesis::Transcriber::TranscriberItem item;
        TranscriberWidget *w = dynamic_cast<TranscriberWidget *>(ui->SettingTabs->widget(i));
        QString filename = w->dir() + QDir::separator() + "oto.ini";
        stand::io::UtauLibrary *lib = new stand::io::UtauLibrary();
        if(lib->readFromOtoIni(filename, w->codec()) && lib->size() != 0)
        {
            item.body = lib;
            item.brightness = w->bri();
            item.note = w->note();
            s.libraries.push_back(item);
            const stand::io::UtauPhoneme *p = lib->at(0);
            // 波形データの場合，とりあえずユーザに通知しておしまいにする．
            //  ConverterDialogにしておけばよかったか…
            if(p->filename.contains(".wav"))
            {
                QMessageBox::critical(this, tr("Error"), tr("You must convert the library below.\n") + lib->directory().absolutePath());
                for(int i = 0; i < s.libraries.size(); i++)
                {
                    delete s.libraries.at(i).body;
                }
                return false;
            }
        }
        else
        {
            // 読み込めなかった．
            QMessageBox::critical(this, tr("Error"), tr("Could not find the library below\n") + filename);
            delete lib;
            for(int i = 0; i < s.libraries.size(); i++)
            {
                delete s.libraries.at(i).body;
            }
            return false;
        }
    }
    // ToDo::そーーーーと！！
    for(int i = 0; i < s.libraries.size(); i++)
    {
        int minimum = s.libraries.at(i).brightness;
        int index = i;
        for(int j = i + 1; j < s.libraries.size(); j++)
        {
            if(minimum < s.libraries.at(j).brightness)
            {
                minimum = s.libraries.at(j).brightness;
                index = j;
            }
        }
        stand::synthesis::Transcriber::TranscriberItem item, tmp;
        item = s.libraries.at(index);
        tmp = s.libraries.at(i);
        s.libraries.replace(i, item);
        s.libraries.replace(index, tmp);
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

bool TranscriberWindow::_checkSettingAvailability()
{
    QDir root(ui->RootDirectory->text());
    for(int i = 0; i < ui->SettingTabs->count(); i++)
    {
        TranscriberWidget *w = dynamic_cast<TranscriberWidget *>(ui->SettingTabs->widget(i));
        QString relative = root.relativeFilePath(w->dir());
        QString absolute = QDir(relative).absolutePath();
        if(relative == absolute)
        {
            QMessageBox::critical(this, tr("Error"), tr("Could not create relative path of the library below;\n") + relative);
            return false;
        }
    }
    return true;
}

