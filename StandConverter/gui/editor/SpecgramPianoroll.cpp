#include "SpecgramPianoroll.h"

#include "../VerticalPianoView.h"
#include "../../math/SpecgramSet.h"
#include "../../utility/F0Contour.h"

#include <QPainter>
#include <QPaintEvent>

#include <math.h>

using namespace stand::gui::editor;

QColor SpecgramPianoroll::Colors[] =
{
    Qt::red,
    Qt::green,
    Qt::blue,
    Qt::yellow,
    Qt::cyan,
    Qt::magenta,
    Qt::white
};

SpecgramPianoroll::SpecgramPianoroll(const Setting &setting, int width, int noteHeight, QWidget *parent)
    : SpecgramView(setting, parent)
{
    _noteHeight = noteHeight;
    setSpecgram(_setting);
    QWidget *w = new stand::gui::VerticalPianoView(this);
    w->setMinimumSize(64, minimumHeight());
    w->setMaximumSize(64, minimumHeight());
}

SpecgramPianoroll::~SpecgramPianoroll()
{
    for(int i = 0; i < _pitches.size(); i++)
    {
        if(_pitches[i].autodelete)
        {
            delete _pitches[i].p;
        }
    }
}

void SpecgramPianoroll::paintEvent(QPaintEvent *e)
{
    paint(e);

    QPainter painter(this);

    // Draw vertical lines.
    int begin = e->rect().top() / _noteHeight - 1;
    int end = (e->rect().top() + e->rect().height()) / _noteHeight + 1;
    painter.setPen(Qt::gray);
    for(int i = begin; i < end; i++)
    {
        int y = i * _noteHeight;
        painter.drawLine(e->rect().left(), y, e->rect().left() + e->rect().width() + 1, y);
    }
    painter.setPen(Qt::red);

    // Draw pitch contours;
    _paintPitch(&painter, e);
}

void SpecgramPianoroll::_paintPitch(QPainter *painter, QPaintEvent *e)
{
    for(int i = 0; i < _pitches.size(); i++)
    {
        painter->setPen(Colors[i % (sizeof(Colors) / sizeof(QColor))]);
        int prev = positionByFrequency(
                    _pitches[i].p->at(
                        e->rect().left() / (double)width() * _setting.specgram->tLen() * _setting.specgram->framePeriod()
                        )
                    );
        for(int x = e->rect().left() + 1, j = 0; j < e->rect().width(); x++, j++)
        {
            stand::utility::F0Contour *p = _pitches[i].p;
            double ms = (double)x / (double)width() * (double)_setting.specgram->tLen() * _setting.specgram->framePeriod();
            int current = positionByFrequency(p->at(ms));
            if(current < height() && prev < height())
            {
                painter->drawLine(x - 1, prev, x, current);
            }
            prev = current;
        }
    }
}

void SpecgramPianoroll::setSpecgram(const stand::gui::SpecgramView::Setting &setting)
{
    createImage(setting);
    _setting.frequencyCeil = stand::A4Frequency * pow(2, (128 - stand::A4NoteNumber + 0.5) / 12.0);
    _setting.frequencyFloor = stand::A4Frequency * pow(2, (0 - stand::A4NoteNumber + 0.5) / 12.0);
    _setting.scaleType = SpecgramView::Log;
    setMinimumSize(_image.width(), _noteHeight * 128);
}

void SpecgramPianoroll::addF0Contour(stand::utility::F0Contour *f0, bool autodelete)
{
    if(!f0)
    {
        return;
    }
    if(indexOf(f0) != -1)
    {
        qWarning("SpecgramPianoroll::addF0Contour(); // F0 duplication.");
        return;
    }
    PitchItem p = {f0, autodelete};
    _pitches.push_back(p);
}

void SpecgramPianoroll::removeF0Contour(stand::utility::F0Contour *f0)
{
    int index = indexOf(f0);
    if(index != -1)
    {
        if(_pitches[index].autodelete)
        {
            delete _pitches[index].p;
        }
        _pitches.remove(index);
    }
}

int SpecgramPianoroll::indexOf(stand::utility::F0Contour *f0)
{
    int retval = -1;
    for(int i = 0; i < _pitches.size(); i++)
    {
        if(_pitches[i].p == f0)
        {
            retval = i;
        }
    }
    return retval;
}
