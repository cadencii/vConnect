#pragma once

#include "../stand/vConnect.h"
#include "../stand/WaveOut.h"

/* windows.h対策 */
#ifdef GetCurrentDirectory
#undef GetCurrentDirectory
#endif

#define NUM_SPECTRUM_MAX 256
#define SPECTRUM_LENGTH 1024
#define SPECTRUM_MAX_LENGTH 4096

#define NUM_T_POINT_MAX 16

namespace vtdEditor {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    struct editor_point{
        int   src_x;
        float x;
        float y;
        bool  select;
    };
    
    enum DRAW_TYPE{
        DRAW_NONE = 0,
        DRAW_FILTER,
        DRAW_TRANS
    };

    /// <summary>
    /// wspEditor の概要
    ///
    /// 参考スペクトルを見ながらスペクトルを変形する関数をデザインするツールです。
    /// このエディタで出来上がる.vtdデータはv.Connect-STANDでBRI/CLEとして使用できるようにする予定です。
    ///
    /// </summary>
    public ref class wspEditor : public System::Windows::Forms::Form
    {

    // 以下プロトタイプ宣言
    public: wspEditor();
    protected: ~wspEditor();
    protected: bool make_sure_exit( void );
    protected: void draw_spectrum( Object^ sender, PaintEventArgs^ e ); /* 実際の描画 */
    protected: bool on_import_select( void );
    protected: bool on_add_select( void );
    protected: bool on_save_select( void );
    protected: void on_reset_select( void );
    private: bool read_spectrum_file( String^ input, double* target, double* target_ap, double* target_f0 );
    private: bool write_vtd_file( String^ output );

    private: double** spectra;
    private: double*  src_spectrum;
    private: double*  src_aperiodicity;
    private: double   src_min, src_max, src_f0;
    private: int      num_spectra;
    private: double*  filter;
    private: double*  trans;
    private: int      trans_index;

    private: editor_point* t_points;

    private: DRAW_TYPE draw_type;

    // グラフの線の描画に使用するpen達
    private: Pen    ^_pen_grid;
    private: Pen    ^_pen_x_axis;
    private: Pen    ^_pen_spec_reference;
    private: Pen    ^_pen_spec_original;
    private: Pen    ^_pen_vtd;
    private: Pen    ^_pen_filter;
    private: SolidBrush ^_brush_tpoints;
    private: SolidBrush ^_brush_tpoints_select;

    // グラフ描画時に使うバッファ
    private: array<System::Drawing::Point>  ^_buf_points;

    private: System::Windows::Forms::Button^  buttonOpen;
    private: System::Windows::Forms::Button^  buttonSave;
    private: System::Windows::Forms::Button^  buttonExit;
    private: System::Windows::Forms::Button^  buttonAdd;
    private: System::Windows::Forms::Button^  buttonRemove;
    private: System::Windows::Forms::PictureBox^  pictureBox1;
    private: System::Windows::Forms::MenuStrip^  menuStrip1;
    private: System::Windows::Forms::ToolStripMenuItem^  menuFile;
    private: System::Windows::Forms::ToolStripMenuItem^  menuFileOpen;
    private: System::Windows::Forms::ToolStripMenuItem^  menuFileExit;
    private: System::Windows::Forms::Button^  buttonPlay;
    private: System::Windows::Forms::ToolStripMenuItem^  menuFileImport;
    private: System::Windows::Forms::Button^  buttonImport;
    private: System::Windows::Forms::Button^  button1;

    protected: 

    private:
        /// <summary>
        /// 必要なデザイナ変数です。
        /// </summary>
        System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
        /// <summary>
        /// デザイナ サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディタで変更しないでください。
        /// </summary>
        void InitializeComponent(void)
        {
            this->buttonOpen = (gcnew System::Windows::Forms::Button());
            this->buttonSave = (gcnew System::Windows::Forms::Button());
            this->buttonExit = (gcnew System::Windows::Forms::Button());
            this->buttonAdd = (gcnew System::Windows::Forms::Button());
            this->buttonRemove = (gcnew System::Windows::Forms::Button());
            this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
            this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
            this->menuFile = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->menuFileOpen = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->menuFileImport = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->menuFileExit = (gcnew System::Windows::Forms::ToolStripMenuItem());
            this->buttonPlay = (gcnew System::Windows::Forms::Button());
            this->buttonImport = (gcnew System::Windows::Forms::Button());
            this->button1 = (gcnew System::Windows::Forms::Button());
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
            this->menuStrip1->SuspendLayout();
            this->SuspendLayout();
            // 
            // buttonOpen
            // 
            this->buttonOpen->Location = System::Drawing::Point(1056, 40);
            this->buttonOpen->Name = L"buttonOpen";
            this->buttonOpen->Size = System::Drawing::Size(96, 24);
            this->buttonOpen->TabIndex = 0;
            this->buttonOpen->Text = L"Open";
            this->buttonOpen->UseVisualStyleBackColor = true;
            // 
            // buttonSave
            // 
            this->buttonSave->Location = System::Drawing::Point(1056, 104);
            this->buttonSave->Name = L"buttonSave";
            this->buttonSave->Size = System::Drawing::Size(96, 24);
            this->buttonSave->TabIndex = 2;
            this->buttonSave->Text = L"Save";
            this->buttonSave->UseVisualStyleBackColor = true;
            this->buttonSave->Click += gcnew System::EventHandler(this, &wspEditor::buttonSave_Click);
            // 
            // buttonExit
            // 
            this->buttonExit->Location = System::Drawing::Point(1056, 200);
            this->buttonExit->Name = L"buttonExit";
            this->buttonExit->Size = System::Drawing::Size(96, 24);
            this->buttonExit->TabIndex = 5;
            this->buttonExit->Text = L"Exit";
            this->buttonExit->UseVisualStyleBackColor = true;
            this->buttonExit->Click += gcnew System::EventHandler(this, &wspEditor::buttonExit_Click);
            // 
            // buttonAdd
            // 
            this->buttonAdd->Location = System::Drawing::Point(1056, 136);
            this->buttonAdd->Name = L"buttonAdd";
            this->buttonAdd->Size = System::Drawing::Size(96, 24);
            this->buttonAdd->TabIndex = 3;
            this->buttonAdd->Text = L"Add";
            this->buttonAdd->UseVisualStyleBackColor = true;
            this->buttonAdd->Click += gcnew System::EventHandler(this, &wspEditor::buttonAdd_Click);
            // 
            // buttonRemove
            // 
            this->buttonRemove->Location = System::Drawing::Point(1056, 168);
            this->buttonRemove->Name = L"buttonRemove";
            this->buttonRemove->Size = System::Drawing::Size(96, 24);
            this->buttonRemove->TabIndex = 4;
            this->buttonRemove->Text = L"Remove";
            this->buttonRemove->UseVisualStyleBackColor = true;
            // 
            // pictureBox1
            // 
            this->pictureBox1->BackColor = System::Drawing::Color::White;
            this->pictureBox1->Location = System::Drawing::Point(16, 32);
            this->pictureBox1->Name = L"pictureBox1";
            this->pictureBox1->Size = System::Drawing::Size(1024, 768);
            this->pictureBox1->TabIndex = 5;
            this->pictureBox1->TabStop = false;
            this->pictureBox1->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &wspEditor::pictureBox1_MouseMove);
            this->pictureBox1->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &wspEditor::pictureBox1_MouseDown);
            this->pictureBox1->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &wspEditor::pictureBox1_MouseUp);
            // 
            // menuStrip1
            // 
            this->menuStrip1->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
            this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->menuFile});
            this->menuStrip1->Location = System::Drawing::Point(0, 0);
            this->menuStrip1->Name = L"menuStrip1";
            this->menuStrip1->Size = System::Drawing::Size(1168, 24);
            this->menuStrip1->TabIndex = 6;
            this->menuStrip1->Text = L"menuStrip1";
            // 
            // menuFile
            // 
            this->menuFile->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->menuFileOpen, 
                this->menuFileImport, this->menuFileExit});
            this->menuFile->Name = L"menuFile";
            this->menuFile->Size = System::Drawing::Size(35, 20);
            this->menuFile->Text = L"File";
            // 
            // menuFileOpen
            // 
            this->menuFileOpen->Name = L"menuFileOpen";
            this->menuFileOpen->Size = System::Drawing::Size(106, 22);
            this->menuFileOpen->Text = L"Open";
            // 
            // menuFileImport
            // 
            this->menuFileImport->Name = L"menuFileImport";
            this->menuFileImport->Size = System::Drawing::Size(106, 22);
            this->menuFileImport->Text = L"Import";
            this->menuFileImport->Click += gcnew System::EventHandler(this, &wspEditor::menuFileImport_Click);
            // 
            // menuFileExit
            // 
            this->menuFileExit->Name = L"menuFileExit";
            this->menuFileExit->Size = System::Drawing::Size(106, 22);
            this->menuFileExit->Text = L"Exit";
            this->menuFileExit->Click += gcnew System::EventHandler(this, &wspEditor::menuFileExit_Click);
            // 
            // buttonPlay
            // 
            this->buttonPlay->Location = System::Drawing::Point(1056, 256);
            this->buttonPlay->Name = L"buttonPlay";
            this->buttonPlay->Size = System::Drawing::Size(96, 24);
            this->buttonPlay->TabIndex = 6;
            this->buttonPlay->Text = L"Play";
            this->buttonPlay->UseVisualStyleBackColor = true;
            this->buttonPlay->Click += gcnew System::EventHandler(this, &wspEditor::buttonPlay_Click);
            // 
            // buttonImport
            // 
            this->buttonImport->Location = System::Drawing::Point(1056, 72);
            this->buttonImport->Name = L"buttonImport";
            this->buttonImport->Size = System::Drawing::Size(96, 24);
            this->buttonImport->TabIndex = 1;
            this->buttonImport->Text = L"Import";
            this->buttonImport->UseVisualStyleBackColor = true;
            this->buttonImport->Click += gcnew System::EventHandler(this, &wspEditor::buttonImport_Click);
            // 
            // button1
            // 
            this->button1->Location = System::Drawing::Point(1056, 352);
            this->button1->Name = L"button1";
            this->button1->Size = System::Drawing::Size(96, 24);
            this->button1->TabIndex = 7;
            this->button1->Text = L"Reset";
            this->button1->UseVisualStyleBackColor = true;
            this->button1->Click += gcnew System::EventHandler(this, &wspEditor::button1_Click);
            // 
            // wspEditor
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->BackgroundImageLayout = System::Windows::Forms::ImageLayout::None;
            this->ClientSize = System::Drawing::Size(1168, 800);
            this->Controls->Add(this->button1);
            this->Controls->Add(this->buttonImport);
            this->Controls->Add(this->buttonPlay);
            this->Controls->Add(this->pictureBox1);
            this->Controls->Add(this->buttonRemove);
            this->Controls->Add(this->buttonAdd);
            this->Controls->Add(this->buttonExit);
            this->Controls->Add(this->buttonSave);
            this->Controls->Add(this->buttonOpen);
            this->Controls->Add(this->menuStrip1);
            this->MainMenuStrip = this->menuStrip1;
            this->Name = L"wspEditor";
            this->Text = L"vtdEditor";
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
            this->menuStrip1->ResumeLayout(false);
            this->menuStrip1->PerformLayout();
            this->ResumeLayout(false);
            this->PerformLayout();

        }
#pragma endregion
private: System::Void buttonExit_Click(System::Object^  sender, System::EventArgs^  e) {
             if( make_sure_exit() )
                 Close();
         }
private: System::Void menuFileExit_Click(System::Object^  sender, System::EventArgs^  e) {
             if( make_sure_exit() )
                 Close();
         }
private: System::Void buttonImport_Click(System::Object^  sender, System::EventArgs^  e) {
             on_import_select();
         }
private: System::Void menuFileImport_Click(System::Object^  sender, System::EventArgs^  e) {
             on_import_select();
         }
private: System::Void buttonAdd_Click(System::Object^  sender, System::EventArgs^  e) {
             on_add_select();
         }
private: System::Void buttonSave_Click(System::Object^  sender, System::EventArgs^  e) {
             on_save_select();
         }
private: System::Void pictureBox1_MouseMove( System::Object^ sender, System::Windows::Forms::MouseEventArgs^  e );
private: System::Void pictureBox1_MouseDown( System::Object^ sender, System::Windows::Forms::MouseEventArgs^  e );
private: System::Void pictureBox1_MouseUp( System::Object^ sender, System::Windows::Forms::MouseEventArgs^  e );
private: System::Void buttonPlay_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
             on_reset_select();
         }
};
}

