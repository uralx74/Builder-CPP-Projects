#include "Storage.h"

//---------------------------------------------------------------------------
// TStorage
// The interface class
//---------------------------------------------------------------------------

TStorage::TStorage()
{
    TableIndex = 0;
    TableCount = 0;
    FieldCount = 0;
    FieldIndex = 0;
    RecordCount = 0;
    RecordIndex = 0;
    Active = false;
    ReadOnly = true;
    Modified = false;
}

//---------------------------------------------------------------------------
// Возвращает true если достигнут конец списка таблиц
bool TStorage::Eot()
{
    return TableIndex >= TableCount;
}

//---------------------------------------------------------------------------
// Возвращает true если достигнут конец списка полей
bool TStorage::Eof()
{
    return FieldIndex  >= FieldCount;
}

//---------------------------------------------------------------------------
// Переходит к следующей таблице
void TStorage::NextTable()
{
    if (!Eot()) {
        TableIndex ++;
        FieldIndex = 0;
    }
}

//---------------------------------------------------------------------------
// Переходит к следующему полю таблицы
void TStorage::NextField()
{
    if (!Eof()) {
        FieldIndex++;
    }
}

//---------------------------------------------------------------------------
//
AnsiString TStorage::GetTableStage()
{
    return IntToStr(TableIndex+1) + "/" + IntToStr(TableCount);
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

//---------------------------------------------------------------------------
// Добавляет новое поле в список полей
void TStorageDbase::AddField(const TDbaseField& Field)
{
    Fields.push_back(Field);
    FieldCount++;
}
//---------------------------------------------------------------------------
// Добавляет таблицу в список (имя файла, может быть задано маской)
void TStorageDbase::AddTable(const TDbaseTable& Table)
{
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
void TStorageDbase::Open()
{
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
    for(std::vector<TDbaseField>::iterator it = Fields.begin(); it < Fields.end(); it++ ) {
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
        throw Exception("Field not found " + Field + ".");
    }
}

//---------------------------------------------------------------------------
// Устанавливает значение активного поля
void TStorageDbase::Set(Variant Value)
{
    if (Fields[FieldIndex].active && Fields[FieldIndex].enable) {
        if (/*!VarIsEmpty(Value) && !VarIsNull(Value) &&*/ (AnsiString)Value != "")
                // (AnsiString)Value != "" - Эта проверка критична,
                // так как в DBF исключено значение NULL
            pTable->FieldByName(Fields[FieldIndex].name)->Value = Value;
    }
}

//---------------------------------------------------------------------------
// Фиксирует изменения
void TStorageDbase::Commit()
{
    if (ReadOnly)
        return;

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
    FieldIndex = 0;
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
        return Tables[TableIndex].File;
    }
}


//---------------------------------------------------------------------------
// TStorageOra
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
void TStorageOra::OpenConnection(AnsiString Server, AnsiString Username, AnsiString Password)
{
    try {
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
    } catch (Exception& e) {
        delete OraSession;
        OraSession = NULL;
        throw Exception("Can't connect to server \"" + Server + "\". " + e.Message);
    }
}

//---------------------------------------------------------------------------
//
void TStorageOra::Close()
{
    if (OraQuery != NULL) {
        Active = false;
        OraQuery->Close();
        delete OraQuery;
        OraQuery = NULL;
    }

    if (OraSession != NULL) {
        OraSession->Close();
        delete OraSession;
        OraSession = NULL;
    }
    Active = false;
}

//---------------------------------------------------------------------------
//
TStorageOra::~TStorageOra()
{
    Close();
}

//---------------------------------------------------------------------------
// Создание принимающего OraQuery
void TStorageOraProc::Open()
{
    if (Tables.size()>0)
        OpenConnection(Tables[TableIndex].Server, Tables[TableIndex].Username, Tables[TableIndex].Password);
    else
        throw Exception("Storage is not specified.");

    try {
   	    OraQuery = new TOraQuery(NULL);
        OraQuery->Session = OraSession;
        OraQuery->SQL->Clear();
    } catch (...) {
        delete OraQuery;
        OraQuery = NULL;
        throw;
    }

    if (Tables[TableIndex].Truncate) {
        try {
            OraQuery->SQL->Add("truncate table " + Tables[TableIndex].Table);
            OraQuery->ExecSQL();
            OraQuery->SQL->Clear();
        } catch (...) {
            delete OraQuery;
            OraQuery = NULL;
            throw Exception("Can't truncate table " + Tables[TableIndex].Table + ".");
        }
    }

    try {
 	    OraQuery->CreateProcCall(Tables[TableIndex].Procedure, 0);
    } catch (...) {
        delete OraQuery;
        OraQuery = NULL;
        throw Exception("Can't create procedure call.");
    }

    FieldCount = Fields.size();
    Active = true;
}

//---------------------------------------------------------------------------
// Устанавливает значение активного поля
void TStorageOraProc::Set(Variant Value)
{
    if (Fields[FieldIndex].active && Fields[FieldIndex].enable)
        OraQuery->ParamByName(Fields[FieldIndex].name)->Value = Value;
}

//---------------------------------------------------------------------------
//
void TStorageOraProc::Post()
{
    OraQuery->ExecSQL();
    FieldIndex = 0;
    RecordCount++;
    Modified = true;
}


//---------------------------------------------------------------------------
//
void TStorageOraProc::NextField()
{
    if (!Eof()) {
        FieldIndex++;
    }
}

//---------------------------------------------------------------------------
// Возвращает соответствующее активному полю - имя поля в таблице-источнике
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
void TStorageOraProc::AddField(const TOraField& Field)
{
    Fields.push_back(Field);
    FieldCount++;
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
        if (Tables[TableIndex].Table != "") {   // Если задана таблица, то возвращаем имя таблицы
            return Tables[TableIndex].Table;
        } else {
            return Tables[TableIndex].Procedure;
        }
    }
}

//---------------------------------------------------------------------------
//  TStorageOraSql
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
void TStorageOraSql::Open()
{
    OraQuery = NULL;

    OpenConnection(Tables[TableIndex].Server, Tables[TableIndex].Username, Tables[TableIndex].Password);

    AnsiString Sql = Tables[TableIndex].Sql;
    AnsiString SqlText;

    if (Sql != "") {
        if (!FileExists(Sql)) {
            throw Exception("File not found " + Sql + ".");
        }
        TStringList* pStringList;
        pStringList=new TStringList();
        pStringList->Clear();
        pStringList->LoadFromFile(Sql);
        SqlText = pStringList->Text;
        pStringList->Free();
    } else {
        SqlText =  "select * from " + Tables[TableIndex].Table + (ReadOnly == true? "": " for update");
    }


   	OraQuery = new TOraQuery(NULL);
    OraQuery->Session = OraSession;
    OraQuery->SQL->Clear();
    OraQuery->SQL->Add(SqlText);

    try {
        OraQuery->Open();
        RecordCount = OraQuery->RecordCount;
    } catch (...) {
        throw Exception("Can't to open query.");
    }


    // Возможно следует добавить флаг - признак назначения
    // источник или приемник.
    // Если источник, то присваивать значение FieldsCount = OraQuery->Fields->Count
    FieldCount = OraQuery->Fields->Count;
    //FieldCount = Fields.size();
    Active = true;
}

//---------------------------------------------------------------------------
// Возвращает значение поля
Variant TStorageOraSql::Get(AnsiString Field)
{
    // Возможно Переделать?????? Field[FieldIndex].name
    return OraQuery->FieldByName(Field)->Value;
}


//---------------------------------------------------------------------------
// Устанавливает значение активного поля
void TStorageOraSql::Set(Variant Value)
{
//    OraQuery->Append();
    if (Fields[FieldIndex].active && Fields[FieldIndex].enable)
        OraQuery->FieldByName(Fields[FieldIndex].name)->Value = Value;
        //OraQuery->ParamByName(Fields[FieldIndex].name)->Value = Value;
}

//---------------------------------------------------------------------------
// Добавляет новую запись в таблицу
void TStorageOraSql::Append()
{
    OraQuery->Append();
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// Фиксирует изменения
void TStorageOraSql::Commit()
{
    if (ReadOnly)
        return;

    if (OraQuery->Modified) {
        try {
            OraQuery->Post();
            Modified = true;
            RecordCount = OraQuery->RecordCount;
        } catch (...) {
            throw;
        }
    }
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
    if (OraQuery != NULL) {
        return OraQuery->Eof;
    } else {
        return true;
    }
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
// Возвращает соответствующее активному полю - имя поля в таблице-источнике
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
void TStorageOraSql::AddField(const TOraField& Field)
{
    Fields.push_back(Field);
    FieldCount++;
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
        if (Tables[TableIndex].Sql != "") {
            return Tables[TableIndex].Sql;
        } else {
            return Tables[TableIndex].Table;
        }
    } else {
        throw Exception("End of table is reached.");
    }
}

 //---------------------------------------------------------------------------
//  TStorageExcel
//---------------------------------------------------------------------------
TStorageExcel::~TStorageExcel()
{
    Close();
}

//---------------------------------------------------------------------------
// Добавляет таблицу в список (имя файла, может быть задано маской)
void TStorageExcel::AddTable(const TExcelTable& Table)
{
    TSearchRec SearchRec;
    FindFirst(Table.File, faAnyFile, SearchRec);

    if (SearchRec.Name != "") {     // Если файл найден, то формируем новый элемент-Таблицу
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
    }
    FindClose(SearchRec);
}

//---------------------------------------------------------------------------
// Открытие таблицы
void TStorageExcel::Open()
{
    // Продумать случаи открытия - создание пустого файла!!!!
    msexcel = new MSExcelWorks();
    if (ReadOnly) {
        if (FileExists(Tables[TableIndex].File)) {
            try {
                msexcel->OpenApplication();
                Workbook = msexcel->OpenDocument(Tables[TableIndex].File);
                Worksheet = msexcel->GetSheet(Workbook, 1);
            } catch (...) {
                throw Exception("Can't to open file  \"" + Tables[TableIndex].File + "\".");
            }
        } else {
            throw Exception("File not found \"" + Tables[TableIndex].File + "\".");
        }
    } else {  // Если открываем как приемник, то в любом случае создаем новый файл
        if (FileExists(Tables[TableIndex].File)) {
            try {
                msexcel->OpenApplication();
                Workbook = msexcel->OpenDocument(Tables[TableIndex].File);
            } catch (...) {
                Close();
                throw Exception("Can't to open file  \"" + Tables[TableIndex].File + "\".");
            }
            if (msexcel->IsReadOnly(Workbook)) {
                Close();
                throw Exception("Can't to open file  \"" + Tables[TableIndex].File + "\" for writing.");
            }
            Worksheet = msexcel->GetSheet(Workbook, 1);

            for(int i = 0; i < Fields.size(); i++) {
                AnsiString sCellValue = msexcel->ReadCell(Worksheet, 1, i+1);
                if (Fields[i].name != sCellValue) {
                    Close();
                    throw Exception("Field names \"" + Fields[i].name + "\" and \"" + sCellValue +"\" do not match.");
                }
            }
            if (Tables[TableIndex].Truncate) {
                //msexcel->SetVisibleExcel();
                msexcel->ClearWorksheet(Worksheet);
                // Создаем "структуру" таблицы (шапку)
                for(int i = 0; i< Fields.size(); i++) {
                    msexcel->WriteToCell(Worksheet, Fields[i].name, 1, i+1, "@");
                }
            }
        } else {
            try {
                msexcel->OpenApplication();
                Workbook = msexcel->OpenDocument();
                Worksheet = msexcel->GetSheet(Workbook, 1);

                // Создаем "структуру" таблицы (шапку)
                for(int i = 0; i< Fields.size(); i++) {
                    msexcel->WriteToCell(Worksheet, Fields[i].name, 1, i+1, "@");
                }
                msexcel->SaveDocument(Workbook, Tables[TableIndex].File);
                //Modified = true;
            } catch (...) {
                throw Exception("Can't to create file  \"" + Tables[TableIndex].File + "\".");
            }
        }
    }

    // Подсчет столбцов
    if (ReadOnly) {
        int j = 1;
        String sCellValue;
        while ( (sCellValue = msexcel->ReadCell(Worksheet, 1, j)) != "") {
            FieldsList[LowerCase(sCellValue)] = j;
            j++;
        }
        FieldCount = j - 1;
    } else {
        FieldCount = Fields.size();
    }

    // Подсчет строк
    int i = 2;
    String sCellValue;
    while ( (sCellValue = msexcel->ReadCell(Worksheet, i, 1)) != "") {
        i++;
    }
    RecordCount = i - 2;

    Active = true;
}

//---------------------------------------------------------------------------
// Закрывает таблицу
void TStorageExcel::Close()
{
    if (msexcel != NULL) {
        Active = false;
        msexcel->CloseApplication();
        delete msexcel;
        msexcel = NULL;
    }
}

//---------------------------------------------------------------------------
//
bool TStorageExcel::Eor()
{
    return RecordIndex >= RecordCount;
}

//---------------------------------------------------------------------------
// Возвращает наименование активного источника/приемника данных
AnsiString TStorageExcel::GetTable()
{
    if (!Eot()) {
        return Tables[TableIndex].File;
    }
}

//---------------------------------------------------------------------------
// Возвращает значение поля
Variant TStorageExcel::Get(AnsiString Field)
{
    Field = LowerCase(Field);
    if (FieldsList.find(Field) != FieldsList.end() ) {
        return msexcel->ReadCell(Worksheet, RecordIndex+2, FieldsList[Field]);
    } else {
        throw Exception("Field not found " + Field + ".");
    }
}

//---------------------------------------------------------------------------
// Переходит к следующей записи таблицы
void TStorageExcel::NextRecord()
{
    if (!Eor()) {
        RecordIndex++;
        FieldIndex = 0;
    }
}

//---------------------------------------------------------------------------
// Добавляет новую запись в таблицу
void TStorageExcel::Append()
{
    RecordIndex = RecordCount;
    RecordCount++;

    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// Устанавливает значение активного поля
void TStorageExcel::Set(Variant Value)
{
    if (Fields[FieldIndex].active && Fields[FieldIndex].enable) {
        msexcel->WriteToCell(Worksheet, Value, RecordIndex+2, FieldIndex+1, Fields[FieldIndex].format);
        //pTable->FieldByName(Fields[FieldIndex].name)->Value = Value;
    }
}

//---------------------------------------------------------------------------
// Добавляет новое поле в список полей
void TStorageExcel::AddField(const TExcelField& Field)
{
    Fields.push_back(Field);
    FieldCount++;
}

//---------------------------------------------------------------------------
// Фиксирует изменения (сохраняет файл)
void TStorageExcel::Commit()
{
    //Здесь сделать 
    if (ReadOnly)
        return;

    msexcel->SaveDocument(Workbook);
    Modified = true;
}

//---------------------------------------------------------------------------
// Возвращает соответствующее активному полю - имя поля в таблице-источнике
AnsiString TStorageExcel::GetSrcField()
{
    return Fields[FieldIndex].name_src;
}

