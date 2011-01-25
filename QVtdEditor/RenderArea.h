/**
 * RenderArea.h
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
#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QFrame>
#include <QPainter>
#include <QPen>

class RenderArea : public QWidget{
    Q_OBJECT
public:
    RenderArea( QWidget *parent );

signals:
    void onPaint( QPainter *e );
    void onMouseMove( QMouseEvent *e );
    void onMouseRelease( QMouseEvent *e );
    void onMousePress( QMouseEvent *e );

protected:
    void paintEvent( QPaintEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mousePressEvent( QMouseEvent *e );

};

#endif // RENDERAREA_H
