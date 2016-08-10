//---------------------------------------------------------------------------
#ifndef StorageTextH
#define StorageTextH

#include <vector>
#include "Storage.h"

//---------------------------------------------------------------------------
// Модуль объекта приемника данных TStorageSqlText
//
// Автор: В.С.Овчинников
// 2016-07-26
//
// Назначение:
// Для осуществления замены полей в текстовом файле, преимущественно в
// текстофом файле с запросом на языке sql.
//
// Описание:
// В качестве хранилища выступает текстовый файл.
// В качестве полей в этом файле выступают именованные фрагменты в тексте,
// заключенные в /** **/ и начинающиеся с символа (:).
// Например:
//   /**:field_name**/
//
// Характеристики:
// Максимальное количество таблиц неограничено = 1 (в дальнейшем необходимо обойти это ограничение)
// Максимальное и минимальное количество RecordCount = 1
// Использование в качестве источника: Нет
// Использование в качестве приемника: Да
//
// Примечания:
// 1. Требуется доработка и тестирование
// 2. Необходимо реализовать возможность экспорта в несколько таблиц (2016-07-26 не реализовано)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// Структура для хранения полей в MS Excel
//---------------------------------------------------------------------------
class TSqlTextField : public TStorageField {    // Для описания структуры dbf-файла
public:
};

//---------------------------------------------------------------------------
// Структуры для хранения таблиц
//---------------------------------------------------------------------------
class TSqlTextTable : public TStorageTable
{
public:
    AnsiString File;
    AnsiString Template;
};

//---------------------------------------------------------------------------
// class TStorageSqlText: TStorage
//---------------------------------------------------------------------------

class TStorageSqlText: TStorage
{
protected:
    void Close();
    
public:
    TStorageSqlText(){};
    ~TStorageSqlText();
    void Open(bool ReadOnly = true);
    //Variant GetFieldValue(TStorageField* Field) {};
    void SetFieldValue(Variant Value);
    void Commit();

    //bool Eot();     // End Of Tables
    //bool Eor();     // End Of Records
    //bool Eof();     // End Of Fields

    void NextTable() {};
    void NextRecord() {};
    void Append() { FieldIndex = 0;};

    //void NextField();
    //void NextField();

    TSqlTextField* AddField();
    void AddTable(const TSqlTextTable& Table);

    AnsiString GetTable();

    //CopyFieldsFrom(TStorage* storage);

private:
    //CopyFieldsToExcel(TStorage* storage);
    std::vector<TSqlTextTable> Tables;    // Список полей для экспрта
    AnsiString SqlText;

    bool hasChangedAfterAppend;

};


//---------------------------------------------------------------------------
#endif
