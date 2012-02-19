#include "StandEditor.h"
#include "ui_StandEditor.h"

using namespace stand::gui::editor;

StandEditor::StandEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StandEditor)
{
    ui->setupUi(this);
}

StandEditor::~StandEditor()
{
    delete ui;
}
