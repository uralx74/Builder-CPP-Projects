//---------------------------------------------------------------------------
#ifndef StorageExcelH
#define StorageExcelH

#include <vector>
#include <map>
//#include "MemDS.hpp"
#include "Storage.h"

#include "c:\PROGRS\current\util\MSExcelWorks.h"


class TStorageExcel;


//---------------------------------------------------------------------------
// Структура для хранения полей в MS Excel
class TExcelField : public TStorageField {    // Для описания структуры dbf-файла
public:
    String format;      // Формат ячейки в MS Excel
};

// Структура для хранения параметров
class TExcelTable : public TStorageTable
{
public:
    AnsiString File;
};

//---------------------------------------------------------------------------
// class TStorageExcel: TStorage
// Класс объекта источника/приемника данных
// Используется таблица, хранимая в файле MS Excel
//---------------------------------------------------------------------------

class TStorageExcel: TStorage
{
protected:
    void Close();

public:
    TStorageExcel(){};
    ~TStorageExcel();
    void Open(bool ReadOnly = true);
    Variant GetFieldValue(TStorageField* Field);
    void SetFieldValue(Variant Value);
    void Commit();
    void Append();


    //bool Eot();     // End Of Tables
    bool Eor();     // End Of Records
    //bool Eof();     // End Of Fields

    //void NextTable();
    void NextRecord();
    //void NextField();


    TExcelField* AddField();
    void AddTable(const TExcelTable& Table);

    AnsiString GetTable();

    CopyFieldsFrom(TStorage* storage);

private:
    CopyFieldsToExcel(TStorage* storage);

    std::vector<TExcelTable> Tables;    // Список полей для экспрта

    MSExcelWorks* msexcel;
    Variant Worksheet;
    Variant Workbook;
    Variant Range;
    std::map<AnsiString, int> FieldsList;   // Список полей
};



//---------------------------------------------------------------------------
#endif
