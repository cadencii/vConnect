/**
 * main.cpp
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
#include <QtGui/QApplication>
#include "MainWindow.h"

void fill( double *buf, int hz, int sample_rate ){
    double amp = 0.2;
    for( int i = 0; i < sample_rate; i++ ){
        double t = i / (double)sample_rate;
        buf[i] = amp * sin( 2.0 * 3.1415926 * t * hz );
    }
}

int main( int argc, char *argv[] ){
#if 0
    int sample_rate = 44100;
    WaveOut wo;
    wo.init();
    int length = sample_rate;//(int)(sample_rate * 0.4);
    wo.prepare( sample_rate );
    double *buf441 = new double[length];
    int hz = 441;
    fill( buf441, 441, length );
    double *buf882 = new double[length];
    fill( buf882, 882, length );
    for( int i = 0; i < 10; i++ ){
        if( i % 2 == 0 ){
            wo.append( buf441, buf441, length );
        }else{
            wo.append( buf882, buf882, length );
        }
    }
    do{
        //CFRunLoopRunInMode(
        //    kCFRunLoopDefaultMode,
        //    0.25,
        //    false
        //);
    }while( true );
    //wo.waitForExit();
    return 0;
#else
    QApplication a( argc, argv );
    MainWindow w;
    w.show();
    return a.exec();
#endif
}
