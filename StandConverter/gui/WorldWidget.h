#ifndef WORLDWIDGET_H
#define WORLDWIDGET_H

#include <QWidget>

namespace stand
{
namespace math
{
namespace world
{
class World;
}
}

namespace gui
{

class WorldWidget : public QWidget
{
    Q_OBJECT
signals:

public slots:


public:
    explicit WorldWidget(const QString &filename, QWidget *parent = 0);
    virtual ~WorldWidget();

    bool read(const QString &filename);

protected:
    void paintEvent(QPaintEvent *e);

private:
    void _destroy();
    double *vol;
    stand::math::world::World *w;
};

}
}

#endif // WORLDWIDGET_H
