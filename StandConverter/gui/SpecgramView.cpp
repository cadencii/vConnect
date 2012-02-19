#include "SpecgramView.h"

#include <QPainter>
#include <QPaintEvent>

#include <float.h>
#include <math.h>

#include "../math/SpecgramSet.h"
#include "../io/SpecgramImage.h"

using namespace stand::gui;

SpecgramView::SpecgramView(const SpecgramView::Setting &setting, QWidget *parent) :
    QWidget(parent)
{
    setSpecgram(setting);
}

SpecgramView::SpecgramView(stand::math::SpecgramSet *specgram, double frequencyFloor, double frequencyCeil, ScaleType type, QWidget *parent) :
    QWidget(parent)
{
    setSpecgram(specgram, frequencyFloor, frequencyCeil, type);
}

SpecgramView::SpecgramView(QWidget *parent) :
    QWidget(parent)
{
}

SpecgramView::~SpecgramView()
{
}

void SpecgramView::setSpecgram(const SpecgramView::Setting &setting)
{
    createImage(setting);
    setMinimumSize(_image.width(), _image.height());
    update();
}

void SpecgramView::createImage(const SpecgramView::Setting &setting)
{
    _setting = setting;
    if(_setting.specgram)
    {
        // get the maximum value of specgram.
        _maximumValue = -DBL_MAX;
        for(int t = 0; t < _setting.specgram->tLen(); t++)
        {
            double *spectrum = _setting.specgram->spectrumAt(t);
            for(int i = 0; i < _setting.specgram->fftl(); i++)
            {
                if(_maximumValue < spectrum[i])
                {
                    _maximumValue = spectrum[i];
                }
            }
        }
        if(_setting.frequencyFloor <= 0)
        {
            _setting.frequencyFloor = 0.0;
        }
        if(_setting.frequencyCeil > _setting.specgram->samplingFrequency() / 2)
        {
            _setting.frequencyCeil = _setting.specgram->samplingFrequency() / 2;
        }
        if(_setting.frequencyFloor >= _setting.frequencyCeil)
        {
            _setting.frequencyFloor = _setting.frequencyCeil / 2;
        }
        _image = stand::io::SpecgramImage::imageFromSpecgram(_setting.specgram->specgram(), _setting.specgram->tLen(), _setting.specgram->fftl() / 2);
    }
}

void SpecgramView::setSpecgram(stand::math::SpecgramSet *specgram, double frequencyFloor, double frequencyCeil, ScaleType type)
{
    Setting s = {specgram, frequencyCeil, frequencyFloor, type};
    setSpecgram(s);
}

void SpecgramView::paintEvent(QPaintEvent *e)
{
    paint(e);
}

void SpecgramView::paint(QPaintEvent *e)
{
    if(_setting.specgram == NULL)
    {
        qDebug("SpecgramView::paint(); // Specgram is not set.");
        return;
    }
    if(_setting.scaleType == Unknown)
    {
        qWarning("SpecgramView::paint(); // Unknown paint type");
    }

    QPainter painter(this);
    int x = e->rect().left() / (double)width() * _image.width();
    int w = e->rect().width() / (double)width() * _image.width() + 1;
    QImage im;
    int prevFr = -1;
    for(int j = 0, y = e->rect().top(); j < e->rect().height(); j++, y++)
    {
        double frequency;
        switch(_setting.scaleType)
        {
        case Linear:
            frequency = _setting.frequencyCeil - (_setting.frequencyCeil - _setting.frequencyFloor) * (double)y / height();
            break;
        case Log:
            frequency = frequencyByPosition(y);
            break;
        }

        int fr = frequency / _setting.specgram->samplingFrequency() * _setting.specgram->fftl();
        if(fr <= 1)
        {
            fr = 1;
        }
        else if(fr > _image.height())
        {
            fr = _image.height();
        }

        if(fr != prevFr)
        {
            im = _image.copy(x, _image.height() - fr, w, 1);
        }
        painter.drawImage(QRect(e->rect().left(), y, e->rect().width(), 1), im);
        prevFr = fr;
    }
}

double SpecgramView::frequencyByPosition(int y)
{
    double frequency;
    switch(_setting.scaleType)
    {
    case Linear:
        frequency = _setting.frequencyCeil - (_setting.frequencyCeil - _setting.frequencyFloor) * (double)y / height();
        break;
    case Log:
        frequency = log(_setting.frequencyCeil) - (log(_setting.frequencyCeil) -log(_setting.frequencyFloor)) * (double)y / height();
        frequency = exp(frequency);
        break;
    }

    return frequency;
}

int SpecgramView::positionByFrequency(double freq)
{
    if(freq < _setting.frequencyFloor)
    {
        return height();
    }
    if(freq > _setting.frequencyCeil)
    {
        return -1;
    }
    double dPos;
    switch(_setting.scaleType)
    {
    case Linear:
        dPos = (_setting.frequencyCeil - freq) / (_setting.frequencyCeil - _setting.frequencyFloor);
        break;
    case Log:
        dPos = (log(_setting.frequencyCeil) - log(freq)) / (log(_setting.frequencyCeil) - log(_setting.frequencyFloor));
        break;
    }
    int position = height() * dPos;
    return position;
}

