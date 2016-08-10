#include "pch.h"
#pragma hdrstop

#include "StorageDbf.h"


//---------------------------------------------------------------------------
// Добавляет новое поле в список полей ()
// возможно переделать на AddField(TDbaseField* Field)
TStorageField* TStorageDbase::AddField(TStorageField* Field)
{
    TStorageField* newField = new TDbaseField();

    //String test2 = (static_cast<TDbaseField*>(newField))->type;

    // Необходимо? приведение типов чтобы скопировать обьект правильно
    *(static_cast<TDbaseField*>(newField)) = *(static_cast<TDbaseField*>(Field));

    this->Fields.push_back(newField);// возможно нужно приводить к  static_cast<TDbaseField*>
    FieldCount++;
    return newField;
}

//---------------------------------------------------------------------------
void TStorageDbase::LoadFieldDefs()
{
    if (!pTable)
        return;

    int n = pTable->DbfFieldDefs->Count;

    for(int i=0; i < pTable->DbfFieldDefs->Count; i++) {
        TDbfFieldDef* fieldDef = pTable->DbfFieldDefs->Items[i];
        TDbaseField* dbaseField = this->AddField();

        dbaseField->type = fieldDef->NativeFieldType;
        dbaseField->name = fieldDef->FieldName;
        dbaseField->length = fieldDef->Size;
        dbaseField->decimals = fieldDef->Precision;
        dbaseField->active = true;
        dbaseField->enable = true;
        dbaseField->name_src = fieldDef->FieldName;
    }
}

//---------------------------------------------------------------------------
// Копирование полей из источника
// Используется двойная диспетчеризация
TStorageDbase::CopyFieldsFrom(TStorage* storage)
{
    // В зависимости от типа storage (производного от TStorage), вызывается метод CopyFieldsToDbf
    storage->CopyFieldsToDbf(this);
}


//---------------------------------------------------------------------------
// Полное копирование полей из DBF в DBF
TStorageDbase::CopyFieldsToDbf(TStorage* storage)
{
    TStorage::FullCopyFields(this, storage);
}


//---------------------------------------------------------------------------
//  TStorageDbase
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
TStorageDbase::TStorageDbase()
{
    pTable = NULL;
}

TStorageDbase::TStorageDbase(String fileName)
{
    //TStorageDbase* storageDbaseStructure = new TStorageDbase();
    TDbaseTable table;
    table.File = fileName;
    //table.File = ExpandFileName(fileName);
    this->AddTable(table);
}


//---------------------------------------------------------------------------
// Добавляет новое поле в список полей
TDbaseField* TStorageDbase::AddField()
{
    /*TStorageField* Field = new TDbaseField();
    Fields.push_back(Field);
    FieldCount++;
    return Field;*/

    TStorageField* Field = new TDbaseField();
    //Fields.push_back(dynamic_cast<TStorageField*>(Field));
    Fields.push_back((TStorageField*)(Field));

    //TStorageField* pField = (TStorageField*)Field;
    //TStorageField* tttt = new TDbaseField();

    //String name1 = typeid(*tttt).name();
    //String name2 = typeid(*pField).name();


    FieldCount++;
    return (TDbaseField*)Field;
}

/*
//---------------------------------------------------------------------------
// Добавляет новое поле в список полей
void TStorageDbase::AddField(const TDbaseField* Field)
{
    Fields.push_back((TStorageField*)Field);
    FieldCount++;
}*/

//---------------------------------------------------------------------------
// Добавляет таблицу в список (имя файла, может быть задано маской)
void TStorageDbase::AddTable(const TDbaseTable& Table)
{
    //TDbaseTable* Table = new TDbaseTable();

    TSearchRec SearchRec;
    FindFirst(Table.File, faAnyFile, SearchRec);

    if (SearchRec.Name != "") {
        AnsiString FilePath = ExtractFilePath(Table.File);
        do {
            TDbaseTable NewTable;
            NewTable.File = FilePath + SearchRec.Name;
            Tables.push_back(NewTable);
            TableCount++;
        } while ( FindNext(SearchRec) == 0);
    } else {
        Tables.push_back(Table);
        TableCount++;
    }
    FindClose(SearchRec);


    //Tables.push_back(Table);
    //TableCount++;
}

//---------------------------------------------------------------------------
// Открытие таблицы
void TStorageDbase::Open(bool ReadOnly)
{
    this->ReadOnly = ReadOnly;
    if (Tables.size() <= TableIndex) {
        throw Exception("The table with index \"" + IntToStr(TableIndex) + "\" doesn't exists.");
    }

    if (ReadOnly) {

        pTable = new TDbf(NULL);

        pTable->TableName = Tables[TableIndex].File;
        pTable->Exclusive = true;

        if (FileExists(Tables[TableIndex].File)) {
            try {
                pTable->Open();
                RecordCount = pTable->RecordCount;
            } catch (...) {
                throw Exception("Can't to open file  " + Tables[TableIndex].File + ".");
            }
        } else {
            throw Exception("File not found " + Tables[TableIndex].File + ".");
        }
    } else {
        Create();
    }
    Active = true;
}

//---------------------------------------------------------------------------
//
void TStorageDbase::Create()
{
    pTable = new TDbf(NULL);

    //pTableDst->TableLevel = 7; // required for AutoInc field
    pTable->TableLevel = 4;
    pTable->LanguageID = DbfLangId_RUS_866;

    pTable->TableName = ExtractFileName(Tables[TableIndex].File);
    pTable->FilePathFull = ExtractFilePath(Tables[TableIndex].File);

    // Создаем определение полей таблицы из параметров
    TDbfFieldDefs* TempFieldDefs;
    TempFieldDefs = new TDbfFieldDefs(NULL);



    // Копирование полей из шаблона, если он задан
    if (templateStorage) {
        templateStorage->Open();
        templateStorage->LoadFieldDefs();

        this->CopyFieldsFrom(templateStorage);

        templateStorage->Close();
        if (delTemplateStorage) {
            delete templateStorage;
        }
    }



    for(std::vector<TStorageField*>::iterator it = Fields.begin(); it < Fields.end(); it++ ) {

        TDbaseField* Field = (TDbaseField*)*it;

        if (!Field->enable)
            continue;
        TDbfFieldDef* TempFieldDef = TempFieldDefs->AddFieldDef();
        TempFieldDef->FieldName = Field->name;
        //TempFieldDef->Required = true;
        //TempFieldDef->FieldType = Field->type;    // Use FieldType if Field->Type is TFieldType else use NativeFieldType
        TempFieldDef->NativeFieldType = Field->type;
        TempFieldDef->Size = Field->length;
        TempFieldDef->Precision = Field->decimals;
    }

    if (TempFieldDefs->Count == 0) {
        delete pTable;
        return;
    }

    pTable->CreateTableEx(TempFieldDefs);
    pTable->Exclusive = true;
    try {
        pTable->Open();
    } catch (...) {
    }

    FieldCount = Fields.size();
}


std::vector<TStorageField> TStorageDbase::GetFieldDefs()
{
    //pTable->FieldDefs
    //return FieldDef;
    std::vector<TStorageField> Result;

}

void TStorageDbase::SetFieldDefs(std::vector<TStorageField>)
{
}

//---------------------------------------------------------------------------
// Возвращает значение поля
Variant TStorageDbase::GetFieldValue(TStorageField* Field)
{
    // Возможно Переделать?????? Field[FieldIndex].name
    //if ()
    try {
        return pTable->FieldByName(Field->name_src)->Value;
    } catch (...) {
        throw Exception("Field \"" + Field->name_src + "\" not found.");
    }
}

//---------------------------------------------------------------------------
// Устанавливает значение активного поля
void TStorageDbase::SetFieldValue(Variant Value)
{
    if (Fields[FieldIndex]->active && Fields[FieldIndex]->enable) {
        if (/*!VarIsEmpty(Value) &&*/ !VarIsNull(Value) && (AnsiString)Value != "")
        //if (/*!VarIsEmpty(Value) && !VarIsNull(Value) &&*/ (AnsiString)Value != "")
                // (AnsiString)Value != "" - Эта проверка критична,
                // так как в DBF исключено значение NULL
            pTable->FieldByName(Fields[FieldIndex]->name)->Value = Value;
    }
}

//---------------------------------------------------------------------------
// Фиксирует изменения
void TStorageDbase::Commit()
{
    if (ReadOnly) {
        throw Exception("Can't commit the storage because it is read-only.");
    }

    if (pTable->Modified) {
        try {
            pTable->Post();
            Modified = true;
            RecordCount = pTable->RecordCount;
        } catch (...) {
            throw;
        }
    }
    //pTable->Post();
}
//---------------------------------------------------------------------------
// Добавляет новую запись в таблицу
void TStorageDbase::Append()
{
    pTable->Append();
    //FieldIndex = 0;
    TStorage::Append();
}

//---------------------------------------------------------------------------
// Закрывает таблицу
void TStorageDbase::Close()
{
    if (pTable != NULL) {
        Active = false;
        pTable->Close();
        delete pTable;
        pTable = NULL;
    }
    TStorage::Close();
}

//---------------------------------------------------------------------------
// Возвращает true если достигнут конец таблицы
bool TStorageDbase::Eor()
{
    return pTable->Eof;
}

//---------------------------------------------------------------------------
// Возвращает true если достигнута последняя запись
//bool TStorageDbase::Eof()
//{
//    return FieldIndex + 1 == Fields.size();
//}

//---------------------------------------------------------------------------
// Переходит к следующей таблице
void TStorageDbase::NextTable()
{
    TStorage::NextTable();

    if (pTable != NULL) {
        Close();
    }

    if (!Eot()) {
        //TableIndex ++;
        //FieldIndex = 0;
        Open();
    }
}

//---------------------------------------------------------------------------
// Переходит к следующей записи таблицы
void TStorageDbase::NextRecord()
{
    //int k = pTable->RecNo;
    pTable->Next();
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// Переходит к следующему полю таблицы
//void TStorageDbase::NextField()
//{

//    if (!Eof()) {
//        FieldIndex++;
//    }
//}


//---------------------------------------------------------------------------
// Возвращает наименование активного источника/приемника данных
AnsiString TStorageDbase::GetTable()
{
    if (!Eot()) {
        return Tables[TableIndex].File;
    }
}

