//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "TransferModule.h"
#include "Logger.h"

/*
//---------------------------------------------------------------------------
// ��������� ��� �������� ���������� ���� (�������) DBASE
typedef struct {    // ��� �������� ��������� dbf-�����
    String type;    // ��� fieldtype is a single character [C,D,F,L,M,N]
    String name;    // ��� ���� (�� 10 ��������).
    int length;         // ����� ����
    int decimals;       // ����� ���������� �����
    bool active;        // ������� ������������� ����������� ���� (���� false, ���� ���������, �� �� �����������)
    bool enable;        // ������� ����, ���������� �� ������ ��������� ��� ���� (���� false, ���� �� ���������)
    String name_src;    // ��� ���� �� ���������� ��
    // Character 	 1-255
    // Date	  8
    // Logical	  1
    // Memo	  10
    // Numeric	1-30
    // Decimals is 0 for non-numeric, number of decimals for numeric.
} DBASEFIELD;

// ��������� ��� �������� ���������� �������� � DBF
typedef struct {    // ��� �������� ������� ����� � Excel
    AnsiString id;
    AnsiString label;
    AnsiString file;
    //bool fAllowUnassignedFields;
    std::vector<DBASEFIELD> Fields;    // ������ ����� ��� ������� � ���� DBF
} TExportParamsDbase;


//---------------------------------------------------------------------------
// ��������� ��� �������� ���������� ���� (�������) DBASE
typedef struct {    // ��� �������� ��������� dbf-�����
    //String type;    // ��� fieldtype is a single character [C,D,F,L,M,N]
    String name;    // ��� ���� (�� 10 ��������).
    //int length;         // ����� ����
    //int decimals;       // ����� ���������� �����
    bool active;        // ������� ������������� ����������� ���� (���� false, ���� ���������, �� �� �����������)
    bool enable;        // ������� ����, ���������� �� ������ ��������� ��� ���� (���� false, ���� �� ���������)
    String name_src;    // ��� ���� �� ���������� ��
} TOraProcField;

// ��������� ��� �������� ���������� �������� � Oracle � ������ ���������
typedef struct {    // ��� �������� ������� ����� � Excel
    AnsiString server;
    AnsiString username;
    AnsiString password;
    AnsiString procedure;   // ���������, ����� ������� ������������ �������� ������
    AnsiString table;   // ����������� ������� (�������������). ������������ ��� truncate=true
    bool truncate;
    std::vector<TOraProcField> Fields;    // ������ ����� (����������) ��������� � ��������������� ����� � ���������
} TExportParamsOraProc;

//---------------------------------------------------------------------------
// ��������� ��� �������� ���������� ���� (�������) DBASE
typedef struct {    // ��� �������� ��������� dbf-�����
    String type;    // ��� fieldtype is a single character [C,D,F,L,M,N] VARCHAR2(255)
    String name;    // ��� ���� (�� 10 ��������).
    //int length;         // ����� ����
    //int decimals;       // ����� ���������� �����
    bool active;        // ������� ������������� ����������� ���� (���� false, ���� ���������, �� �� �����������)
    bool enable;        // ������� ����, ���������� �� ������ ��������� ��� ���� (���� false, ���� �� ���������)
    String name_src;    // ��� ���� �� ���������� ��
} TOraTableField;

// ��������� ��� �������� ���������� �������� � Oracle � ������ ���������
typedef struct {    // ��� �������� ������� ����� � Excel
    AnsiString server;
    AnsiString username;
    AnsiString password;
    AnsiString table;   // ����������� �������

	AnsiString tablespace;  // USERS
	AnsiString pctfree;     // 10
	AnsiString initrans;    // 1
	AnsiString maxtrans;    // 255
	AnsiString initial;     //64K
	AnsiString minextents;  // 1
	AnsiString maxextents;  // unlimited

    bool truncate;
    std::vector<TOraProcField> Fields;    // ������ ����� ��� ������� � ���� DBF
} TExportParamsOraTable;

//---------------------------------------------------------------------------
//
typedef struct {    // ��� �������� ������� ����� � Excel
    //AnsiString type;
    AnsiString name;
} TImportDbaseFile;

typedef struct {    //
    std::vector<TImportDbaseFile> Files;    // ������ ������ ��� �������
} TImportParamsDbase;
*/

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TButton *Button1;
    TStatusBar *StatusBar1;
    TButton *Button2;
    TRichEdit *RichEdit1;
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
    void __fastcall SetStatusText(AnsiString sStatus);
    TTransferModule TransferModule;
};

//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
