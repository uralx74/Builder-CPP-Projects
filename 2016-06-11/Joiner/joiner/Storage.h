//---------------------------------------------------------------------------
#ifndef StorageH
#define StorageH


#include <vector>
#include "Ora.hpp"
#include "dbf.hpp"
#include "Dbf_Lang.hpp"
#include <Db.hpp>
#include <ComCtrls.hpp>
#include "DBAccess.hpp"
#include "MemDS.hpp"


typedef enum {
    SS_EMPTY = 0,
    SS_FILE_NOT_FOUND,  // SS_TABLE_NOT_FOUND
    SS_FILE_IS_OPENED,
    SS_FIELD_NOT_FOUND
} TSTORAGESTATUS;

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
} TDBaseField;

// Структура для хранения параметров Полей процедуры
typedef struct {    // Для описания структуры dbf-файла
    String name;    // Имя поля (до 10 символов).
    bool active;        // Признак необходимости заполненеия поля (если false, поле создается, но не заполняется)
    bool enable;        // Признак того, необходимо ли вообще учитывать это поле (если false, поле не создается)
    String name_src;    // Имя поля из копируемой БД
} TOraProcField;


// Возможно обьединить эти структуры в одну
// Структура для хранения таблицы
typedef struct {
    AnsiString Server;
    AnsiString Username;
    AnsiString Password;
    AnsiString Table;     // Имя таблицы
} TOraTableTable;

// Структура для хранения параметров
typedef struct {
    AnsiString Server;
    AnsiString Username;
    AnsiString Password;
    AnsiString Procedure;   // Имя процедуры
    AnsiString Table;       // Имя таблицы (используется если нужно предварительно очистить)
    bool Truncate;
} TOraProcTable;

// Структура для хранения параметров
typedef struct {
    AnsiString Server;
    AnsiString Username;
    AnsiString Password;
    AnsiString Sql;     // Тект Sql-запроса
} TOraSqlTable;


//---------------------------------------------------------------------------
// class TStorage
//---------------------------------------------------------------------------

class TStorage
{
public:
    TStorage();
    //virtual void Create() = 0;
    virtual bool Open(bool ReadOnly = true) {};
    virtual Variant Get(AnsiString Field) {};
    //virtual void Set(AnsiString Field, Variant Value) = 0;
    virtual void Set(Variant Value) {};
    virtual void Commit() {};
    virtual void Append() {};   // Добавляет пустую запись перед (используется в DBase)
    virtual void Post() {};     // Фиксирует запись (используется в OraProc)
    virtual void Close() = 0;

    virtual bool Eot();     // End Of Tables
    virtual bool Eor() {};     // End Of Records
    virtual bool Eof();     // End Of Fields

    virtual void NextTable();
    virtual void NextRecord() {};
    virtual void NextField();

    virtual AnsiString GetSrcField() = 0;
    virtual bool IsActiveField() = 0;

    // Информационные функции
    virtual AnsiString GetTable() = 0;     // Возвращает наименование активного источника/приемника данных
    //virtual AnsiString GetRecord() = 0;     //
    virtual TSTORAGESTATUS GetStatus() { return StorageStatus;};     //



    //virtual void dial(char *number) { cout << «????? ?????? » << number << endl; }

protected:
    int TableIndex;
    int TableCount;
    int FieldCount;
    int FieldIndex;
    int RecordCount;
    int RecordIndex;

    TSTORAGESTATUS StorageStatus;
};



//---------------------------------------------------------------------------
// class TStorageDbase: TStorage
// Класс объекта источника/приемника данных
// Используется файл DBase4
//---------------------------------------------------------------------------
class TStorageDbase: TStorage
{
public:
    TStorageDbase();
    void Create();
    bool Open(bool ReadOnly = true);
    Variant Get(AnsiString Field);
    void Set(Variant Value);
    void Commit();
    void Append();
    void Close();

    //bool Eot();     // End Of Tables
    bool Eor();     // End Of Records
    //bool Eof();     // End Of Fields

    void NextTable();
    void NextRecord();
    //void NextField();

    AnsiString GetSrcField();
    bool IsActiveField();


    void AddTable(AnsiString FileName);

    AnsiString GetTable();

    std::vector<TDBaseField> Fields;
private:
    //AnsiString file;
    std::vector<AnsiString> Tables;    // Список полей для экспрта в файл DBF
    TDbf* pTable;
};








//---------------------------------------------------------------------------
// class TStorageOra
// base class for Oracle source/destination
//---------------------------------------------------------------------------

class TStorageOra: public TStorage
{
public:
 /*   virtual void Create() = 0;
    virtual void Open() {};
    virtual Variant Get(AnsiString Field) {};
    //virtual void Set(AnsiString Field, Variant Value) = 0;
    virtual void Set(Variant Value) = 0;
    virtual void Commit() {};
    virtual void Append() {};   // Добавляет пустую запись перед (используется в DBase)
    virtual void Post() {};     // Фиксирует запись (используется в OraProc)
    virtual void Close() = 0;

    virtual bool Eot() {};     // End Of Tables
    virtual bool Eor() {};     // End Of Records
    virtual bool Eof() = 0;     // End Of Fields

    virtual void NextTable() {};
    virtual void NextRecord() {};
    virtual void NextField() = 0;

    virtual AnsiString GetSrcField() = 0;
    virtual bool IsActiveField() = 0;

    // Информационные функции
    virtual AnsiString GetTable() = 0;     // Возвращает наименование активного источника/приемника данных
    //virtual AnsiString GetRecord() = 0;     //

*/

    //virtual void dial(char *number) { cout << «????? ?????? » << number << endl; }
    void Close();

    //AnsiString Server;
    //AnsiString Username;    // Имя пользователя БД
    //AnsiString Password;    // Пароль пользователя БД

protected:
    void OpenConnection(AnsiString Server, AnsiString Username, AnsiString Password);
    TOraSession* OraSession;
    TOraQuery* OraQuery;

};

//---------------------------------------------------------------------------
// class TStorageOraProc: TStorage
// Класс объекта приемника данных
// Используется процедура в БД Oracle
//---------------------------------------------------------------------------
class TStorageOraProc: public TStorageOra
{
public:
    TStorageOraProc();
    ~TStorageOraProc();

    //void Create();
    bool Open(bool ReadOnly = true);
    //Variant Get(AnsiString Field);

    void Set(Variant Value);
    //void Commit();
    void Post();

    //bool Eot();     // End Of Tables
    //bool Eor();     // End Of Records
    bool Eof();     // End Of Fields

    //void NextTable();
    //void NextRecord();
    void NextField();

    AnsiString GetSrcField();
    bool IsActiveField();
    void AddTable(const TOraProcTable& Table);
    AnsiString GetTable();



    std::vector<TOraProcField> Fields;      // Список полей процедуры

private:
    std::vector<TOraProcTable> Tables;    // Список полей для экспрта в файл DBF

    //int TableIndex;
    //int FieldIndex;

};





//---------------------------------------------------------------------------
// class TStorageOraTable: TStorage
// Класс объекта источника/приемника данных
// Используется таблица в БД Oracle
//---------------------------------------------------------------------------
class TStorageOraTable: public TStorageOra

{
public:
    TStorageOraTable();
    ~TStorageOraTable();

    //void Create();
    bool Open(bool ReadOnly);
    Variant Get(AnsiString Field);

    void Set(Variant Value);
    void Append();
    void Commit();
    //void Post();
    //void Close();

    //bool Eot();     // End Of Tables
    bool Eor();     // End Of Records
    //bool Eof();     // End Of Fields

    //void NextTable();
    void NextRecord();
    //void NextField();


    AnsiString GetSrcField();
    bool IsActiveField();

    void AddTable(const TOraTableTable &Table);

    AnsiString GetTable();

 

    //bool Truncate;
    AnsiString Table;


    std::vector<TOraProcField> Fields;

private:
    std::vector<TOraTableTable> Tables;    // Список полей для экспрта в файл DBF
};




//---------------------------------------------------------------------------
// class TStorageOraSql: TStorage
// Класс объекта источника/приемника данных
// Используется таблица в БД Oracle
//---------------------------------------------------------------------------
class TStorageOraSql: public TStorageOra
{
public:
    TStorageOraSql();
    ~TStorageOraSql();

    //void Create();
    bool Open(bool ReadOnly = true);

    Variant Get(AnsiString Field);

    //void Set(Variant Value);
    void Commit();
    void Post();

    bool Eor();     // End Of Records

    void NextTable();
    void NextRecord();

    AnsiString GetSrcField();
    bool IsActiveField();


    void AddTable(const TOraSqlTable &Table);

    AnsiString GetTable();
                
    //bool Truncate;

    std::vector<TOraProcField> Fields;

private:
    std::vector<TOraSqlTable> Tables;    // Список полей для экспрта в файл DBF
};



//---------------------------------------------------------------------------
#endif
