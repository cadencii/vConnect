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
    struct Item
    {
        int note;
        int brightness;
        QColor color;
    };

    explicit MappingView(QWidget *parent = 0);
    ~MappingView();

    void paintEvent(QPaintEvent *e);

public slots:
    void setMapping(QVector<stand::gui::MappingView::Item> &mapping);

signals:

public slots:

private:
    struct _color
    {
        int r, g, b;
    };

    _color **_map;
    QVector<Item> _data;
    const static int _MAPPING_SIZE;
};

}
}

#endif // MAPPINGVIEW_H
