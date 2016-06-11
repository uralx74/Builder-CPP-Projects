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
// Структура для хранения параметров поля (столбца) DBASE
typedef struct {    // Для описания структуры dbf-файла
    String type;    // Тип fieldtype is a single character [C,D,F,L,M,N]
    String name;    // Имя поля (до 10 символов).
    int length;         // Длина поля
    int decimals;       // Длина десятичной части
    bool active;        // Признак необходимости заполненеия поля (если false, поле создается, но не заполняется)
    bool enable;        // Признак того, необходимо ли вообще учитывать это поле (если false, поле не создается)
    String name_src;    // Имя поля из копируемой БД
    // Character 	 1-255
    // Date	  8
    // Logical	  1
    // Memo	  10
    // Numeric	1-30
    // Decimals is 0 for non-numeric, number of decimals for numeric.
} DBASEFIELD;

// Структура для хранения параметров экспорта в DBF
typedef struct {    // Для описания формата ячеек в Excel
    AnsiString id;
    AnsiString label;
    AnsiString file;
    //bool fAllowUnassignedFields;
    std::vector<DBASEFIELD> Fields;    // Список полей для экспрта в файл DBF
} TExportParamsDbase;


//---------------------------------------------------------------------------
// Структура для хранения параметров поля (столбца) DBASE
typedef struct {    // Для описания структуры dbf-файла
    //String type;    // Тип fieldtype is a single character [C,D,F,L,M,N]
    String name;    // Имя поля (до 10 символов).
    //int length;         // Длина поля
    //int decimals;       // Длина десятичной части
    bool active;        // Признак необходимости заполненеия поля (если false, поле создается, но не заполняется)
    bool enable;        // Признак того, необходимо ли вообще учитывать это поле (если false, поле не создается)
    String name_src;    // Имя поля из копируемой БД
} TOraProcField;

// Структура для хранения параметров экспорта в Oracle с помощь процедуры
typedef struct {    // Для описания формата ячеек в Excel
    AnsiString server;
    AnsiString username;
    AnsiString password;
    AnsiString procedure;   // Процедура, через которую производится загрузка данных
    AnsiString table;   // Заполняемая таблица (необязательно). Используется при truncate=true
    bool truncate;
    std::vector<TOraProcField> Fields;    // Список полей (параметров) процедуры и соответствующих полей в источнике
} TExportParamsOraProc;

//---------------------------------------------------------------------------
// Структура для хранения параметров поля (столбца) DBASE
typedef struct {    // Для описания структуры dbf-файла
    String type;    // Тип fieldtype is a single character [C,D,F,L,M,N] VARCHAR2(255)
    String name;    // Имя поля (до 10 символов).
    //int length;         // Длина поля
    //int decimals;       // Длина десятичной части
    bool active;        // Признак необходимости заполненеия поля (если false, поле создается, но не заполняется)
    bool enable;        // Признак того, необходимо ли вообще учитывать это поле (если false, поле не создается)
    String name_src;    // Имя поля из копируемой БД
} TOraTableField;

// Структура для хранения параметров экспорта в Oracle с помощь процедуры
typedef struct {    // Для описания формата ячеек в Excel
    AnsiString server;
    AnsiString username;
    AnsiString password;
    AnsiString table;   // Заполняемая таблица

	AnsiString tablespace;  // USERS
	AnsiString pctfree;     // 10
	AnsiString initrans;    // 1
	AnsiString maxtrans;    // 255
	AnsiString initial;     //64K
	AnsiString minextents;  // 1
	AnsiString maxextents;  // unlimited

    bool truncate;
    std::vector<TOraProcField> Fields;    // Список полей для экспрта в файл DBF
} TExportParamsOraTable;

//---------------------------------------------------------------------------
//
typedef struct {    // Для описания формата ячеек в Excel
    //AnsiString type;
    AnsiString name;
} TImportDbaseFile;

typedef struct {    //
    std::vector<TImportDbaseFile> Files;    // Список файлов для импорта
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
