#include "HorizontalPianoView.h"

#include <QPaintEvent>
#include <QPainter>
#include <QColor>

using namespace stand::gui;

const int HorizontalPianoView::_MINIMUM_HEIGHT = 32;
const int HorizontalPianoView::_MINIMUM_WIDTH = 256;
const bool HorizontalPianoView::_IS_BLACK[12] =
{
    false, // C
    true,  // C+
    false, // D
    true,  // D+
    false, // E
    false, // F
    true,  // F+
    false, // G
    true,  // G+
    false, // A
    true,  // B-
    false, // B
};

HorizontalPianoView::HorizontalPianoView(QWidget *parent) :
    QWidget(parent)
{
    setMinimumSize(_MINIMUM_WIDTH, _MINIMUM_HEIGHT);
}

void HorizontalPianoView::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    int begin = (double)(e->rect().x() - 1) / width() * 128;
    int end = (double)(e->rect().x() + e->rect().width() + 1) / width() * 128;
    for(int i = begin; i < end; i++)
    {
        int x = i / 128.0 * width();
        int w = (i + 1) / 128.0 * width() - x;
        p.fillRect(x, 0, w, height(), Qt::white);
        if(_IS_BLACK[i%12])
        {
            p.fillRect(x, 0, w, height() * 2 / 3, QColor(128, 128, 128));
        }
        if(i == 60)
        {
            p.fillRect(x, 0, w, height(), Qt::red);
        }
        p.setPen(QColor(192, 192, 192));
        p.drawLine(x, 0, x, height());
    }
}
