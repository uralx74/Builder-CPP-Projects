//---------------------------------------------------------------------------

#ifndef FQueryEditH
#define FQueryEditH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "fmain.h"
#include <ImgList.hpp>
#include <Grids.hpp>
//---------------------------------------------------------------------------
class TFormQueryEdit : public TForm
{
__published:	// IDE-managed Components
    TBevel *Bevel2;
    TLabel *Label1;
    TGroupBox *GroupBox1;
    TListView *QueryLV;
    TGroupBox *GroupBox2;
    TListView *TabLV;
    TBitBtn *BitBtn5;
    TBitBtn *BitBtn6;
    TBitBtn *BitBtn7;
    TBitBtn *BitBtn8;
    TBitBtn *TabDownBitBtn;
    TBitBtn *TabUpBitBtn;
    TBitBtn *TabAddBitBtn;
    TBitBtn *TabDelBitBtn;
    TGroupBox *GroupBox3;
    TListBox *ParametersLB;
    TBitBtn *ParamUpBitBtn;
    TBitBtn *ParamDownBitBtn;
    TBitBtn *BitBtn15;
    TRichEdit *SqlTextREdt;
    TButton *Button1;
    TBitBtn *BitBtn9;
    TBitBtn *ParamAddBitBtn;
    TEdit *VarNameEdt;
    TComboBox *VarTypeCB;
    TComboBox *VarValueCB;
    TBitBtn *ParamDelBitBtn;
    TLabel *Label2;
    TLabel *Label3;
    TLabel *Label4;
    TComboBox *VarDBIndexCB;
    TComboBox *VarValueFormatCB;
    TLabel *Label5;
    TEdit *VarDispNameEdt;
    TCheckBox *VarVisibleChkBox;
    TLabel *Label6;
    TLabel *Label7;
    TBitBtn *VarShowCodeBtn;
    TImageList *ImageList1;
    TEdit *QueryNameEdt;
    TLabel *Label9;
    TCheckBox *CheckBox1;
    TLabel *Label8;
    TComboBox *ComboBox1;
    TListBox *ListBox1;
    TStringGrid *VarListValSGrid;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall TabLVClick(TObject *Sender);
    void __fastcall QueryLVClick(TObject *Sender);
    void __fastcall BitBtn15Click(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall VarShowCodeBtnClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall ParametersLBClick(TObject *Sender);
private:	// User declarations
    enum {chNone = 0, chRewrite, chAdd, chDel};
    void __fastcall ChangeRewrite(int itemindex);

    std::vector<int> ChangeList;           // Вектор действия с записями

    std::vector<TABITEM> TabList;           // Вектор разделов
    std::vector<QUERYITEM> QueryList;       // Вектор строк заросов

    std::vector<TABITEM>* pTabList;           // Вектор разделов
    TABITEM* pCurrentTab;           // Текущий разделов

public:		// User declarations
    __fastcall TFormQueryEdit(TComponent* Owner);
    void FillQueryLV();
    void FillTabLV();
    void FillOptions();
    void FillParameters();

    //std::vector<TABITEM>* pTabList;           // Вектор разделов

    //void __fastcall TFormQueryEdit::ShowBalloonTip(TWinControl *Control,int  Icon,char *Title,char *Text,TColor BackCL,TColor TextCL);
    //void ShowBalloonTip(HWND hCtrl, int Icon, LPCWSTR Title, LPCWSTR Text);


};
//---------------------------------------------------------------------------
extern PACKAGE TFormQueryEdit *FormQueryEdit;
//---------------------------------------------------------------------------
#endif
