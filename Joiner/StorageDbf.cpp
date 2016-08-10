#include "pch.h"
#pragma hdrstop

#include "StorageDbf.h"


//---------------------------------------------------------------------------
// ��������� ����� ���� � ������ ����� ()
// �������� ���������� �� AddField(TDbaseField* Field)
TStorageField* TStorageDbase::AddField(TStorageField* Field)
{
    TStorageField* newField = new TDbaseField();

    //String test2 = (static_cast<TDbaseField*>(newField))->type;

    // ����������? ���������� ����� ����� ����������� ������ ���������
    *(static_cast<TDbaseField*>(newField)) = *(static_cast<TDbaseField*>(Field));

    this->Fields.push_back(newField);// �������� ����� ��������� �  static_cast<TDbaseField*>
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
// ����������� ����� �� ���������
// ������������ ������� ���������������
TStorageDbase::CopyFieldsFrom(TStorage* storage)
{
    // � ����������� �� ���� storage (������������ �� TStorage), ���������� ����� CopyFieldsToDbf
    storage->CopyFieldsToDbf(this);
}


//---------------------------------------------------------------------------
// ������ ����������� ����� �� DBF � DBF
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
// ��������� ����� ���� � ������ �����
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
// ��������� ����� ���� � ������ �����
void TStorageDbase::AddField(const TDbaseField* Field)
{
    Fields.push_back((TStorageField*)Field);
    FieldCount++;
}*/

//---------------------------------------------------------------------------
// ��������� ������� � ������ (��� �����, ����� ���� ������ ������)
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
// �������� �������
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

    // ������� ����������� ����� ������� �� ����������
    TDbfFieldDefs* TempFieldDefs;
    TempFieldDefs = new TDbfFieldDefs(NULL);



    // ����������� ����� �� �������, ���� �� �����
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
// ���������� �������� ����
Variant TStorageDbase::GetFieldValue(TStorageField* Field)
{
    // �������� ����������?????? Field[FieldIndex].name
    //if ()
    try {
        return pTable->FieldByName(Field->name_src)->Value;
    } catch (...) {
        throw Exception("Field \"" + Field->name_src + "\" not found.");
    }
}

//---------------------------------------------------------------------------
// ������������� �������� ��������� ����
void TStorageDbase::SetFieldValue(Variant Value)
{
    if (Fields[FieldIndex]->active && Fields[FieldIndex]->enable) {
        if (/*!VarIsEmpty(Value) &&*/ !VarIsNull(Value) && (AnsiString)Value != "")
        //if (/*!VarIsEmpty(Value) && !VarIsNull(Value) &&*/ (AnsiString)Value != "")
                // (AnsiString)Value != "" - ��� �������� ��������,
                // ��� ��� � DBF ��������� �������� NULL
            pTable->FieldByName(Fields[FieldIndex]->name)->Value = Value;
    }
}

//---------------------------------------------------------------------------
// ��������� ���������
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
// ��������� ����� ������ � �������
void TStorageDbase::Append()
{
    pTable->Append();
    //FieldIndex = 0;
    TStorage::Append();
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
    TStorage::Close();
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
// ���������� ������������ ��������� ���������/��������� ������
AnsiString TStorageDbase::GetTable()
{
    if (!Eot()) {
        return Tables[TableIndex].File;
    }
}

