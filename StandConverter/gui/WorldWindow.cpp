#include "WorldWindow.h"
#include "ui_WorldWindow.h"

#include "math/World.h"
#include "math/SpecgramSet.h"
#include "io/WaveFile.h"

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
}

WorldWindow::~WorldWindow()
{
    delete ui;
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
    _w = new stand::math::world::WorldSet();
    // 読み込めていたら．
    if(_s && _w)
    {
        _s->compute(_wave->data(), _wave->length());
        _w->compute(_wave->data(), _wave->length());
        _imagePowerSpec.set(_s->specgram(), _s->tLen(), _s->fftl() / 2 + 1, _s->samplingFrequency() / 2);
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


