#include "TranscribeWidget.h"
#include "ui_TranscribeWidget.h"

using namespace stand::gui;

TranscribeWidget::TranscribeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TranscribeWidget)
{
    ui->setupUi(this);
}

TranscribeWidget::~TranscribeWidget()
{
    delete ui;
}

QString TranscribeWidget::dir()
{
    return ui->DirectoryName->text();
}

int TranscribeWidget::bri()
{
    return ui->BirghtnessSpinBox->value();
}

int TranscribeWidget::note()
{
    return ui->NoteSpinBox->value();
}
