//---------------------------------------------------------------------------
#ifndef StorageTextH
#define StorageTextH

#include <vector>
#include "Storage.h"

//---------------------------------------------------------------------------
// ������ ������� ��������� ������ TStorageSqlText
//
// �����: �.�.����������
// 2016-07-26
//
// ����������:
// ��� ������������� ������ ����� � ��������� �����, ��������������� �
// ��������� ����� � �������� �� ����� sql.
//
// ��������:
// � �������� ��������� ��������� ��������� ����.
// � �������� ����� � ���� ����� ��������� ����������� ��������� � ������,
// ����������� � /** **/ � ������������ � ������� (:).
// ��������:
//   /**:field_name**/
//
// ��������������:
// ������������ ���������� ������ ������������ = 1 (� ���������� ���������� ������ ��� �����������)
// ������������ � ����������� ���������� RecordCount = 1
// ������������� � �������� ���������: ���
// ������������� � �������� ���������: ��
//
// ����������:
// 1. ��������� ��������� � ������������
// 2. ���������� ����������� ����������� �������� � ��������� ������ (2016-07-26 �� �����������)
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
// ��������� ��� �������� ����� � MS Excel
//---------------------------------------------------------------------------
class TSqlTextField : public TStorageField {    // ��� �������� ��������� dbf-�����
public:
};

//---------------------------------------------------------------------------
// ��������� ��� �������� ������
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
    std::vector<TSqlTextTable> Tables;    // ������ ����� ��� �������
    AnsiString SqlText;

    bool hasChangedAfterAppend;

};


//---------------------------------------------------------------------------
#endif
