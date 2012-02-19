#include "WorldWindow.h"
#include "ui_WorldWindow.h"

#include "math/World.h"
#include "math/SpecgramSet.h"
#include "math/LPCSet.h"
#include "../io/WaveFile.h"
#include "../io/audio/AudioMixer.h"
#include "../io/audio/RawWaveTrack.h"

#include "editor/SpecgramPianoroll.h"
#include "../utility/F0Contour.h"

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

    SpecgramView::Setting s = {NULL, 0, 0, SpecgramView::Linear};
    ui->ViewerArea->setWidget(new editor::SpecgramPianoroll(s, 1920, 14, this));

    _f.setByteOrder(QAudioFormat::LittleEndian);
    _f.setChannels(2);
    _f.setCodec("audio/pcm");
    _f.setSampleRate(44100 /4);
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
        changeSpectrumType(true);
        editor::SpecgramPianoroll *w = dynamic_cast<editor::SpecgramPianoroll *>(ui->ViewerArea->widget());
        if(w)
        {
            w->addF0Contour(new stand::utility::F0Contour(_w->f0(), _w->tLen(), _w->framePeriod()), true);
        }
    }
}

void WorldWindow::changeSpectrumType(bool b)
{
    if(!b)
    {
        return;
    }
    stand::math::SpecgramSet *s = NULL;
    SpecgramView *w = dynamic_cast<SpecgramView *>(ui->ViewerArea->widget());
    if(ui->TypeFFT->isChecked())
    {
        s = _s;
    }
    else if(ui->TypeLPC->isChecked())
    {
        s = _lpc;
    }
    else if(ui->TypeStar->isChecked())
    {
        s = _w;
    }
    if(w)
    {
        w->setSpecgram(s, 100, 22050, SpecgramView::Log);
        w->update();
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


