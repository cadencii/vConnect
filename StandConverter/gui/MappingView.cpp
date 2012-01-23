#include "MappingView.h"
#include <QColor>
#include <QPaintEvent>
#include <QPainter>

using namespace stand::gui;

const int MappingView::_MAPPING_SIZE = 128;

MappingView::MappingView(QWidget *parent) :
    QWidget(parent)
{
/*    _map = new *QColor[_MAPPING_SIZE];
    _map[0] = new QColor[_MAPPING_SIZE * _MAPPING_SIZE];
    for(int i = 1; i < _MAPPING_SIZE; i++)
    {
        _map[1] = _map[0] + i * _MAPPING_SIZE;
    }*/
    _map = NULL;
}

MappingView::~MappingView()
{
    if(_map)
    {
        delete[] _map[0];
    }
    delete _map;
}

void MappingView::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    int xBegin = (double)(e->rect().x() - 1) / width() * 128;
    int xEnd = (double)(e->rect().x() + e->rect().width() + 1) / width() * 128;
    for(int i = xBegin; i < xEnd; i++)
    {
        int x = i / (double)_MAPPING_SIZE * width();
        int w = (i + 1) / (double)_MAPPING_SIZE * width() - x;

        int yBegin = (double)(e->rect().y() - 1) / height() * 128;
        int yEnd = (double)(e->rect().y() + e->rect().height() + 1) / height() * 128;
        for(int j = yBegin; j < yEnd; j++)
        {
            int y = j / (double)_MAPPING_SIZE * height();
            int h = (j + 1) / (double)_MAPPING_SIZE * height() - y;

            p.fillRect(x, y, w, h, QColor(0, 0, 0));
            p.setPen(QColor(192, 192, 192));
            p.drawLine(x, y, x, y + h);
            p.drawLine(x, y, x + w, y);
        }
    }
}
