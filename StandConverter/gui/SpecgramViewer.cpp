#include "SpecgramViewer.h"

#include <QImage>

using namespace stand::gui;

SpecgramViewer::SpecgramViewer(QWidget *parent) :
    QWidget(parent)
{
    _image = NULL;
}

SpecgramViewer::~SpecgramViewer()
{
    _destroy();
}

void SpecgramViewer::_destroy()
{
    delete _image;
    _image = NULL;
}

void SpecgramViewer::setSpecgram(double **specgram, int tLen, int fftl)
{
    if(!specgram)
    {
        qDebug("SpecgramViewer::setSpecgram(%d, %d %d); // Invalid args", specgram, tLen, fftl);
        return;
    }

    _destroy();
    _image = new QImage(tLen, fftl, QImage::Format_ARGB32);

    // 最小の値と最大の値を見つけてくる．
//    for(int x = 0; x)
}
