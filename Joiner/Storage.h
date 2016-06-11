//---------------------------------------------------------------------------
// Storage.h
// author: vsovchinnikov
//---------------------------------------------------------------------------
#ifndef StorageH
#define StorageH

#include <vector>
#include <map>
#include "Ora.hpp"
#include "dbf.hpp"
#include "Dbf_Lang.hpp"
#include <Db.hpp>
#include <ComCtrls.hpp>
#include "DBAccess.hpp"
#include "MemDS.hpp"
#include "c:\PROGRS\current\util\MSExcelWorks.h"
      

//---------------------------------------------------------------------------
// Структуры для полей
//---------------------------------------------------------------------------

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
} TDbaseField;

// Структура для хранения параметров полей в Oracle (процедуры/таблицы)
typedef struct {    // Для описания структуры dbf-файла
    String name;    // Имя поля (до 10 символов).
    bool active;        // Признак необходимости заполненеия поля (если false, поле создается, но не заполняется)
    bool enable;        // Признак того, необходимо ли вообще учитывать это поле (если false, поле не создается)
    String name_src;    // Имя поля из копируемой БД
} TOraField;

//---------------------------------------------------------------------------
// Структура для хранения полей в MS Excel
typedef struct {    // Для описания структуры dbf-файла
    String name;    // Имя поля (до 10 символов).
    bool active;        // Признак необходимости заполненеия поля (если false, поле создается, но не заполняется)
    bool enable;        // Признак того, необходимо ли вообще учитывать это поле (если false, поле не создается)
    String name_src;    // Имя поля из копируемой БД
    String format;      // Формат ячейки в MS Excel
} TExcelField;




//---------------------------------------------------------------------------
// Структуры для хранения таблиц
//---------------------------------------------------------------------------

// Структура для хранения параметров
typedef struct {
    AnsiString File;
    bool Truncate;
} TDbaseTable;

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
    AnsiString Sql;     // Имя файла с Sql-запросом
    AnsiString Table;   // Имя таблицы (используется если нужно предварительно очистить)
    bool Truncate;
} TOraSqlTable;

// Структура для хранения параметров
typedef struct {
    AnsiString File;
    bool Truncate;
} TExcelTable;

//---------------------------------------------------------------------------
// class TStorage
//---------------------------------------------------------------------------

class TStorage
{
public:
    TStorage();
    virtual void Open() {};
    virtual Variant Get(AnsiString Field) {};
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

    virtual AnsiString GetSrcField() = 0;   // Возвращает имя сопоставленного поля в источнике
    virtual bool IsActiveField() = 0;
    virtual bool IsActiveTable() { return Active; };
    virtual bool IsModified() { return Modified; };
    virtual void SetReadOnly(bool ReadOnlyFlag){ this->ReadOnly = ReadOnlyFlag; };
    virtual int GetRecordCount(){ return RecordCount; };
     
    // Информационные функции
    virtual AnsiString GetTable() = 0;     // Возвращает наименование активного хранилища данных
    virtual AnsiString GetTableStage();     // Возвращает текущий этап обработки данных
      
protected:
    int TableIndex;
    int TableCount;
    int FieldCount;
    int FieldIndex;
    int RecordCount;
    int RecordIndex;
    bool Active;    // Признак того, что источник/приемник открыт и готов к считыванию/записи данных
    bool ReadOnly;
    bool Modified;
    //TSTORAGESTATUS StorageStatus;
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
    void Open();
    Variant Get(AnsiString Field);
    void Set(Variant Value);
    void Commit();
    void Append();
    void Close();

    bool Eor();     // End Of Records

    void NextTable();
    void NextRecord();
    //void NextField();

    AnsiString GetSrcField();
    bool IsActiveField();

    void AddField(const TDbaseField& Field);
    void AddTable(const TDbaseTable& Table);

    AnsiString GetTable();
private:
    void Create();  // Создает файл DBF
    std::vector<TDbaseTable> Tables;    // Список полей для экспрта в файл DBF
    std::vector<TDbaseField> Fields;

    TDbf* pTable;   // Текущее хранилище
};



//---------------------------------------------------------------------------
// class TStorageOra
// base class for Oracle source/destination
//---------------------------------------------------------------------------

class TStorageOra: public TStorage
{
public:
    void Close();
    
protected:
    ~TStorageOra();

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
    TStorageOraProc() {};
    void Open();
    //Variant Get(AnsiString Field);
    void Set(Variant Value);
    void Post();
    void NextField();
    AnsiString GetSrcField();
    bool IsActiveField();
    void AddField(const TOraField& Field);
    void AddTable(const TOraProcTable& Table);
    AnsiString GetTable();
private:
    std::vector<TOraField> Fields;      // Список полей процедуры
    std::vector<TOraProcTable> Tables;    // Список таблиц (процедур)
};



//---------------------------------------------------------------------------
// class TStorageOraSql: TStorage
// Класс объекта источника/приемника данных
// Используется таблица в БД Oracle
//---------------------------------------------------------------------------
class TStorageOraSql: public TStorageOra
{
public:
    TStorageOraSql() {};
    void Open();
    Variant Get(AnsiString Field);      // Возвращает значение по имени поля
    void Set(Variant Value);      // Устанавливает значение активного поля
    void Append();
    void Commit();
    void Post();
    bool Eor();     // End Of Records
    void NextTable();
    void NextRecord();
    AnsiString GetSrcField();
    bool IsActiveField();
    void AddField(const TOraField& Field);
    void AddTable(const TOraSqlTable &Table);
    AnsiString GetTable();
private:
    std::vector<TOraField> Fields;
    std::vector<TOraSqlTable> Tables;    // Список полей для экспрта в файл DBF
};


//---------------------------------------------------------------------------
// class TStorageExcel: TStorage
// Класс объекта источника/приемника данных
// Используется таблица, хранимая в файле MS Excel
//---------------------------------------------------------------------------

class TStorageExcel: TStorage
{
public:
    TStorageExcel(){};
    ~TStorageExcel();
    void Open();
    Variant Get(AnsiString Field);
    void Set(Variant Value);
    void Commit();
    void Append();
    void Close();

    //bool Eot();     // End Of Tables
    bool Eor();     // End Of Records
    //bool Eof();     // End Of Fields

    //void NextTable();
    void NextRecord();
    //void NextField();


    AnsiString GetSrcField();
    bool IsActiveField() {};


    void AddField(const TExcelField& Field);
    void AddTable(const TExcelTable& Table);

    AnsiString GetTable();

private:
    void OpenExcel();

    std::vector<TExcelField> Fields;
    std::vector<TExcelTable> Tables;    // Список полей для экспрта

    MSExcelWorks* msexcel;
    Variant Worksheet;
    Variant Workbook;
    Variant Range;
    std::map<AnsiString, int> FieldsList;   // Список полей
             
};


//---------------------------------------------------------------------------
#endif
