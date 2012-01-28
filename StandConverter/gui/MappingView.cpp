#include "MappingView.h"
#include <QColor>
#include <QPaintEvent>
#include <QPainter>

#include <math.h>

using namespace stand::gui;

const int MappingView::_MAPPING_SIZE = 128;

// min, max macro
#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef max
#define max(a,b) (((a)<(b))?(b):(a))
#endif

MappingView::MappingView(QWidget *parent) :
    QWidget(parent)
{
    _map = new _color*[_MAPPING_SIZE];
    _map[0] = new _color[_MAPPING_SIZE * _MAPPING_SIZE];
    for(int i = 1; i < _MAPPING_SIZE; i++)
    {
        _map[i] = _map[0] + i * _MAPPING_SIZE;
    }
    for(int i = 0; i < _MAPPING_SIZE * _MAPPING_SIZE; i++)
    {
        _map[0][i].r = _map[0][i].b = _map[0][i].g = 0;
    }
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

            p.fillRect(x, y, w, h, QColor(_map[i][j].r, _map[i][j].g, _map[i][j].b));
            p.setPen(QColor(192, 192, 192));
            p.drawLine(x, y, x, y + h);
            p.drawLine(x, y, x + w, y);
        }
    }
    for(int i = 0; i < _data.size(); i++)
    {
        int x = _data.at(i).note / (double)_MAPPING_SIZE * width();
        int w = (1 + _data.at(i).note) / (double)_MAPPING_SIZE * width() - x;
        int y = (_MAPPING_SIZE - 1 - _data.at(i).brightness) / (double)_MAPPING_SIZE * height();
        int h = (1 + _MAPPING_SIZE - 1 - _data.at(i).brightness) / (double)_MAPPING_SIZE * height() - y;
        p.setPen(Qt::white);
        p.drawLine(x, y, x, y + h);
        p.drawLine(x, y, x + w, y);
        p.drawLine(x, y + h, x + w, y + h);
        p.drawLine(x + w, y, x + w, y + h);
    }
}

void MappingView::setMapping(QVector<stand::gui::MappingView::Item> &mapping)
{
    _data = mapping;
    unsigned int i;
    for(i = 0; i < _data.size(); i++)
    {
        Item m = _data.at(i), tmp;
        int index = i;
        for(int j = i + 1; j < _data.size(); j++)
        {
            if(m.brightness > _data.at(j).brightness)
            {
                m = _data.at(j);
                index = j;
            }
        }
        tmp = _data.at(i);
        _data.replace(i, m);
        _data.replace(index, tmp);
    }

    int r = _data.at(0).color.red();
    int g = _data.at(0).color.green();
    int b = _data.at(0).color.blue();
    for(i = 0; i <= _data.at(0).brightness; i++)
    {
        int y = 127 - i;

        for(int j = 0; j < _MAPPING_SIZE; j++)
        {
            _map[j][y].r = r;
            _map[j][y].g = g;
            _map[j][y].b = b;
        }
    }
    for(int index = 1; index < _data.size(); index++)
    {
        int pbri = _data.at(index - 1).brightness;
        int bri = _data.at(index).brightness;
        for(; i <= (bri + pbri) / 2 && i < _MAPPING_SIZE; i++)
        {
            int y = 127 - i;
            int c = (1 + i - pbri) * 2;
            int d = bri - pbri;
            int cr = r + _data.at(index).color.red() * log((double)c) / log((double)d);
            int cg = g + _data.at(index).color.green() * log((double)c) / log((double)d);
            int cb = b + _data.at(index).color.blue() * log((double)c) / log((double)d);
            for(int j = 0; j < _MAPPING_SIZE; j++)
            {
                _map[j][y].r = min(255, cr);
                _map[j][y].g = min(255, cg);
                _map[j][y].b = min(255, cb);
            }
        }
        for(; i < bri && i < _MAPPING_SIZE; i++)
        {
            int y = 127 - i;
            int c = (bri - i) * 2;
            int d = bri - pbri;
            int cr = r * log((double)c) / log((double)d) + _data.at(index).color.red();
            int cg = g * log((double)c) / log((double)d) + _data.at(index).color.green();
            int cb = b * log((double)c) / log((double)d) + _data.at(index).color.blue();
            for(int j = 0; j < _MAPPING_SIZE; j++)
            {
                _map[j][y].r = min(255, cr);
                _map[j][y].g = min(255, cg);
                _map[j][y].b = min(255, cb);
            }
        }
        r = _data.at(index).color.red();
        g = _data.at(index).color.green();
        b = _data.at(index).color.blue();
    }
    for(; i < _MAPPING_SIZE; i++)
    {
        int y = 127 - i;
        for(int j = 0; j < _MAPPING_SIZE; j++)
        {
            _map[j][y].r = _data.last().color.red();
            _map[j][y].g = _data.last().color.green();
            _map[j][y].b = _data.last().color.blue();
        }
    }

    for(int i = 0; i < _data.size(); i++)
    {
        int x = mapping.at(i).note;
        int y = 127 - _data.at(i).brightness;
        _map[x][y].r = _data.at(i).color.red();
        _map[x][y].g = _data.at(i).color.green();
        _map[x][y].b = _data.at(i).color.blue();
    }
    update();
}
