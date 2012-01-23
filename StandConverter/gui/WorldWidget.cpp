#include "WorldWidget.h"

#include "../math/World.h"
#include "../io/WaveFile.h"

#include <QDir>

using namespace stand::gui;

WorldWidget::WorldWidget(const QString &filename, QWidget *parent) :
    QWidget(parent)
{
    w = NULL;
    vol = NULL;
    read(filename);
}

WorldWidget::~WorldWidget()
{
    _destroy();
}

void WorldWidget::_destroy()
{
    delete[] vol;
    vol = NULL;
    delete w;
    w = NULL;
}

bool WorldWidget::read(const QString &filename)
{
    stand::io::WaveFile wave;
    if(!wave.read(QDir::toNativeSeparators(filename).toLocal8Bit().data()))
    {
        qDebug("WorldWidget::read(%s); Unable to read file.", filename);
        return false;
    }
}

void WorldWidget::paintEvent(QPaintEvent *e)
{

}
