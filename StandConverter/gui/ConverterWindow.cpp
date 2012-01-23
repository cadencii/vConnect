/*!
 * @file ConverterWindow.cpp
 * @author HAL
 */
#include "ConverterWindow.h"
#include "ui_ConverterWindow.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QTextCodec>

#include "../io/UtauLibrary.h"
#include "../utility/Utility.h"
#include "../synthesis/ConverterSetting.h"
#include "../synthesis/Converter.h"

using namespace stand::gui;

ConverterWindow::ConverterWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConverterWindow)
{
    ui->setupUi(this);
    isAnalyzing = false;
    setEditorEnabled(true);
    currentConverter = NULL;
}

ConverterWindow::~ConverterWindow()
{
    delete ui;
}

////
// 終了時の処理
void ConverterWindow::closeEvent(QCloseEvent *e)
{
    QMessageBox::StandardButton val = QMessageBox::Yes;

    // 分析中ならユーザにお伺いを立てる．
    if(isAnalyzing)
    {
        val = (QMessageBox::StandardButton)(QMessageBox::warning(
                    this,
                    tr("Confirmation"),
                    tr("Converter is analyzing. Do you want to exit anyway?"),
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

void ConverterWindow::setEditorEnabled(bool e)
{
    // Directory Settings
    ui->DestDirectory->setEnabled(e);
    ui->DestFinder->setEnabled(e);
    ui->DestEncodingType->setEnabled(e);
    ui->SourceDirectory->setEnabled(e);
    ui->SourceFinder->setEnabled(e);
    ui->SourceEncodingType->setEnabled(e);
    ui->SourceIsRecursive->setEnabled(e);

    // Compression Settings
    ui->MelcepstrumDim->setEnabled(e);
    ui->VorbisBitrate->setEnabled(e);

    // Normalization Settings
    ui->EnableNormalize->setEnabled(e);
    bool normalizeEnabled = (Qt::Checked == ui->EnableNormalize->checkState());
    ui->PrePronounce->setEnabled(e && normalizeEnabled);
    ui->PhonemeLength->setEnabled(e && normalizeEnabled);

    // Analysis Settings
    ui->ThreadNumber->setEnabled(e);
    ui->AnalysisFramePeriod->setEnabled(false);

    // ProgressBar Setting
    ui->AnalysisProgress->setEnabled(!e);
    if(e)
    {
        ui->AnalysisProgress->setValue(0);
        ui->ConvertButton->setText(tr("Convert"));
    }
    else
    {
        ui->ConvertButton->setText(tr("Cancel"));
    }
}

void ConverterWindow::normalizeEnabledChanged()
{
    setEditorEnabled(!isAnalyzing);
}

void ConverterWindow::sourceFinderClicked()
{
    QString dirName = QFileDialog::getExistingDirectory(
                this,
                tr("Select input directory."),
                tr("")
                );
    ui->SourceDirectory->setText(dirName);
}

void ConverterWindow::destFinderClicked()
{
    QString dirName = QFileDialog::getExistingDirectory(
                this,
                tr("Select output directory."),
                tr("")
                );
    ui->DestDirectory->setText(dirName);
}

void ConverterWindow::convertButtonClicked()
{
    if(isAnalyzing)
    {
        cancelConvert();
    }
    else
    {
        beginConvert();
    }
}

void ConverterWindow::beginConvert()
{
    qDebug("ConverterWindow::beginConvert()");

    // Read UtauLibrary
    stand::synthesis::ConverterSetting setting;
    if(!_setConverterSetting(setting))
    {
        qDebug(" Could not create ConverterSetting.");
        _errorMessage(tr("There is no oto.ini below the directory\n") + ui->SourceDirectory->text());
        return;
    }

    // Not found outDir, make a user sure to mkdir.
    if(!stand::utility::makeDirectory(dynamic_cast<QWidget *>(parent()), setting.outDir, true))
    {
        qDebug(" Error; could not open or create output Directory.");
        _errorMessage(tr("Could not make/open output directory\n"));
        return;
    }

    // Create Converter
    stand::synthesis::Converter *converter;
    currentConverter = converter = new stand::synthesis::Converter(setting, this);
    connect(this, SIGNAL(sendCancelToConverter()), converter, SLOT(cancel()));
    connect(converter, SIGNAL(finish(bool)), this, SLOT(converterFinished(bool)));
    connect(converter, SIGNAL(progressChanged(int)), ui->AnalysisProgress, SLOT(setValue(int)));
    ui->AnalysisProgress->setMaximum(setting.library->size());

    // Start Converter
    converter->start();

    // Disable Editor
    isAnalyzing = true;
    setEditorEnabled(false);
}

bool ConverterWindow::_setConverterSetting(stand::synthesis::ConverterSetting &setting)
{
    qDebug("-ConverterWindow::SetConverterSetting()");
    setting.library = new stand::io::UtauLibrary(
                                        ui->SourceDirectory->text(),
                                        QTextCodec::codecForName(ui->SourceEncodingType->currentText().toLocal8Bit().data()),
                                        (ui->SourceIsRecursive->checkState() == Qt::Checked)? 1 : 0
                                        );
    if(setting.library->size() == 0)
    {
        qDebug("--Can not read UtauLibrary: %s", ui->SourceDirectory->text().toLocal8Bit().data());
        return false;
    }
    setting.outDir = QDir(ui->DestDirectory->text());
    setting.cepstrumLength = ui->MelcepstrumDim->value();
    setting.vorbisBitrate = ui->VorbisBitrate->value();
    setting.normalizeUtauLibrary = (ui->EnableNormalize->checkState() == Qt::Checked);
    setting.preUtterance = ui->PrePronounce->value();
    setting.phonemeLength = ui->PhonemeLength->value();
    setting.numThreads = ui->ThreadNumber->value();

    setting.framePeriod = ui->AnalysisFramePeriod->value();
    setting.outCodec = QTextCodec::codecForName(ui->DestEncodingType->currentText().toLocal8Bit().data());

    bool duplication = false;
    for(unsigned int i = 0; i < setting.library->size(); i++)
    {
        const stand::io::UtauPhoneme *p = setting.library->at(i);
        QString filename;
        filename.setNum(i + 1);
        filename = setting.outDir.absolutePath() + QDir::separator() + filename + ".vvd";
        QFileInfo info(filename);
        duplication |= info.exists();
    }

    setting.overwrite = false;
    if(duplication)
    {
        int ret = QMessageBox::warning(this, tr("Confirm"), tr("Some file already exists in output directory.\nDo you want to overwrite them?"), QMessageBox::Yes, QMessageBox::No);
        setting.overwrite = (ret == QMessageBox::Yes);
    }

    return true;
}

void ConverterWindow::cancelConvert()
{
    QMessageBox::StandardButton ret;
    ret = (QMessageBox::StandardButton)(QMessageBox::warning(
                                            this,
                                            tr("Confirm"),
                                            tr("Do you really want to cancel convertion?"),
                                            QMessageBox::Yes,
                                            QMessageBox::No));
    if(ret == QMessageBox::Yes)
    {
        emit sendCancelToConverter();
    }
}

void ConverterWindow::_errorMessage(const QString &text)
{
    QMessageBox::critical(this, tr("Error"), text);
}

void ConverterWindow::converterFinished(bool f)
{
    isAnalyzing = false;
    setEditorEnabled(true);
    // 変換が失敗したと聞いて．
    if(!f)
    {
        _errorMessage("Error occured in converting.");
    }
    // 処理が終了したので変換器を殺しても大丈夫．
    delete currentConverter;
    currentConverter = NULL;
}
