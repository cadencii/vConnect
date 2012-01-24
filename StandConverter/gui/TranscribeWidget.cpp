#include "TranscribeWidget.h"
#include "ui_TranscribeWidget.h"

#include <QStringList>

using namespace stand::gui;

TranscribeWidget::TranscribeWidget(QWidget *parent, int index) :
    QWidget(parent),
    ui(new Ui::TranscribeWidget)
{
    ui->setupUi(this);

    // Set ColorComboBox
    QStringList colorNames;
    colorNames << tr("red") << tr("green") << tr("blue") << tr("cyan") << tr("magenta") << tr("yellow") << tr("white");
    QPalette pallete = ui->ColorSelector->palette();
    pallete.setColor(QPalette::Highlight, Qt::transparent);

    int size = ui->ColorSelector->style()->pixelMetric(QStyle::PM_SmallIconSize);
    QPixmap pixmap(size, size);

    for(unsigned int i = 0; i < colorNames.size(); i++)
    {
        QString name = colorNames.at(i);
        ui->ColorSelector->addItem(name, QColor(i));
        pixmap.fill(QColor(name));
        ui->ColorSelector->setItemData(i, pixmap, Qt::DecorationRole);
    }
    ui->ColorSelector->setCurrentIndex(index % ui->ColorSelector->count());
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
