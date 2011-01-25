#include "wspEditor.h"

using namespace vtdEditor; 

// wspEditorはpure CLIなクラスなので、ネイティブなstruct等(HWAVEOUT)な
// フィールドを持つWaveOutをメンバーに出来ない
static WaveOut _player;

wspEditor::wspEditor()
{
    InitializeComponent();
    pictureBox1->Paint += gcnew PaintEventHandler( this, &wspEditor::draw_spectrum );
    this->src_f0 = 0.0;
    this->src_aperiodicity = new double[4];
    this->src_spectrum = new double[SPECTRUM_MAX_LENGTH];
    memset( this->src_spectrum, 0, sizeof(double) *  SPECTRUM_MAX_LENGTH );

    this->spectra = new double*[NUM_SPECTRUM_MAX];
    this->num_spectra = 0;
    memset( this->spectra, 0, sizeof(double*) * NUM_SPECTRUM_MAX );

    this->filter = new double[SPECTRUM_LENGTH];
    this->trans  = new double[SPECTRUM_LENGTH];
    for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
        this->filter[i] = 0.0; //0.2 * Math::Sin( ST_PI * 2.0 / SPECTRUM_LENGTH * (double)i );//0.0;
        trans[i] = i;
    }

    t_points = new editor_point[NUM_T_POINT_MAX];
    for( int i = 0; i < NUM_T_POINT_MAX; i++ ){
        t_points[i].src_x = t_points[i].x = SPECTRUM_LENGTH * i / NUM_T_POINT_MAX + SPECTRUM_LENGTH / NUM_T_POINT_MAX / 2;
        t_points[i].select = false;
    }

    // グラフ描画時に使うバッファ
    _buf_points = gcnew array<Point>( SPECTRUM_LENGTH );

    // penの初期化
    _pen_grid = gcnew Pen( Color::LightGray );
    _pen_x_axis = gcnew Pen( Color::Black );
    _pen_spec_reference = gcnew Pen( Color::LightPink, 2 );
    _pen_spec_original = gcnew Pen( Color::LightGray, 2 );
    _pen_vtd = gcnew Pen( Color::Blue, 2 );
    _pen_filter = gcnew Pen( Color::Red, 2 );

    // ブラシの初期化
    _brush_tpoints = gcnew SolidBrush( Color::Magenta );
    _brush_tpoints_select = gcnew SolidBrush( Color::DarkMagenta );

    /* 描画してないよ！ */
    draw_type = DRAW_NONE;

    // プレーヤーの初期化
    _player.init();
}

wspEditor::~wspEditor()
{
    if (components){
        SAFE_DELETE_ARRAY( t_points );
        SAFE_DELETE_ARRAY( this->src_aperiodicity );
        SAFE_DELETE_ARRAY( this->filter );
        SAFE_DELETE_ARRAY( this->trans );
        SAFE_DELETE_ARRAY( this->src_spectrum );
        for( int i = 0; i < this->num_spectra; i++ )
            SAFE_DELETE_ARRAY( this->spectra[i] );
        SAFE_DELETE_ARRAY( this->spectra );
        delete components;
    }

    _player.kill();
}

bool wspEditor::make_sure_exit()
{
    bool ret = false;
    String^ message = "do you want to exit?";
    String^ caption = "vtdEditor";
    MessageBoxButtons button = MessageBoxButtons::YesNo;
    Windows::Forms::DialogResult result;

    result = MessageBox::Show( this, message, caption, button );
    if( result == Windows::Forms::DialogResult::Yes )
        ret = true;
    return ret;
}

void wspEditor::draw_spectrum( Object^ sender, PaintEventArgs^ e )
{
    Graphics^ g = e->Graphics;
    float   height = (float)pictureBox1->Height;
    float   width  = (float)pictureBox1->Width;
    double  *temp = new double[SPECTRUM_LENGTH];
    memcpy( temp, src_spectrum, sizeof(double) * SPECTRUM_LENGTH );
    applyStretching( trans, temp, SPECTRUM_LENGTH );

    /* グリッドの表示 */
    for( int i = 1; i < height / 64; i++ )
        g->DrawLine( _pen_grid, 0, i * 64, SPECTRUM_LENGTH, i * 64 );
    g->DrawLine( _pen_x_axis, 0, (int)( height / 2 ), SPECTRUM_LENGTH, (int)( height / 2 ) );
    for( int i = 1; i < width / 64; i++ )
        g->DrawLine( _pen_grid, i * 64, 0, i * 64, (int)height );

    /* 参考用スペクトルを描画 */
    for( int sp_i = 0; sp_i < this->num_spectra; sp_i++ ){
        double *sp = spectra[sp_i];
        for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
            this->_buf_points[i].X = i;
            this->_buf_points[i].Y = (int)height - (int)((0.5 + sp[i]) * height / 2);
        }
        g->DrawLines( this->_pen_spec_reference, this->_buf_points );
    }

    /* 元スペクトルの描画 */
    for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
        _buf_points[i].X = i;
        _buf_points[i].Y = (int)height - (int)( ( 0.5 + src_spectrum[i]   ) * height / 2 );
    }
    g->DrawLines( _pen_spec_original, _buf_points );

    /* VoiceTexturingData の表示 */
    for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
        _buf_points[i].X = i;
        _buf_points[i].Y = (int)( height / 2 - filter[i] * height / 2 );
    }
    g->DrawLines( _pen_vtd, _buf_points );

    /* 変形後スペクトルの描画 */
    for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
        _buf_points[i].X = i;
        _buf_points[i].Y = (int)height - (int)( ( 0.5 + filter[i]   + temp[i]   ) * height / 2 );
    }
    g->DrawLines( _pen_filter, _buf_points );

    /* 伸縮制御点の描画 */
    for( int i = 0; i < NUM_T_POINT_MAX; i++ ){
        t_points[i].y = height / 2.0f;
        if( 0 <= t_points[i].x && t_points[i].x < SPECTRUM_LENGTH ){
            t_points[i].y = height / 2.0f - filter[(int)t_points[i].x] * height / 2.0f;
        }
        if( t_points[i].select ){
            g->FillEllipse( _brush_tpoints_select, t_points[i].x - 4.0f, t_points[i].y - 4.0f, 8.0f, 8.0f );
        }else{
            g->FillEllipse( _brush_tpoints, t_points[i].x - 4.0f, t_points[i].y - 4.0f, 8.0f, 8.0f );
        }
    }
}

bool wspEditor::read_spectrum_file( String^ input, double* target, double* target_ap, double* target_f0 )
{
    bool ret = false;
    try{
        System::IO::StreamReader^ sr = gcnew System::IO::StreamReader( input );
        try{
            String^ line;
            int i = 0;
            while( line = sr->ReadLine() ){
                target[i] = (double)Double::Parse( line );
                i++;
                if( ( i > 1023 && !target_ap ) || i > 2047 )
                    break;
            }
            i = 0;
            // ap があった場合は読む。無ければ放置。
            if( target_ap && target_f0 ){
                while( line = sr->ReadLine() ){
                    if( i == 4 ) break;
                    target_ap[i] = (double)Double::Parse( line );
                    i++;
                }
                *target_f0 = (double)Double::Parse( line );
            }
        } finally {
            if( sr )
                delete ( IDisposable^ )sr;
            ret = true;
        }
    } catch( System::Exception^ e ) {
        Console::WriteLine( "The file could not be read; " );
        Console::WriteLine( e->Message );
    }
        /* 無事に読み込めた場合は正規化をしておく */
    if( ret ){
        src_min = 10000000000;
        src_max = -10000000000;
        for( int i = 0; i < SPECTRUM_LENGTH; i++ )
            if( src_min > target[i] )
                src_min = target[i];
        for( int i = 0; i < SPECTRUM_LENGTH; i++ )
            target[i] = log( target[i] / src_min );

        for( int i = 0; i < SPECTRUM_LENGTH; i++ )
            if( src_max < target[i] )
                src_max = target[i];
        for( int i = 0; i < SPECTRUM_LENGTH; i++ )
            target[i] /= src_max;
    }
    return ret;
}

bool wspEditor::on_import_select( void )
{
    bool ret = false;
    OpenFileDialog^ ofd = gcnew OpenFileDialog();
    String^ file_name;

    ofd->InitialDirectory = System::IO::Directory::GetCurrentDirectory();
    ofd->Filter           = L"WORLD Spectrum (*.wsp)|*.wsp|" + L"All Files (*.*)|*.*";
    ofd->FilterIndex      = 1;
    ofd->Multiselect      = false;

    if( ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK ) {
        file_name = ofd->FileName;
        double tmp;
        ret = this->read_spectrum_file( file_name, this->src_spectrum, this->src_aperiodicity, &tmp );
        this->src_f0 = tmp;
    }

    this->Refresh();
    return ret;
}

bool wspEditor::on_add_select( void )
{
    bool ret = false;
    OpenFileDialog^ ofd = gcnew OpenFileDialog();
    String^ file_name;

    ofd->InitialDirectory = System::IO::Directory::GetCurrentDirectory();
    ofd->Filter           = L"WORLD Spectrum (*.wsp)|*.wsp|" + L"All Files (*.*)|*.*";
    ofd->FilterIndex      = 1;
    ofd->Multiselect      = false;

    if( ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK ) {
        file_name = ofd->FileName;
        // 第３引数が NULL なら非周期性以降は読み込まない。
        this->spectra[this->num_spectra] = new double[SPECTRUM_LENGTH];
        if( ret = this->read_spectrum_file( file_name, this->spectra[this->num_spectra], NULL, NULL ) )
            this->num_spectra++;
    }

    this->Refresh();
    return ret;
}

bool wspEditor::write_vtd_file( String^ output )
{
    bool ret = false;
    String^ line;

    try{
        System::IO::StreamWriter^ sw = gcnew System::IO::StreamWriter( output );
        try{
            for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
                Double^ tmp = filter[i];
                line = tmp->ToString();
                sw->WriteLine( line );
            }
            for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
                Double^ tmp = trans[i];
                line = tmp->ToString();
                sw->WriteLine( line );
            }
            ret = true;
        } finally {
            delete (IDisposable^)sw;
        }
    } catch( System::Exception^ e ) {
        Console::WriteLine( "The file could not be read; " );
        Console::WriteLine( e->Message );
    }
    return ret;
}

bool wspEditor::on_save_select( void )
{
    bool ret = false;
    SaveFileDialog^ sfd = gcnew SaveFileDialog();
    String^ file_name;

    sfd->InitialDirectory = System::IO::Directory::GetCurrentDirectory();
    sfd->Filter           = L"Voice Texturing Data (*.vtd)|*.vtd|" + L"All Files (*.*)|*.*";
    sfd->FilterIndex      = 1;

    if( sfd->ShowDialog() == System::Windows::Forms::DialogResult::OK ) {
        file_name = sfd->FileName;
        ret = this->write_vtd_file( file_name );
    }

    return ret;
}

System::Void wspEditor::pictureBox1_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^  e )
{
    if( e->Button != System::Windows::Forms::MouseButtons::Left ) return;

    for( int i = 0; i < NUM_T_POINT_MAX; i++ ){
        if( abs( e->X - t_points[i].x ) <= 4 && fabs( (float)e->Y - t_points[i].y ) <= 4 ){
            draw_type = DRAW_TRANS;
            trans_index = i;
            t_points[i].select = true;
            return;
        }
    }

    draw_type = DRAW_FILTER;
}

System::Void wspEditor::pictureBox1_MouseUp( System::Object^ sender, System::Windows::Forms::MouseEventArgs^  e )
{
    if( draw_type == DRAW_TRANS ){
        t_points[trans_index].select = false;
        this->Refresh();
    }
    draw_type = DRAW_NONE;
}


System::Void wspEditor::pictureBox1_MouseMove( System::Object^ sender, System::Windows::Forms::MouseEventArgs^  e )
{
    static int prev_x = e->X;
    static int prev_y = e->Y;
    if( e->Button != System::Windows::Forms::MouseButtons::Left ){
        prev_x = e->X;
        prev_y = e->Y;
        return;
    }

    int x = e->X;
    int y = e->Y;
    int index = 0;

    switch( draw_type ){
        case DRAW_FILTER:
            int begin_x, begin_y;
            int end_x, end_y;
            if( prev_x > e->X ){
                begin_x = e->X; begin_y = e->Y; end_x = prev_x; end_y = prev_y;
            } else {
                begin_x = prev_x; begin_y = prev_y; end_x = e->X; end_y = e->Y;
            }
            float m;
            for( int n = begin_x; n <= end_x; n++ ){
                double rate = (double)( n - begin_x ) / (double)( end_x - begin_x + 1 );
                m = (float)begin_y * ( 1.0 -rate ) + (float)end_y * rate;
                if( 0 <= n && n < SPECTRUM_LENGTH )
                    filter[n] = - (double)( m - this->pictureBox1->Height / 2 ) / (double)( this->pictureBox1->Height / 2 );
            }

            this->Refresh();
            break;
        case DRAW_TRANS:
            if( x < trans_index )
                x = trans_index;
            if( x > SPECTRUM_LENGTH - NUM_T_POINT_MAX + trans_index - 1 )
                x = SPECTRUM_LENGTH - NUM_T_POINT_MAX + trans_index - 1;
            for( int i = 0; i < trans_index; i++ )
                t_points[i].x *= (float)x / ( t_points[trans_index].x + 0.000001f );
            for( int i = trans_index + 1; i < NUM_T_POINT_MAX; i++ )
                t_points[i].x = SPECTRUM_LENGTH - 1 - ( SPECTRUM_LENGTH - t_points[i].x - 1 )
                              * (float)( SPECTRUM_LENGTH - x - 1 ) / ( SPECTRUM_LENGTH - t_points[trans_index].x - 1 );

            t_points[trans_index].x = (float)e->X;

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

            this->Refresh();
            break;
        default:
            break;
    }
    prev_x = e->X;
    prev_y = e->Y;
}

System::Void wspEditor::buttonPlay_Click(System::Object^  sender, System::EventArgs^  e)
{
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

    w_out = specgram.synthesizeWave( &w_len );

    /* Normalize */
    w_max = 1.0;
    for( int i = 0; i < w_len; i++ )
        if( fabs(w_out[i]) > w_max )
            w_max = fabs(w_out[i]);
    for( int i = 0; i < w_len; i++ )
        w_out[i] /= w_max;

    // 再生
    _player.prepare( fs );
    _player.append( w_out, w_out, (int)w_len );
    _player.waitForExit();
    _player.unprepare();

    SAFE_DELETE_ARRAY( tempSp );
}

void wspEditor::on_reset_select( void )
{
    for( int i = 0; i < SPECTRUM_LENGTH; i++ ){
        this->filter[i] = 0.0; //0.2 * Math::Sin( ST_PI * 2.0 / SPECTRUM_LENGTH * (double)i );//0.0;
        trans[i] = i;
    }

    for( int i = 0; i < NUM_T_POINT_MAX; i++ ){
        t_points[i].src_x = t_points[i].x = SPECTRUM_LENGTH * i / NUM_T_POINT_MAX + SPECTRUM_LENGTH / NUM_T_POINT_MAX / 2;
        t_points[i].select = false;
    }

    this->Refresh();
}
