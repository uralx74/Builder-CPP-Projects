#include "Storage.h"

TStorage::TStorage()
{
    TableIndex = 0;
    TableCount = 0;
    FieldCount = 0;
    FieldIndex = 0;
    RecordCount = 0;
    RecordIndex = 0;
    StorageStatus = SS_EMPTY;
}

// Возвращает true если достигнут конец списка таблиц
bool TStorage::Eot()
{
    return TableIndex >= TableCount;
}

bool TStorage::Eof()
{
    return FieldIndex  >= FieldCount;
}


void TStorage::NextTable()
{
    if (!Eot()) {
        TableIndex ++;
        FieldIndex = 0;
    }
}

// Переходит к следующему полю таблицы
void TStorage::NextField()
{
    if (!Eof()) {
        FieldIndex++;
    }
}







//---------------------------------------------------------------------------
//
TStorageDbase::TStorageDbase()
{
    pTable = NULL;
}

//---------------------------------------------------------------------------
// Добавляет таблицу в список (имя файла)
void TStorageDbase::AddTable(AnsiString FileName)
{
    Tables.push_back(FileName);
}

//---------------------------------------------------------------------------
// Открытие таблицы
bool TStorageDbase::Open(bool ReadOnly)
{
    if (ReadOnly) {

        pTable = new TDbf(NULL);

        pTable->TableName = Tables[TableIndex];
        pTable->Exclusive = true;

        if (FileExists(Tables[TableIndex])) {
            try {
                pTable->Open();
                StorageStatus = SS_FILE_IS_OPENED;
            } catch (...) {
                return false;
            }
        } else {
            StorageStatus = SS_FILE_NOT_FOUND;
        }

        TableCount = Tables.size();  //!!!!!!!!!!!! внимание переделать

    } else {
        Create();
    }
    return true;

}

//---------------------------------------------------------------------------
//
void TStorageDbase::Create()
{
    pTable = new TDbf(NULL);

    //pTableDst->TableLevel = 7; // required for AutoInc field
    pTable->TableLevel = 4;
    pTable->LanguageID = DbfLangId_RUS_866;

    pTable->TableName = ExtractFileName(Tables[TableIndex]);
    pTable->FilePathFull = ExtractFilePath(Tables[TableIndex]);

    // Создаем определение полей таблицы из параметров
    TDbfFieldDefs* TempFieldDefs;
    TempFieldDefs = new TDbfFieldDefs(NULL);
    for(std::vector<TDBaseField>::iterator it = Fields.begin(); it < Fields.end(); it++ ) {
        if (!it->enable)
            continue;
        TDbfFieldDef* TempFieldDef = TempFieldDefs->AddFieldDef();
        TempFieldDef->FieldName = it->name;
        TempFieldDef->NativeFieldType = it->type[1];
        TempFieldDef->Size = it->length;
        TempFieldDef->Precision = it->decimals;
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

//---------------------------------------------------------------------------
// Возвращает значение поля
Variant TStorageDbase::Get(AnsiString Field)
{
    // Возможно Переделать?????? Field[FieldIndex].name
    //if ()
    try {
        return pTable->FieldByName(Field)->Value;
    } catch (...) {
        StorageStatus = SS_FIELD_NOT_FOUND;
        throw;
    }
}

//---------------------------------------------------------------------------
// Устанавливает значение активного поля
//void TStorageDbase::Set(AnsiString Field, Variant Value)
void TStorageDbase::Set(Variant Value)
{

    if (Fields[FieldIndex].active && Fields[FieldIndex].enable)
        pTable->FieldByName(Fields[FieldIndex].name)->Value = Value;
    //pTable->FieldByNumber(FieldIndex)->Value = Value;
    //pTable->FieldByName(Field)->Value = Value;
}

//---------------------------------------------------------------------------
// Фиксирует изменения
void TStorageDbase::Commit()
{
    pTable->Post();
}
//---------------------------------------------------------------------------
// Добавляет новую запись в таблицу
void TStorageDbase::Append()
{
    pTable->Append();
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// Закрывает таблицу
void TStorageDbase::Close()
{
    if (pTable != NULL) {
        pTable->Close();
        delete pTable;
        pTable = NULL;
    }
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
        //delete pTable;
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
// Возвращает соответствующее активному полю - имя поля в таблице-источнике
AnsiString TStorageDbase::GetSrcField()
{
    return Fields[FieldIndex].name_src;
}

//---------------------------------------------------------------------------
// Возвращает true - если поле поле подлежит записи
bool TStorageDbase::IsActiveField()
{
    return Fields[FieldIndex].active && Fields[FieldIndex].enable;
}

//---------------------------------------------------------------------------
// Возвращает наименование активного источника/приемника данных
AnsiString TStorageDbase::GetTable()
{
    if (!Eot()) {
        return Tables[TableIndex];
    }
}

//---------------------------------------------------------------------------
//
void TStorageOra::OpenConnection(AnsiString Server, AnsiString Username, AnsiString Password)
{
    // Открываем приемник
    OraSession = new TOraSession(NULL);
    //ThreadOraSession->OnError = OraSession1Error;
    OraSession->LoginPrompt = false;
    OraSession->Password = Password;
    OraSession->Username = Username;
    OraSession->Server = Server;
    //OraSession->Options = TemplateOraSession->Options;
    //OraSession->HomeName = TemplateOraSession->HomeName;
    OraSession->Options->Direct = true;
    OraSession->ConnectMode = cmNormal;
    OraSession->Pooling = false;
    OraSession->ThreadSafety = true;
    OraSession->DataTypeMap->Clear();
    OraSession->Connect();
}

//---------------------------------------------------------------------------
//
void TStorageOra::Close()
{
    if (OraQuery != NULL) {

        OraQuery->Close();
        delete OraQuery;
        OraQuery = NULL;
    }

    if (OraSession != NULL) {
        OraSession->Close();
        delete OraSession;
        OraSession = NULL;

    }
}

//---------------------------------------------------------------------------
//
TStorageOraProc::TStorageOraProc()
{
    TableIndex = 0;
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
//
TStorageOraProc::~TStorageOraProc()
{
}

//---------------------------------------------------------------------------
// Создание принимающего OraQuery
bool TStorageOraProc::Open(bool ReadOnly)
{
    OpenConnection(Tables[TableIndex].Server, Tables[TableIndex].Username, Tables[TableIndex].Password);


   	OraQuery = new TOraQuery(NULL);
    OraQuery->Session = OraSession;
    OraQuery->SQL->Clear();

    if (Tables[TableIndex].Truncate) {
        try {
            OraQuery->SQL->Add("truncate table " + Tables[TableIndex].Table);
            OraQuery->ExecSQL();
            OraQuery->SQL->Clear();
        } catch (...) {
        }
    }

    try {
 	    OraQuery->CreateProcCall(Tables[TableIndex].Procedure, 0);
    } catch (...) {
    }
}

/*void TStorageOraProc::Open()
{

} */

/*Variant TStorageOraProc::Get(AnsiString Field)
{

}*/

//---------------------------------------------------------------------------
// Устанавливает значение активного поля
void TStorageOraProc::Set(Variant Value)
{
    if (Fields[FieldIndex].active && Fields[FieldIndex].enable)
        OraQuery->ParamByName(Fields[FieldIndex].name)->Value = Value;
}

/*
void TStorageOraProc::Commit()
{

}*/

//---------------------------------------------------------------------------
//
void TStorageOraProc::Post()
{
    OraQuery->ExecSQL();
    FieldIndex = 0;
}


/*// End Of Tables
bool TStorageOraProc::Eot()
{

}*/

/*// End Of Records
bool TStorageOraProc::Eor()
{

} */

// End Of Fields
bool TStorageOraProc::Eof()
{
    return FieldIndex + 1 == Fields.size();
}

/*void TStorageOraProc::NextTable()
{

}*/

/*void TStorageOraProc::NextRecord()
{

}*/

//---------------------------------------------------------------------------
//
void TStorageOraProc::NextField()
{
    if (!Eof()) {
        FieldIndex++;
    }
}

//---------------------------------------------------------------------------
//
AnsiString TStorageOraProc::GetSrcField()
{
    return Fields[FieldIndex].name_src;
}

//---------------------------------------------------------------------------
//
bool TStorageOraProc::IsActiveField()
{
    return Fields[FieldIndex].active && Fields[FieldIndex].enable;
}

//---------------------------------------------------------------------------
//
void TStorageOraProc::AddTable(const TOraProcTable& Table)
{
    Tables.push_back(Table);
    TableCount++;
}

//---------------------------------------------------------------------------
// Возвращает наименование активного источника/приемника данных
AnsiString TStorageOraProc::GetTable()
{
    //return Procedure;
    if (!Eot()) {
        return Tables[TableIndex].Procedure;
    }
}







//---------------------------------------------------------------------------
//
TStorageOraTable::TStorageOraTable()
{
    TableIndex = 0;
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
//
TStorageOraTable::~TStorageOraTable()
{
}

//---------------------------------------------------------------------------
//
/*void TStorageOraTable::Create()
{
    OpenConnection(Tables[TableIndex].Server, Tables[TableIndex].Username, Tables[TableIndex].Password);

   	OraQuery = new TOraQuery(NULL);
    OraQuery->Session = OraSession;
    OraQuery->SQL->Clear();
    OraQuery->SQL->Add("select * from " + Tables[TableIndex].Table + " for update");

    try {
        OraQuery->Open();
    } catch (...) {
    }

    FieldCount = Fields.size();
}*/

//---------------------------------------------------------------------------
//
bool TStorageOraTable::Open(bool ReadOnly)
{
    OpenConnection(Tables[TableIndex].Server, Tables[TableIndex].Username, Tables[TableIndex].Password);

    AnsiString sForUpdate = "";
    if (!ReadOnly)
        sForUpdate = " for update";

   	OraQuery = new TOraQuery(NULL);
    OraQuery->Session = OraSession;
    OraQuery->SQL->Clear();
    OraQuery->SQL->Add("select * from " + Tables[TableIndex].Table + sForUpdate);

    try {
        OraQuery->Open();
    } catch (...) {
    }

    FieldCount = Fields.size();
}

//---------------------------------------------------------------------------
//
Variant TStorageOraTable::Get(AnsiString Field)
{

}

//---------------------------------------------------------------------------
//
void TStorageOraTable::Set(Variant Value)
{
//    OraQuery->Append();
    if (Fields[FieldIndex].active && Fields[FieldIndex].enable)
        OraQuery->FieldByName(Fields[FieldIndex].name)->Value = Value;
        //OraQuery->ParamByName(Fields[FieldIndex].name)->Value = Value;
}

//---------------------------------------------------------------------------
// Добавляет новую запись в таблицу
void TStorageOraTable::Append()
{
    OraQuery->Append();
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// Фиксирует изменения
void TStorageOraTable::Commit()
{
    OraQuery->Post();
}

//---------------------------------------------------------------------------
//
//void TStorageOraTable::Post()
//{
    //OraQuery->ExecSQL();
    //FieldIndex = 0;
//}

//---------------------------------------------------------------------------
// End Of Records
bool TStorageOraTable::Eor()
{

}

/*void TStorageOraTable::NextTable()
{

}*/

//---------------------------------------------------------------------------
//
void TStorageOraTable::NextRecord()
{

}

//---------------------------------------------------------------------------
//
//void TStorageOraTable::NextField()
//{
///    if (!Eof()) {
//        FieldIndex++;
//    }
//}

//---------------------------------------------------------------------------
//
AnsiString TStorageOraTable::GetSrcField()
{
    return Fields[FieldIndex].name_src;
}

//---------------------------------------------------------------------------
//
bool TStorageOraTable::IsActiveField()
{
    return Fields[FieldIndex].active && Fields[FieldIndex].enable;
}

//---------------------------------------------------------------------------
//
void TStorageOraTable::AddTable(const TOraTableTable &Table)
{
    Tables.push_back(Table);
    TableCount++;
}

//---------------------------------------------------------------------------
// Возвращает наименование активного источника/приемника данных
AnsiString TStorageOraTable::GetTable()
{
    //return Procedure;
    /*if (!Eot()) {
        return Files[TableIndex];
    }*/
}




//---------------------------------------------------------------------------
//
TStorageOraSql::TStorageOraSql()
{
    //TableIndex = 0;
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
//
TStorageOraSql::~TStorageOraSql()
{
}

//---------------------------------------------------------------------------
//
//void TStorageOraSql::Create()
//{


    //try {
 	    //OraQuery->CreateProcCall("pk_nasel_otdel.LoadMailbankProc", 0);
    //} catch (...) {
    //}

//}

//---------------------------------------------------------------------------
//
bool TStorageOraSql::Open(bool ReadOnly)
{

    //AnsiString sss = Tables[TableIndex].Password;

    //OraSession->Password = Tables[TableIndex].Password;
    //OraSession->Username = Tables[TableIndex].Username;
    //OraSession->Server = Tables[TableIndex].Server;

    AnsiString Sql = Tables[TableIndex].Sql;
    AnsiString SqlText;

    if (!FileExists(Sql))
        return false;

    OpenConnection(Tables[TableIndex].Server, Tables[TableIndex].Username, Tables[TableIndex].Password);

    TStringList* ptStrintList;
    ptStrintList=new TStringList();
    ptStrintList->Clear();
    ptStrintList->LoadFromFile(Sql);
    SqlText = ptStrintList->Text;
    ptStrintList->Free();

   	OraQuery = new TOraQuery(NULL);
    OraQuery->Session = OraSession;
    OraQuery->SQL->Clear();
    OraQuery->SQL->Add(SqlText);

    OraQuery->Open();

    return true;

}

//---------------------------------------------------------------------------
// Возвращает значение поля
Variant TStorageOraSql::Get(AnsiString Field)
{
    // Возможно Переделать?????? Field[FieldIndex].name
    //if ()
    return OraQuery->FieldByName(Field)->Value;
}




/*//---------------------------------------------------------------------------
// Устанавливает значение активного поля
void TStorageOraSql::Set(Variant Value)
{
    if (Fields[FieldIndex].active && Fields[FieldIndex].enable)
        OraQuery->ParamByName(Fields[FieldIndex].name)->Value = Value;
} */



//---------------------------------------------------------------------------
// Фиксирует изменения
void TStorageOraSql::Commit()
{
    OraQuery->Post();
    FieldIndex = 0;

}

//---------------------------------------------------------------------------
//
void TStorageOraSql::Post()
{
    //OraQuery->ExecSQL();
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// Возвращает true если достигнут конец таблицы
bool TStorageOraSql::Eor()
{
    return OraQuery->Eof;
}

//---------------------------------------------------------------------------
// Переходит к следующей таблице
void TStorageOraSql::NextTable()
{
    TStorageOra::NextTable();

    if (OraQuery != NULL) {
        Close();
    }

    if (!Eot()) {
        Open();
    }
}

//---------------------------------------------------------------------------
// Переходит к следующей записи таблицы
void TStorageOraSql::NextRecord()
{
    //int k = pTable->RecNo;
    OraQuery->Next();
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
//
AnsiString TStorageOraSql::GetSrcField()
{
    return Fields[FieldIndex].name_src;
}

//---------------------------------------------------------------------------
//
bool TStorageOraSql::IsActiveField()
{
    return Fields[FieldIndex].active && Fields[FieldIndex].enable;
}

//---------------------------------------------------------------------------
//
void TStorageOraSql::AddTable(const TOraSqlTable &Table)
{
    Tables.push_back(Table);
    TableCount++;
}

//---------------------------------------------------------------------------
// Возвращает наименование активного источника/приемника данных
AnsiString TStorageOraSql::GetTable()
{
    if (!Eot()) {
        return Tables[TableIndex].Sql;
    }
}


 