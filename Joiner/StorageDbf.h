//---------------------------------------------------------------------------
#ifndef StorageDbfH
#define StorageDbfH

#include <vector>
#include <map>
#include "Ora.hpp"
#include "dbf.hpp"
#include "Dbf_Lang.hpp"
#include "DBAccess.hpp"
#include "MemDS.hpp"
#include "Storage.h"

class TStorageDbf;

//---------------------------------------------------------------------------
// Структура для хранения параметров поля (столбца) DBASE
class TDbaseField : public TStorageField {    // Для описания структуры dbf-файла
public:
    //TDbaseField() {};
    //TDbaseField(TDbaseField* Field);
    //TDbaseField(TStorageField* Field);

    char type;    // Тип fieldtype is a single character [C,D,F,L,M,N]
    int length;         // Длина поля
    int decimals;       // Длина десятичной части

    // Character 	 1-255
    // Date	  8
    // Logical	  1
    // Memo	  10
    // Numeric	1-30
    // Decimals is 0 for non-numeric, number of decimals for numeric.

};

// Структура для хранения параметров
class TDbaseTable: public TStorageTable
{
public:
    AnsiString File;
};


//---------------------------------------------------------------------------
// class TStorageDbase: TStorage
// Класс объекта источника/приемника данных
// Используется файл DBase4
//---------------------------------------------------------------------------
class TStorageDbase: public TStorage
{
protected:
    void Close();
public:
    TStorageDbase();
    TStorageDbase(String fileName);
    void Open(bool ReadOnly = true);
    Variant GetFieldValue(TStorageField* Field);
    void SetFieldValue(Variant Value);
    void Commit();
    void Append();


    bool Eor();     // End Of Records

    void NextTable();
    void NextRecord();
    //void NextField();

    TDbaseField* AddField();
    TStorageField* AddField(TStorageField* Field);
    void AddTable(const TDbaseTable& Table);

    std::vector<TStorageField> GetFieldDefs();
    void SetFieldDefs(std::vector<TStorageField>);

    AnsiString GetTable();

    void LoadFieldDefs();


    // Тестирование
    //CopyDbaseFields(TDbaseField* field);


    // Тестирование 2
    CopyFieldsFrom(TStorage* storage);
    CopyFieldsToDbf(TStorage* storage);



private:
    void Create();  // Создает файл DBF
    std::vector<TDbaseTable> Tables;    // Список таблиц
    TDbf* pTable;   // Текущее хранилище
};




//---------------------------------------------------------------------------
#endif
