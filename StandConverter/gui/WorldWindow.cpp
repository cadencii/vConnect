#include "WorldWindow.h"
#include "ui_WorldWindow.h"

#include "math/World.h"
#include "math/SpecgramSet.h"
#include "math/LPCSet.h"
#include "../io/WaveFile.h"
#include "../io/audio/AudioMixer.h"
#include "../io/audio/RawWaveTrack.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>

using namespace stand::gui;

WorldWindow::WorldWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WorldWindow)
{
    ui->setupUi(this);
    _s = NULL;
    _w = NULL;
    _wave = NULL;

    ui->ViewerArea->setWidget(new QLabel(""));

    _f.setByteOrder(QAudioFormat::LittleEndian);
    _f.setChannels(2);
    _f.setCodec("audio/pcm");
    _f.setSampleRate(44100);
    _f.setSampleSize(16);
    _f.setSampleType(QAudioFormat::SignedInt);

    _mixer = new stand::io::audio::AudioMixer(_f, this);
}

WorldWindow::~WorldWindow()
{
    delete ui;
    delete _mixer;
    _destroy();
}

void WorldWindow::_destroy()
{
    delete _s;
    delete _w;
    delete _wave;
    _s = NULL;
    _w = NULL;
    _wave = NULL;
}

void WorldWindow::beginAnalyze()
{
    QFileInfo fileInfo(ui->FileNameEdit->text());
    if(!fileInfo.exists())
    {
        QMessageBox::critical(this, tr("Error"), tr("Could not find file: ") + ui->FileNameEdit->text());
        return;
    }
    _destroy();
    _wave = new stand::io::WaveFile(fileInfo.absoluteFilePath().toLocal8Bit().data());
    _s = new stand::math::SpecgramSet();
    _lpc = new stand::math::LPCSet();
    _w = new stand::math::world::WorldSet();
    // 読み込めていたら．
    if(_s && _w)
    {
        _s->compute(_wave->data(), _wave->length());
        _lpc->compute(_wave->data(), _wave->length());
        _w->compute(_wave->data(), _wave->length());
        _imagePowerSpec.set(_s->specgram(), _s->tLen(), _s->fftl() / 2 + 1, _s->samplingFrequency() / 2);
        _imageLPCSpec.set(_lpc->specgram(), _lpc->tLen(), _lpc->fftl() / 2 + 1, _lpc->samplingFrequency() / 2);
        _imageStarSpec.set(_w->specgram(), _w->tLen(), _w->fftl() / 2 + 1, _w->samplingFrequency() / 2);
        changeSpectrumType(true);
    }
}

void WorldWindow::changeSpectrumType(bool b)
{
    if(!b)
    {
        return;
    }
    QImage image;
    if(ui->TypeFFT->isChecked())
    {
        image = _imagePowerSpec.image();
    }
    else if(ui->TypeLPC->isChecked())
    {
        image = _imageLPCSpec.image();
    }
    else if(ui->TypeStar->isChecked())
    {
        image = _imageStarSpec.image();
    }
    QLabel *l = dynamic_cast<QLabel *>(ui->ViewerArea->widget());
    if(l)
    {
        l->setPixmap(QPixmap::fromImage(image));
    }
}

void WorldWindow::searchDirectory()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Wave File"), "", tr("WaveFile (*.wav)"));
    ui->FileNameEdit->setText(fileName);
}

void WorldWindow::playSound()
{
    if(!_w)
    {
        return;
    }
    if(ui->pushButton->text() == "Play")
    {
        stand::io::audio::RawWaveTrack *t = new stand::io::audio::RawWaveTrack(_wave, _f, this);
        _mixer->addTrack(t, 1, 0);
        _mixer->start();
        ui->pushButton->setText("Stop");
    }
    else
    {
        _mixer->stop();
        ui->pushButton->setText("Play");
    }
}


