/**
 * MainWindow.cpp
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
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <QScrollBar>
#include <QMouseEvent>
#include <QFrame>
#include <QPolygon>
#include <math.h>
#include "ui_MainWindow.h"
#include "MainWindow.h"
#include "RenderArea.h"
#include "qscrollareaex.h"

MainWindow::MainWindow( QWidget *parent ) :
    QMainWindow( parent ),
    ui( new Ui::MainWindow )
{
    ui->setupUi( this );

    // penの初期化
    _pen_grid.setColor( Qt::darkGray );
    _pen_x_axis.setColor( Qt::black );
    _pen_spec_reference.setColor( QColor( 255, 182, 193 ) ); // means lightpink
    _pen_spec_reference.setWidth( 2 );
    _pen_spec_original.setColor( Qt::white );
    _pen_spec_original.setWidth( 2 );
    _pen_vtd.setColor( Qt::blue );
    _pen_vtd.setWidth( 2 );
    _pen_filter.setColor( Qt::red );
    _pen_filter.setWidth( 2 );

    // ブラシの初期化
    _brush_tpoints.setColor( Qt::magenta );
    _brush_tpoints.setStyle( Qt::SolidPattern );
    _brush_tpoints_select.setColor( Qt::darkMagenta );
    _brush_tpoints_select.setStyle( Qt::SolidPattern );

    // スペクトル等のデフォルト値
    this->src_f0 = 0.0;
    this->src_aperiodicity = new double[4];
    this->src_spectrum = new double[SPECTRUM_MAX_LENGTH];
    memset( this->src_spectrum, 0, sizeof( double ) * SPECTRUM_MAX_LENGTH );

    this->spectra = new double*[NUM_SPECTRUM_MAX];
    this->num_spectra = 0;
    memset( this->spectra, 0, sizeof( double * ) * NUM_SPECTRUM_MAX );

    this->filter = new double[SPECTRUM_LENGTH];
    this->trans  = new double[SPECTRUM_LENGTH];
    for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
        this->filter[i] = 0.0;//0.2 * sin( ST_PI * 2.0 / SPECTRUM_LENGTH * (double)i );//0.0;
        trans[i] = i;
    }
    t_points = new editor_point[NUM_T_POINT_MAX];
    for( int i = 0; i < NUM_T_POINT_MAX; i++ ){
        float x = SPECTRUM_LENGTH * i / NUM_T_POINT_MAX + SPECTRUM_LENGTH / NUM_T_POINT_MAX / 2;
        t_points[i].src_x = (int)x;
        t_points[i].x = x;
        t_points[i].select = false;
    }

    // グラフ描画時に使うバッファ
    this->_buf_points = new QPoint[SPECTRUM_LENGTH];

    // グラフ部分のコンポーネントの初期化
    this->renderArea = new RenderArea( NULL );
    this->renderArea->setSizePolicy( QSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ) );
    this->renderArea->setGeometry( 0, 0, SPECTRUM_LENGTH, 500 );
    ui->scrollArea->setWidget( this->renderArea );
    ui->scrollArea->setFrameShape( QFrame::NoFrame );
    ui->scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    ui->scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    // シグナル・スロットの接続
    connect( ui->pushButtonSave, SIGNAL(clicked()), this, SLOT(pushButtonSaveClick()) );
    connect( ui->pushButtonExit, SIGNAL(clicked()), this, SLOT(pushButtonExitClick()) );
    connect( ui->pushButtonAdd, SIGNAL(clicked()), this, SLOT(pushButtonAddClick()) );
    connect( ui->pushButtonImport, SIGNAL(clicked()), this, SLOT(pushButtonImportClick()) );
    connect( ui->pushButtonPlay, SIGNAL(clicked()), this, SLOT(pushButtonPlayClick()) );
    connect( ui->pushButtonReset, SIGNAL(clicked()), this, SLOT(pushButtonResetClick()) );
    connect( ui->actionImport, SIGNAL(triggered()), this, SLOT(menuFileImportClick()) );
    connect( ui->actionExit, SIGNAL(triggered()), this, SLOT(menuFileExitClick()) );
    connect( ui->scrollArea, SIGNAL(onResize()), this, SLOT(mainWindowResize()) );
    connect( this->renderArea, SIGNAL(onPaint(QPainter *)), this, SLOT(renderAreaPaint(QPainter*)) );
    connect( this->renderArea, SIGNAL(onMouseMove(QMouseEvent*)), this, SLOT(renderAreaMouseMove(QMouseEvent*)) );
    connect( this->renderArea, SIGNAL(onMouseRelease(QMouseEvent*)), this, SLOT(renderAreaMouseRelease(QMouseEvent*)) );
    connect( this->renderArea, SIGNAL(onMousePress(QMouseEvent*)), this, SLOT(renderAreaMousePress(QMouseEvent*)) );

    // 描画してないよ！
    draw_type = DRAW_NONE;

    // サウンドプレイヤーを初期化
    _player.init();
}

MainWindow::~MainWindow(){
    delete renderArea;
    delete [] this->src_aperiodicity;
    delete [] this->src_spectrum;
    for( int i = 0; i < this->num_spectra; i++ ){
        delete [] this->spectra[i];
    }
    delete [] this->spectra;
    delete [] this->filter;
    delete [] this->trans;
    delete [] this->_buf_points;
    delete [] this->t_points;
    _player.kill();
}

void MainWindow::changeEvent( QEvent *e ){
    QMainWindow::changeEvent( e );
    switch( e->type() ) {
        case QEvent::LanguageChange:{
            ui->retranslateUi( this );
            break;
        }
        default:{
            break;
        }
    }
}

void MainWindow::resizeEvent( QResizeEvent *e ){
    emit onResize();
}

bool MainWindow::make_sure_exit(){
    bool ret = false;

    int result = QMessageBox::question(
                    this,
                    "QVtdEditor",
                    tr( "do you want to exit?" ),
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
    if( result == QMessageBox::Yes ){
        ret = true;
    }
    return ret;
}

void MainWindow::renderAreaPaint( QPainter *g ){
    float height = this->renderArea->height();
    float width  = this->renderArea->width();

    // TODO:今後要検討(tempをメンバー変数に)
    double  *temp = new double[SPECTRUM_LENGTH];
    memcpy( temp, src_spectrum, sizeof(double) * SPECTRUM_LENGTH );
    applyStretching( trans, temp, SPECTRUM_LENGTH );

    g->fillRect( 0, 0, (int)width, (int)height, Qt::gray );
    int half_height = (int)(height / 2);
    int num_y_grid = 24;
    float yscale = height / (float)num_y_grid; //グリッド１個あたり何ピクセルか？
    int x, y, lasty;

    // グリッドの表示、横線
    g->setPen( _pen_grid );
    y = 0;
    for( int i = 1; i < num_y_grid; i++ ){
        if( i == num_y_grid / 2 ){
            continue;
        }
        y = (int)(yscale * i);
        g->drawLine( 0, y, SPECTRUM_LENGTH, y );
    }

    // x軸の表示
    g->setPen( _pen_x_axis );
    g->drawLine( 0, half_height, SPECTRUM_LENGTH, half_height );

    // グリッドの表示、縦線
    g->setPen( _pen_grid );
    x = 0;
    int num = width / 64 + 1;
    for( int i = 1; i < num; i++ ){
        x += 64;
        g->drawLine( x, 0, x, (int)height );
    }

    // 元スペクトルの描画
    g->setPen( _pen_spec_original );
    for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
        y = (int)height - (int)((0.5 + src_spectrum[i]) * half_height);
        this->_buf_points[i].setX( i );
        this->_buf_points[i].setY( y );
    }
    g->drawPolyline( this->_buf_points, SPECTRUM_LENGTH );

    // 参考用スペクトルを描画
    g->setPen( _pen_spec_reference );
    lasty = 0;
    for( int sp_i = 0; sp_i < this->num_spectra; sp_i++ ){
        double *sp = this->spectra[sp_i];
        for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
            y = (int)height - (int)((0.5 + sp[i]) * half_height);
            //this->_buf_points[i].setX( i );
            this->_buf_points[i].setY( y );
        }
        g->drawPolyline( this->_buf_points, SPECTRUM_LENGTH );
    }

    // VoiceTexturingData の表示
    g->setPen( _pen_vtd );
    for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
        y = (int)(half_height - filter[i] * half_height);
        //this->_buf_points[i].setX( i );
        this->_buf_points[i].setY( y );
    }
    g->drawPolyline( this->_buf_points, SPECTRUM_LENGTH );

    // 変形後スペクトルの描画
    g->setPen( _pen_filter );
    for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
        y = (int)height - (int)((0.5 + filter[i] + temp[i]) * half_height);
        //this->_buf_points[i].setX( i );
        this->_buf_points[i].setY( y );
    }
    g->drawPolyline( this->_buf_points, SPECTRUM_LENGTH );

    /* 伸縮制御点の描画 */
    for( int i = 0; i < NUM_T_POINT_MAX; i++ ){
        t_points[i].y = height / 2.0f;
        if( 0 <= t_points[i].x && t_points[i].x < SPECTRUM_LENGTH ){
            t_points[i].y = height / 2.0f - filter[(int)t_points[i].x] * height / 2.0f;
        }
        if( t_points[i].select ){
            g->fillRect( t_points[i].x - DOT_RADIUS, t_points[i].y - DOT_RADIUS, DOT_RADIUS * 2, DOT_RADIUS * 2, _brush_tpoints_select );
        }else{
            g->fillRect( t_points[i].x - DOT_RADIUS, t_points[i].y - DOT_RADIUS, DOT_RADIUS * 2, DOT_RADIUS * 2, _brush_tpoints );
        }
    }
}

bool MainWindow::read_spectrum_file( QString input, double *target, double *target_ap, double *target_f0 ){
    bool ret = false;
    try{
        const int BUFLEN = 4096;
        char *buf = new char[BUFLEN];
        FILE *sr = fopen( input.toStdString().c_str(), "rb" );
        if( !sr ){
            return false;
        }
        try{
            std::string line;
            int i = 0;
            while( fgets( buf, BUFLEN, sr ) ){
                line = buf;
                target[i] = (double)atof( line.c_str() );
                i++;
                if( ( i > 1023 && !target_ap ) || i > 2047 ){
                    break;
                }
            }
            i = 0;
            // ap があった場合は読む。無ければ放置。
            if( target_ap && target_f0 ){
                while( fgets( buf, BUFLEN, sr ) ){
                    if( i == 4 ) break;
                    line = buf;
                    target_ap[i] = (double)atof( line.c_str() );
                    i++;
                }
                *target_f0 = (double)atof( line.c_str() );
            }
        }catch( const char* ex ){
        }
        if( sr ){
            fclose( sr  );
            ret = true;
        }
        if( buf ){
            delete [] buf;
        }
    }catch( const char *ex ) {
        qDebug( "MainWindow::read_spectrum_file; The file could not be read; " );
        qDebug( "MainWindow::read_spectrum_file; ex=%s", ex );
    }
    // 無事に読み込めた場合は正規化をしておく
    if( ret ){
        src_min = 10000000000.0;
        src_max = -10000000000.0;
        for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
            if( src_min > target[i] ){
                src_min = target[i];
            }
        }
        for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
            target[i] = log( target[i] / src_min );
        }

        for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
            if( src_max < target[i] ){
                src_max = target[i];
            }
        }
        for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
            target[i] /= src_max;
        }

    }
    return ret;
}

bool MainWindow::on_import_select(){
    bool ret = false;
    QString file_name;

    file_name = QFileDialog::getOpenFileName(
            this,
            tr( "Select File" ),
            QProcess( this ).workingDirectory(),
            tr( "WORLD Spectrum (*.wsp);;All Files (*.*)" ) );

//#ifdef _DEBUG
    qDebug( "MainWindow::on_import_select; file_name=%s", file_name.toStdString().c_str() );
//#endif
    if( !file_name.isEmpty() ){
        double tmp;
        ret = this->read_spectrum_file( file_name, this->src_spectrum, this->src_aperiodicity, &tmp );
        this->src_f0 = tmp;
    }

    this->update();
    return ret;
}

bool MainWindow::on_add_select(){
    bool ret = false;
    QString file_name;

    file_name =QFileDialog::getOpenFileName(
        this,
        tr( "Select File "),
        QProcess( this ).workingDirectory(),
        tr( "WORLD Spectrum (*.wsp);;All Files (*.*)" ) );

    if( !file_name.isEmpty() ){
        // 第３引数が NULL なら非周期性以降は読み込まない。
        this->spectra[this->num_spectra] = new double[SPECTRUM_LENGTH];
        if( ret = this->read_spectrum_file( file_name, this->spectra[this->num_spectra], NULL, NULL ) ){
            this->num_spectra++;
        }
    }

    this->update();
    return ret;
}

bool MainWindow::write_vtd_file( QString output ){
    bool ret = false;
    QString line;

    FILE *sw = fopen( output.toStdString().c_str(), "wb" );
    if( !sw ){
        qDebug( "MainWindow::write_vtd_file; cannot open file: '%s'", output.toStdString().c_str() );
        return false;
    }
    try{
        for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
            fprintf( sw, "%e¥n", filter[i] );
        }
        for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
            fprintf( sw, "%e¥n", trans[i] );
        }
        ret = true;
    }catch( const char *ex ){
        qDebug( "MainWindow::write_vtd_file; ex=%s", ex );
    }
    if( sw ){
        fclose( sw );
    }
    return ret;
}

bool MainWindow::on_save_select(){
    bool ret = false;
    QString file_name;

    file_name = QFileDialog::getSaveFileName(
        this,
        tr( "Select File" ),
        QProcess( this ).workingDirectory(),
        tr( "Voice Texturing Data (*.vtd);;All Files (*.*)" ) );

    if( !file_name.isEmpty() ){
        ret = this->write_vtd_file( file_name );
    }

    return ret;
}

void MainWindow::pushButtonSaveClick(){
    on_save_select();
}

void MainWindow::pushButtonExitClick(){
    if( make_sure_exit() ){
        this->close();
    }
}

void MainWindow::pushButtonAddClick(){
    on_add_select();
}

void MainWindow::pushButtonImportClick(){
    on_import_select();
}

void MainWindow::menuFileImportClick(){
    on_import_select();
}

void MainWindow::menuFileExitClick(){
    if( make_sure_exit() ){
        this->close();
    }
}

void MainWindow::mainWindowResize(){
    on_resize_graph();
}

void MainWindow::on_resize_graph(){
    // 高さをリサイズ
    int height = ui->scrollArea->contentsRect().height() - ui->scrollArea->horizontalScrollBar()->height();
    QRect rc = this->renderArea->geometry();
    rc.setHeight( height );
    this->renderArea->setGeometry( rc );
}

void MainWindow::renderAreaMouseMove( QMouseEvent *e ){
    if( (e->buttons() & Qt::LeftButton) != Qt::LeftButton ){
        prev_x = e->x();
        prev_y = e->y();
        return;
    }

    int x = e->x();
    int y = e->y();
    int index = 0;

    switch( draw_type ){
        case DRAW_FILTER:{
            int begin_x, begin_y;
            int end_x, end_y;
            if( prev_x > x ){
                begin_x = x; begin_y = y; end_x = prev_x; end_y = prev_y;
            } else {
                begin_x = prev_x; begin_y = prev_y; end_x = x; end_y = y;
            }
            float m;
            for( int n = begin_x; n <= end_x; n++ ){
                double rate = (double)( n - begin_x ) / (double)( end_x - begin_x + 1 );
                m = (float)begin_y * ( 1.0 -rate ) + (float)end_y * rate;
                if( 0 <= n && n < SPECTRUM_LENGTH )
                    filter[n] = - (double)( m - this->renderArea->height() / 2 ) / (double)( this->renderArea->height() / 2 );
            }

            this->renderArea->update();
            break;
        }
        case DRAW_TRANS:{
            if( x < trans_index )
                x = trans_index;
            if( x > SPECTRUM_LENGTH - NUM_T_POINT_MAX + trans_index - 1 )
                x = SPECTRUM_LENGTH - NUM_T_POINT_MAX + trans_index - 1;
            for( int i = 0; i < trans_index; i++ )
                t_points[i].x *= (float)x / ( t_points[trans_index].x + 0.000001f );
            for( int i = trans_index + 1; i < NUM_T_POINT_MAX; i++ )
                t_points[i].x = SPECTRUM_LENGTH - 1 - ( SPECTRUM_LENGTH - t_points[i].x - 1 )
                              * (float)( SPECTRUM_LENGTH - x - 1 ) / ( SPECTRUM_LENGTH - t_points[trans_index].x - 1 );

            t_points[trans_index].x = (float)x;

            for( index = 0; index < t_points[0].src_x; index++ ){
                trans[index] = (double)index * ( t_points[0].x / (double)t_points[0].src_x );
                trans[index] = (double)index + ( (double)index - trans[index] );
            }
            for( int i = 0; i < NUM_T_POINT_MAX; i++ ){
                int next_index;
                double next, begin_i;
                if( i == NUM_T_POINT_MAX - 1 ){
                    next_index = SPECTRUM_LENGTH;
                    next = SPECTRUM_LENGTH;
                }else{
                    next_index = t_points[i+1].src_x;
                    next = t_points[i+1].x;
                }

                begin_i = t_points[i].x;

                for( ; index < next_index; index++ ){
                    trans[index] = begin_i + ( next - begin_i ) * (double)( index - t_points[i].src_x ) / (double)( next_index - t_points[i].src_x );
                    trans[index] = (double)index + ( (double)index - trans[index] );
                }
            }

            this->renderArea->update();
            break;
        }
        default:{
            break;
        }
    }
    prev_x = x;
    prev_y = y;
}

void MainWindow::renderAreaMouseRelease( QMouseEvent *e ){
    if( draw_type == DRAW_TRANS ){
        t_points[trans_index].select = false;
        this->renderArea->update();
    }
    draw_type = DRAW_NONE;
}

void MainWindow::renderAreaMousePress( QMouseEvent *e ){
    prev_x = e->x();
    prev_y = e->y();
    if( (e->buttons() & Qt::LeftButton) != Qt::LeftButton ){
        return;
    }

    for( int i = 0; i < NUM_T_POINT_MAX; i++ ){
        if( fabs( e->x() - t_points[i].x ) <= DOT_RADIUS && fabs( (float)e->y() - t_points[i].y ) <= DOT_RADIUS ){
            draw_type = DRAW_TRANS;
            trans_index = i;
            t_points[i].select = true;
            this->renderArea->update();
            return;
        }
    }

    draw_type = DRAW_FILTER;
}

void MainWindow::pushButtonPlayClick(){
    standSpecgram specgram;
    standFrame    frame;
    long          w_len;
    double       *w_out;
    int           tLen = 1000.0 / framePeriod;
    double       *tempSp = new double[2048];
    double        w_max;

    specgram.setFrameLength( tLen, true );

    /* Spectrum を復元 */
    for( int i = 0; i < SPECTRUM_LENGTH; i++ )
        tempSp[i] = exp( ( src_spectrum[i] + filter[i] ) * src_max ) * src_min;
    // 伸縮を適用
    applyStretching( trans, tempSp, SPECTRUM_LENGTH );
    for( int i = SPECTRUM_LENGTH; i < 2048; i++ )
        tempSp[i] = src_spectrum[i];

    for( int i = 0; i < tLen; i++ ){
        specgram.getFramePointer( i, frame );
        *frame.f0 = 330.0;//src_f0;
        *frame.t  = (double)i * framePeriod / 1000.0;
        memcpy( frame.spectrum, tempSp, sizeof(double) * 2048 );
        memcpy( frame.aperiodicity, src_aperiodicity, sizeof(double) * 4 );
    }

    qDebug( "MainWindow::pushButtonPlayClick; calling specgram.synthesizeWave..." );
    w_out = specgram.synthesizeWave( &w_len );

    qDebug( "MainWindow::pushButtonPlayClick; normalize..." );
    /* Normalize */
    w_max = 1.0;
    for( int i = 0; i < w_len; i++ ){
        if( fabs( w_out[i] ) > w_max ){
            w_max = fabs( w_out[i] );
        }
    }
    double inv_w_max = 1.0 / w_max;
    for( int i = 0; i < w_len; i++ ){
        w_out[i] *= inv_w_max;
    }

#if 0
    double amplitude = 0.2;
    double ifreq = 800;
    double ffreq = 400;
    for( int i = 0; i < w_len; i++ ){
        double t = i / (double)fs;
        double freq = ifreq + (ffreq - ifreq) / (double)w_len * (double)i;
        w_out[i] = amplitude * sin( 2.0 * ST_PI * t * freq );
    }
#endif
    qDebug( "MainWindow::pushButtonPlayClick; calling _player.prepare..." );
    // 再生。現状謎の音が出るっす
    _player.prepare( fs );
    qDebug( "MainWindow::pushButtonPlayClick; calling _player.append..." );
    _player.append( w_out, w_out, (int)w_len );
    qDebug( "MainWindow::pushButtonPlayClick; calling _player.waitForExit..." );
    _player.waitForExit();
    qDebug( "MainWindow::pushButtonPlayClick; calling _player.unprepare..." );
    _player.unprepare();

    SAFE_DELETE_ARRAY( tempSp );
}

void MainWindow::pushButtonResetClick(){
    on_reset_select();
}

void MainWindow::on_reset_select(){
    for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
        this->filter[i] = 0.0; //0.2 * Math::Sin( ST_PI * 2.0 / SPECTRUM_LENGTH * (double)i );//0.0;
        trans[i] = i;
    }

    for( int i = 0; i < NUM_T_POINT_MAX; i++ ){
        t_points[i].src_x = t_points[i].x = SPECTRUM_LENGTH * i / NUM_T_POINT_MAX + SPECTRUM_LENGTH / NUM_T_POINT_MAX / 2;
        t_points[i].select = false;
    }

    this->renderArea->update();
}
