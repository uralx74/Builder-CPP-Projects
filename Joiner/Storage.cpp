#include "pch.h"
#pragma hdrstop

#include "Storage.h"


void TStorage::Close()
{
    std::vector<String>::size_type n = Fields.size();
    for (int i = 0; i < n; i++) {
        delete Fields[i];
    }
    Fields.clear();
    FieldCount = 0;
    FieldIndex = 0;

}

void TStorage::Append()
{
    RecordCount++;
    FieldIndex = 0;
}

void TStorage::Rollback()
{
    RecordCount--;
    FieldIndex = 0;
}


//---------------------------------------------------------------------------
//
bool TStorage::IsLinkedField()
{
    return Fields[FieldIndex]->linked;
}

//---------------------------------------------------------------------------
//
bool TStorage::LinkSource(TStorage* Storage)
{
    // Storage is Source storage
    int n = Fields.size();
    for (int i = 0; i < n; i++) {
        //bool k = Storage->FindField(this->Fields[i]->name_src);




        this->Fields[i]->linked = Storage->FindField(Fields[i]->name_src);
    }
}

//---------------------------------------------------------------------------
//
bool TStorage::FindField(AnsiString fieldName)
{
//int tt = FieldCount;
    int n = Fields.size();
    for (int i = 0; i < n; i++) {
        if (Fields[i]->name == fieldName) {
            return true;
        }
    }
    return false;
}


//---------------------------------------------------------------------------
// ���������� ��������������� ��������� ���� - ��� ���� � �������-���������
TStorageField* TStorage::GetField()
{
    return Fields[FieldIndex];
    //return Fields[FieldIndex]->name_src;
}

//---------------------------------------------------------------------------
// ���������� true - ���� ���� ���� �������� ������
bool TStorage::IsActiveField()
{
    return Fields[FieldIndex]->active && Fields[FieldIndex]->enable;
}


//---------------------------------------------------------------------------
TStorageTable::TStorageTable()
{
    Truncate = false;
}

//---------------------------------------------------------------------------
// ������ templateStorage � �������� ��������� ���������
// ������������ ����� ��������� ������������ ��������
TStorage::SetTemplate(TStorage* storage, bool deleteAfterUse)
{
    templateStorage = storage;
    delTemplateStorage = deleteAfterUse;
}


//---------------------------------------------------------------------------
// ������������ 2
TStorage::CopyFieldsFrom(TStorage* storage)
{
    // �����������
}

//
TStorage::CopyFieldsToDbf(TStorage* storage)
{
    // �����������
    Modified = true;
}

//
TStorage::CopyFieldsToExcel(TStorage* storage)
{
    // �����������
    bool Modified = true;
}



//---------------------------------------------------------------------------
// ������ ����������� �����
// ���������� ������ ��� ���������� ����������� ����!
void TStorage::FullCopyFields(TStorage* src, TStorage* dst)
{
    // �������� �� src � dst
    int n = src->Fields.size();
    for (int i = 0; i < n; i++) {
        dst->AddField(src->Fields[i]);
    }
}




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
// ��������� � ��������� ������ �������
void TStorage::NextRecord()
{
    FieldIndex = 0;
    RecordIndex++;
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
AnsiString TStorage::getTableStage()
{
    return IntToStr(TableIndex+1) + "/" + IntToStr(TableCount);
}

//---------------------------------------------------------------------------
//
AnsiString TStorage::getRecordStage()
{
    return IntToStr(RecordIndex+1) + "/" + IntToStr(RecordCount);
}





