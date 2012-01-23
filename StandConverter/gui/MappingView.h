#ifndef MAPPINGVIEW_H
#define MAPPINGVIEW_H

#include <QWidget>

class QColor;

namespace stand
{
namespace gui
{

class MappingView : public QWidget
{
    Q_OBJECT
public:
    explicit MappingView(QWidget *parent = 0);
    ~MappingView();

    void paintEvent(QPaintEvent *e);

signals:

public slots:

private:
    QColor **_map;
    const static int _MAPPING_SIZE;

};

}
}

#endif // MAPPINGVIEW_H
