#include "pch.h"
#pragma hdrstop

#include "StorageText.h"


//---------------------------------------------------------------------------
// class TStorageSqlText: TStorage
// Класс объекта источника/приемника данных
// Используется, хранимая в файле Text
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
/*void TStorageSqlText::Append()
{
    if (hasChangedAfterAppend) {
        hasChangedAfterAppend = false;
        TStorage::Append();
    }
} */
                                       
TStorageSqlText::~TStorageSqlText()
{
}

void TStorageSqlText::Open(bool ReadOnly)
{
    this->ReadOnly = ReadOnly;
   
    AnsiString templateFileName = Tables[TableIndex].Template;
    if (templateFileName != "") {
        if (!FileExists(templateFileName)) {
            throw Exception("File not found " + templateFileName + ".");
        }
        TStringList* pStringList;
        pStringList = new TStringList();
        pStringList->Clear();
        pStringList->LoadFromFile(templateFileName);
        SqlText = pStringList->Text;
        pStringList->Free();
    } else {
        throw Exception("File not found.");
    }
}


//Variant Get(AnsiString Field) {};

//---------------------------------------------------------------------------
// Задает значение текущего поля
void TStorageSqlText::SetFieldValue(Variant Value)
{
    if (!IsActiveField())
        return;

    String dstParam = "/**:" + Fields[FieldIndex]->name + "**/";
    TReplaceFlags replaceflags = TReplaceFlags() << rfReplaceAll << rfIgnoreCase;

    String OldText = SqlText;
    if (!VarIsNull(Value)) {
        SqlText = StringReplace(SqlText, dstParam, Value, replaceflags);
    } else {
        SqlText = StringReplace(SqlText, dstParam, "null", replaceflags);
    }
    if (SqlText != OldText) {
        RecordCount++;
    }
}

//---------------------------------------------------------------------------
// Фиксирует текущее хранилище
void TStorageSqlText::Commit()
{
    if (ReadOnly) {
        throw Exception("Can't commit the storage because it is read-only.");
    }

    TStringList* pStringList = new TStringList();

    if (!pStringList) {
        throw Exception("Can't allocate memmory.");
    }

    pStringList->Clear();
    pStringList->Text = SqlText;

    try {
        pStringList->SaveToFile(Tables[TableIndex].File);
        pStringList->Free();
        Modified = true;
        //RecordCount = 1;
        //RecordCount++;
    } catch (...) {
        pStringList->Free();
        throw Exception("Can't commit.");
    }




}

//---------------------------------------------------------------------------
//
void TStorageSqlText::Close()
{
    Active = false;
    TStorage::Close();

/*    if (msexcel != NULL) {
        Active = false;

        delete msexcel;
        msexcel = NULL;

        // В последующем обернуть эти переменные
        Workbook = Unassigned;   // эквивалентно VarClear(WorkBooks)
        Worksheet = Unassigned;   // эквивалентно VarClear(WorkBooks)

    } */
}

//void NextTable() {};
//void NextRecord() {};
//void NextField();

//---------------------------------------------------------------------------
// Переходит к следующему полю таблицы
//void TStorageSqlText::NextField()
//{
//}


TSqlTextField* TStorageSqlText::AddField()
{
    TSqlTextField* Field = new TSqlTextField();
    Fields.push_back(dynamic_cast<TSqlTextField*>(Field));
    Field->required = false;

    FieldCount++;
    return Field;
}

void TStorageSqlText::AddTable(const TSqlTextTable& Table)
{
    TSearchRec SearchRec;
    FindFirst(Table.Template, faAnyFile, SearchRec);
    if (SearchRec.Name != "") {     //Если файл найден
        Tables.push_back(Table);
        TableCount++;
    } else {
        FindClose(SearchRec);
        throw Exception("Шаблон не найден.");
    }

    /*if (SearchRec.Name != "") {     // Если файл найден, то формируем новый элемент-Таблицу
        AnsiString FilePath = ExtractFilePath(Table.File);
        do {
            TExcelTable NewTable;
            NewTable.File = FilePath + SearchRec.Name;
            NewTable.Truncate = Table.Truncate;
            Tables.push_back(NewTable);
            TableCount++;
        } while ( FindNext(SearchRec) == 0);
    } else {    // Если файл не найден, добавляем таблицу как есть
        Tables.push_back(Table);
        TableCount++;
    }*/
    FindClose(SearchRec);

}


AnsiString TStorageSqlText::GetTable()
{
    if (!Eot()) {
        return Tables[TableIndex].File;
    }
}



