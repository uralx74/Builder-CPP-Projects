//---------------------------------------------------------------------------
#ifndef StorageOraH
#define StorageOraH

#include <vector>
#include <map>
#include "Ora.hpp"
#include "DBAccess.hpp"
#include "MemDS.hpp"
#include "Storage.h"


// Структура для хранения параметров полей в Oracle (процедуры/таблицы)
class TOraField : public TStorageField {
};

class TStorageTableOra : public TStorageTable
{
public:
    AnsiString Table;       // Имя таблицы (используется если нужно предварительно очистить)
    AnsiString Server;
    AnsiString Username;
    AnsiString Password;
};



// Структура для хранения параметров
class TOraProcTable : public TStorageTableOra
{
public:
    AnsiString Procedure;   // Имя процедуры
};

// Структура для хранения параметров
class TOraSqlTable : public TStorageTableOra
{
public:
    AnsiString Sql;     // Имя файла с Sql-запросом
};





//---------------------------------------------------------------------------
// class TStorageOra
// base class for Oracle source/destination
//---------------------------------------------------------------------------

class TStorageOra: public TStorage
{
protected:
    void Close();

protected:
    ~TStorageOra();

    void OpenConnection(AnsiString Server, AnsiString Username, AnsiString Password);
    TOraSession* dbSession;
    TOraQuery* dbQuery;
    void TruncateTable(TStorageTableOra* Table);
    void PrepareQuery();


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
    void Open(bool ReadOnly = true);
    //Variant Get(AnsiString Field);
    void SetFieldValue(Variant Value);
    void Post();
    void NextField();
    //AnsiString GetSrcField();
    //bool IsActiveField();
    TOraField* AddField();
    //void AddField(const TOraField& Field);
    void AddTable(const TOraProcTable& Table);
    AnsiString GetTable();
private:
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
    void Open(bool ReadOnly = true);
    //Variant Get(AnsiString Field);      // Возвращает значение по имени поля
    Variant GetFieldValue(TStorageField* Field);
    //void Set(Variant Value);      // Устанавливает значение активного поля
    void SetFieldValue(Variant Value);      // Устанавливает значение активного поля
    void Append();
    void Commit();
    void Post();
    bool Eor();     // End Of Records
    void NextTable();
    void NextRecord();
    //AnsiString GetSrcField();
    //bool IsActiveField();
    TOraField* AddField();
    void AddTable(const TOraSqlTable &Table);
    AnsiString GetTable();
    //bool FindField(AnsiString fieldName);

private:
    std::vector<TOraSqlTable> Tables;    // Список полей для экспрта в файл DBF
};







//---------------------------------------------------------------------------
#endif
