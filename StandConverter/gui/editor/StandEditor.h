#ifndef STANDEDITOR_H
#define STANDEDITOR_H

#include <QMainWindow>

namespace Ui {
class StandEditor;
}

namespace stand
{
namespace gui
{
namespace editor
{

class StandEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit StandEditor(QWidget *parent = 0);
    ~StandEditor();

private:
    Ui::StandEditor *ui;
};

}
}
}

#endif // STANDEDITOR_H
