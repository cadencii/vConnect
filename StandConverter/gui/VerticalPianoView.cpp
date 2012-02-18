#include "VerticalPianoView.h"

#include <QPainter>
#include <QPaintEvent>

using namespace stand::gui;

VerticalPianoView::VerticalPianoView(QWidget *parent) :
    HorizontalPianoView(parent)
{
    setMinimumSize(_MINIMUM_HEIGHT, _MINIMUM_WIDTH);
}

VerticalPianoView::~VerticalPianoView()
{
}

void VerticalPianoView::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    int begin = (double)(e->rect().y() - 1) / height() * 128;
    int end = (double)(e->rect().y() + e->rect().height() + 1) / height() * 128;
    p.fillRect(e->rect(), Qt::white);
    for(int i = begin; i < end; i++)
    {
        int y = i / 128.0 * height();
        int h = (i + 1) / 128.0 * height() - y;
        if(_IS_BLACK[(127 - i)%12])
        {
            p.fillRect(0, y, width() * 2 / 3, h, QColor(128, 128, 128));
        }
        if((127 - i) == 60)
        {
            p.fillRect(0, y, width(), h, Qt::red);
        }
        p.setPen(QColor(192, 192, 192));
        p.drawLine(0, y, width(), y);
    }
}
