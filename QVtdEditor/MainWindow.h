/**
 * MainWindow.h
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColor>
#include <QPen>
#include <stdio.h>

#include "../stand/vConnect.h"
#include "../stand/WaveOut.h"

#define NUM_SPECTRUM_MAX 256
#define SPECTRUM_LENGTH 1024
#define SPECTRUM_MAX_LENGTH 4096
#define NUM_T_POINT_MAX 16
//制御点の描画幅(ピクセル)
#define DOT_RADIUS 4

#define _DEBUG

#ifndef ST_PI
#define ST_PI 3.141592653589793238462643383279
#endif

namespace Ui{
    class MainWindow;
}
class RenderArea;
class QScrollAreaEx;
class WaveOut;

enum DRAW_TYPE{
    DRAW_NONE = 0,
    DRAW_FILTER,
    DRAW_TRANS
};

struct editor_point{
    int     src_x;
    float   x;
    float   y;
    bool    select;
};

class MainWindow : public QMainWindow{
    Q_OBJECT
public:
    MainWindow( QWidget *parent = 0 );
    ~MainWindow();

private:
    bool read_spectrum_file( QString input, double *target, double *target_ap, double *target_f0 );
    bool write_vtd_file( QString output );
    bool make_sure_exit();
    bool on_import_select();
    bool on_add_select();
    bool on_save_select();
    void on_reset_select();
    void on_resize_graph();

protected:
    void changeEvent( QEvent *e );
    void resizeEvent( QResizeEvent *e );

private:
    Ui::MainWindow* ui;
    // グラフの描画エリア
    RenderArea*     renderArea;

    // グラフの線の描画に使用するpen達
    QPen            _pen_grid;
    QPen            _pen_x_axis;
    QPen            _pen_spec_reference;
    QPen            _pen_spec_original;
    QPen            _pen_vtd;
    QPen            _pen_filter;
    QBrush  _brush_tpoints;
    QBrush  _brush_tpoints_select;

    // スペクトル等
    double**    spectra;
    double*     src_spectrum;
    double*     src_aperiodicity;
    double      src_min;
    double      src_max;
    double      src_f0;
    int         num_spectra;
    double*     filter;
    double*     trans;
    int         trans_index;
    editor_point*   t_points;

    // グラフ上での描画モード
    DRAW_TYPE   draw_type;

    // グラフの線を描くためのバッファ
    QPoint*     _buf_points;

    // グラフ上での最新のマウス位置
    int         prev_x;
    int         prev_y;

    // サウンドプレイヤー
    WaveOut   _player;

public slots:
    void pushButtonSaveClick();
    void pushButtonExitClick();
    void pushButtonAddClick();
    void pushButtonImportClick();
    void pushButtonPlayClick();
    void pushButtonResetClick();
    void menuFileImportClick();
    void menuFileExitClick();
    void mainWindowResize();
    void renderAreaMouseMove( QMouseEvent *e );
    void renderAreaPaint( QPainter *e );
    void renderAreaMouseRelease( QMouseEvent *e );
    void renderAreaMousePress( QMouseEvent *e );

signals:
    void onResize();
};

#endif // MAINWINDOW_H
