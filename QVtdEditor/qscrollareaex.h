/**
 * qscrollareaex.h
 * Copyright (C) 2010 kbinani
 *
 * This file is part of QVtdEditor.
 *
 * QVtdEditor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU GPL License.
 *
 * QVtdEditor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef QSCROLLAREAEX_H
#define QSCROLLAREAEX_H

#include <QScrollArea>

/**
 * サイズ変更時にシグナルを送ることの出来るQScrollArea
 */
class QScrollAreaEx : public QScrollArea{
    Q_OBJECT

public:
    explicit QScrollAreaEx(QWidget *parent = 0);

protected:
    void resizeEvent( QResizeEvent *e );

signals:
    /**
     * サイズが変更されたときに発生するシグナル
     */
    void onResize();
};

#endif // QSCROLLAREAEX_H
