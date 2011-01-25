/**
 * QScrollAreaEx.cpp
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
#include "qscrollareaex.h"

QScrollAreaEx::QScrollAreaEx( QWidget *parent ) :
    QScrollArea( parent )
{
}

void QScrollAreaEx::resizeEvent( QResizeEvent *e ){
    emit onResize();
}
