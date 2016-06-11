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
// ���������� true ���� ��������� ����� ������ ������
bool TStorage::Eot()
{
    return TableIndex >= TableCount;
}

//---------------------------------------------------------------------------
// ���������� true ���� ��������� ����� ������ �����
bool TStorage::Eof()
{
    return FieldIndex  >= FieldCount;
}

//---------------------------------------------------------------------------
// ��������� � ��������� �������
void TStorage::NextTable()
{
    if (!Eot()) {
        TableIndex ++;
        FieldIndex = 0;
    }
}

//---------------------------------------------------------------------------
// ��������� � ���������� ���� �������
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
// ��������� ����� ���� � ������ �����
void TStorageDbase::AddField(const TDbaseField& Field)
{
    Fields.push_back(Field);
    FieldCount++;
}
//---------------------------------------------------------------------------
// ��������� ������� � ������ (��� �����, ����� ���� ������ ������)
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
// �������� �������
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

    // ������� ����������� ����� ������� �� ����������
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
// ���������� �������� ����
Variant TStorageDbase::Get(AnsiString Field)
{
    // �������� ����������?????? Field[FieldIndex].name
    //if ()
    try {
        return pTable->FieldByName(Field)->Value;
    } catch (...) {
        //StorageStatus = SS_FIELD_NOT_FOUND;
        throw Exception("Field not found " + Field + ".");
    }
}

//---------------------------------------------------------------------------
// ������������� �������� ��������� ����
//void TStorageDbase::Set(AnsiString Field, Variant Value)
void TStorageDbase::Set(Variant Value)
{

    if (Fields[FieldIndex].active && Fields[FieldIndex].enable)
        pTable->FieldByName(Fields[FieldIndex].name)->Value = Value;
    //pTable->FieldByNumber(FieldIndex)->Value = Value;
    //pTable->FieldByName(Field)->Value = Value;
}

//---------------------------------------------------------------------------
// ��������� ���������
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
// ��������� ����� ������ � �������
void TStorageDbase::Append()
{
    pTable->Append();
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// ��������� �������
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
// ���������� true ���� ��������� ����� �������
bool TStorageDbase::Eor()
{
    return pTable->Eof;
}

//---------------------------------------------------------------------------
// ���������� true ���� ���������� ��������� ������
//bool TStorageDbase::Eof()
//{
//    return FieldIndex + 1 == Fields.size();
//}

//---------------------------------------------------------------------------
// ��������� � ��������� �������
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
// ��������� � ��������� ������ �������
void TStorageDbase::NextRecord()
{
    //int k = pTable->RecNo;
    pTable->Next();
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// ��������� � ���������� ���� �������
//void TStorageDbase::NextField()
//{

//    if (!Eof()) {
//        FieldIndex++;
//    }
//}

//---------------------------------------------------------------------------
// ���������� ��������������� ��������� ���� - ��� ���� � �������-���������
AnsiString TStorageDbase::GetSrcField()
{
    return Fields[FieldIndex].name_src;
}

//---------------------------------------------------------------------------
// ���������� true - ���� ���� ���� �������� ������
bool TStorageDbase::IsActiveField()
{
    return Fields[FieldIndex].active && Fields[FieldIndex].enable;
}

//---------------------------------------------------------------------------
// ���������� ������������ ��������� ���������/��������� ������
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
    // ��������� ��������
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
// �������� ������������ OraQuery
void TStorageOraProc::Open()
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
            throw Exception("Can't to truncate table " + Tables[TableIndex].Table + ".");
        }
    }

    try {
 	    OraQuery->CreateProcCall(Tables[TableIndex].Procedure, 0);
    } catch (...) {
        throw Exception("Can't to create procedure call.");
    }

    FieldCount = Fields.size();
    Active = true;
}

//---------------------------------------------------------------------------
// ������������� �������� ��������� ����
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
// ���������� ��������������� ��������� ���� - ��� ���� � �������-���������
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
// ���������� ������������ ��������� ���������/��������� ������
AnsiString TStorageOraProc::GetTable()
{
    //return Procedure;
    if (!Eot()) {
        return Tables[TableIndex].Procedure;
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


    // �������� ������� �������� ���� - ������� ����������
    // �������� ��� ��������.
    // ���� ��������, �� ����������� �������� FieldsCount = OraQuery->Fields->Count
    FieldCount = OraQuery->Fields->Count;
    //FieldCount = Fields.size();
    Active = true;
}

//---------------------------------------------------------------------------
// ���������� �������� ����
Variant TStorageOraSql::Get(AnsiString Field)
{
    // �������� ����������?????? Field[FieldIndex].name
    return OraQuery->FieldByName(Field)->Value;
}


//---------------------------------------------------------------------------
// ������������� �������� ��������� ����
void TStorageOraSql::Set(Variant Value)
{
//    OraQuery->Append();
    if (Fields[FieldIndex].active && Fields[FieldIndex].enable)
        OraQuery->FieldByName(Fields[FieldIndex].name)->Value = Value;
        //OraQuery->ParamByName(Fields[FieldIndex].name)->Value = Value;
}

//---------------------------------------------------------------------------
// ��������� ����� ������ � �������
void TStorageOraSql::Append()
{
    OraQuery->Append();
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// ��������� ���������
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
// ���������� true ���� ��������� ����� �������
bool TStorageOraSql::Eor()
{
    if (OraQuery != NULL) {
        return OraQuery->Eof;
    } else {
        return true;
    }
}

//---------------------------------------------------------------------------
// ��������� � ��������� �������
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
// ��������� � ��������� ������ �������
void TStorageOraSql::NextRecord()
{
    //int k = pTable->RecNo;
    OraQuery->Next();
    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// ���������� ��������������� ��������� ���� - ��� ���� � �������-���������
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
// ���������� ������������ ��������� ���������/��������� ������
AnsiString TStorageOraSql::GetTable()
{
    if (!Eot()) {
        if (Tables[TableIndex].Sql != "") {
            return Tables[TableIndex].Sql;
        } else {
            return Tables[TableIndex].Table;
        }
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
// ��������� ������� � ������ (��� �����, ����� ���� ������ ������)
void TStorageExcel::AddTable(const TExcelTable& Table)
{
    TSearchRec SearchRec;
    FindFirst(Table.File, faAnyFile, SearchRec);

    if (SearchRec.Name != "") {     // ���� ���� ������, �� ��������� ����� �������-�������
        AnsiString FilePath = ExtractFilePath(Table.File);
        do {
            TExcelTable NewTable;
            NewTable.File = FilePath + SearchRec.Name;
            NewTable.Truncate = Table.Truncate;
            Tables.push_back(NewTable);
            TableCount++;
        } while ( FindNext(SearchRec) == 0);
    } else {    // ���� ���� �� ������, ��������� ������� ��� ����
        Tables.push_back(Table);
        TableCount++;
    }
    FindClose(SearchRec);
}

//---------------------------------------------------------------------------
// �������� �������
void TStorageExcel::Open()
{
    // ��������� ������ �������� - �������� ������� �����!!!!
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
    } else {  // ���� ��������� ��� ��������, �� � ����� ������ ������� ����� ����
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
                // ������� "���������" ������� (�����)
                for(int i = 0; i< Fields.size(); i++) {
                    msexcel->WriteToCell(Worksheet, Fields[i].name, 1, i+1, "@");
                }
            }
        } else {
            try {
                msexcel->OpenApplication();
                Workbook = msexcel->OpenDocument();
                Worksheet = msexcel->GetSheet(Workbook, 1);

                // ������� "���������" ������� (�����)
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

    // ������� ��������
    if (ReadOnly) {
        int j = 1;
        String sCellValue;
        while ( (sCellValue = msexcel->ReadCell(Worksheet, 1, j)) != "") {
            FieldsList[sCellValue] = j;
            j++;
        }
        FieldCount = j - 1;
    } else {
        FieldCount = Fields.size();
    }

    // ������� �����
    int i = 2;
    String sCellValue;
    while ( (sCellValue = msexcel->ReadCell(Worksheet, i, 1)) != "") {
        i++;
    }
    RecordCount = i - 2;

    Active = true;
}

//---------------------------------------------------------------------------
//
void TStorageExcel::OpenExcel()
{
}

 //---------------------------------------------------------------------------
// ��������� �������
void TStorageExcel::Close()
{
    if (msexcel != NULL) {
        Active = false;
        msexcel->CloseApplication();
        delete msexcel;
        msexcel = NULL;
    }
}

bool TStorageExcel::Eor()
{
    return RecordIndex >= RecordCount;
}

// ���������� ������������ ��������� ���������/��������� ������
AnsiString TStorageExcel::GetTable()
{
    if (!Eot()) {
        return Tables[TableIndex].File;
    }
}

//---------------------------------------------------------------------------
// ���������� �������� ����
Variant TStorageExcel::Get(AnsiString Field)
{

    if (FieldsList.find(Field) != FieldsList.end() ) {
        //int j = FieldsList[Field];
        return msexcel->ReadCell(Worksheet, RecordIndex+2, FieldsList[Field]);
    } else {
        throw Exception("Field not found " + Field + ".");
    }
/*    try {
        return pTable->FieldByName(Field)->Value;
    } catch (...) {
        throw Exception("Field not found " + Field + ".");
    }*/
}

//---------------------------------------------------------------------------
// ��������� � ��������� ������ �������
void TStorageExcel::NextRecord()
{
    if (!Eor()) {
        RecordIndex++;
        FieldIndex = 0;
    }
    //else {
        //throw Exception("");
    //}
}

//---------------------------------------------------------------------------
// ��������� ����� ������ � �������
void TStorageExcel::Append()
{
    //pTable->Append();
    RecordIndex = RecordCount;
    RecordCount++;

    FieldIndex = 0;
}

//---------------------------------------------------------------------------
// ��������� ����� ������ � �������
//void TStorageExcel::Append()
//{
    //pTable->Append();
//    RecordCount = RecordIndex;
//    FieldIndex = 0;
//}


//---------------------------------------------------------------------------
// ������������� �������� ��������� ����
void TStorageExcel::Set(Variant Value)
{
    if (Fields[FieldIndex].active && Fields[FieldIndex].enable) {
        msexcel->WriteToCell(Worksheet, Value, RecordIndex+2, FieldIndex+1, Fields[FieldIndex].format);
        //pTable->FieldByName(Fields[FieldIndex].name)->Value = Value;
    }

    //pTable->FieldByNumber(FieldIndex)->Value = Value;
    //pTable->FieldByName(Field)->Value = Value;
}

//---------------------------------------------------------------------------
// ��������� ����� ���� � ������ �����
void TStorageExcel::AddField(const TExcelField& Field)
{
    Fields.push_back(Field);
    FieldCount++;
}

//---------------------------------------------------------------------------
// ��������� ���������
void TStorageExcel::Commit()
{
    //����� ������� ���������� ��������� � ����
    if (ReadOnly)
        return;

    msexcel->SaveDocument(Workbook);
    Modified = true;

    /*if (pTable->Modified) {
        try {
            pTable->Post();
            Modified = true;
            RecordCount = pTable->RecordCount;
        } catch (...) {
            throw;
        }
    }
    pTable->Post();
    */
}

//---------------------------------------------------------------------------
// ���������� ��������������� ��������� ���� - ��� ���� � �������-���������
AnsiString TStorageExcel::GetSrcField()
{
    return Fields[FieldIndex].name_src;
}


