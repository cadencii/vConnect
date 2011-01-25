/**
 * RenderArea.cpp
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
#include "RenderArea.h"

RenderArea::RenderArea( QWidget *parent ) :
    QWidget( parent ){
}

void RenderArea::paintEvent( QPaintEvent *e ){
    QPainter painter( this );
    emit onPaint( &painter );
}

void RenderArea::mouseMoveEvent( QMouseEvent *e ){
    emit onMouseMove( e );
}

void RenderArea::mouseReleaseEvent( QMouseEvent *e ){
    emit onMouseRelease( e );
}

void RenderArea::mousePressEvent( QMouseEvent *e ){
    emit onMousePress( e );
}
