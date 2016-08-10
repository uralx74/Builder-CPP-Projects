//---------------------------------------------------------------------------
#ifndef StorageDbfH
#define StorageDbfH

#include <vector>
#include <map>
#include "Ora.hpp"
#include "dbf.hpp"
#include "Dbf_Lang.hpp"
#include "DBAccess.hpp"
#include "MemDS.hpp"
#include "Storage.h"

class TStorageDbf;

//---------------------------------------------------------------------------
// ��������� ��� �������� ���������� ���� (�������) DBASE
class TDbaseField : public TStorageField {    // ��� �������� ��������� dbf-�����
public:
    //TDbaseField() {};
    //TDbaseField(TDbaseField* Field);
    //TDbaseField(TStorageField* Field);

    char type;    // ��� fieldtype is a single character [C,D,F,L,M,N]
    int length;         // ����� ����
    int decimals;       // ����� ���������� �����

    // Character 	 1-255
    // Date	  8
    // Logical	  1
    // Memo	  10
    // Numeric	1-30
    // Decimals is 0 for non-numeric, number of decimals for numeric.

};

// ��������� ��� �������� ����������
class TDbaseTable: public TStorageTable
{
public:
    AnsiString File;
};


//---------------------------------------------------------------------------
// class TStorageDbase: TStorage
// ����� ������� ���������/��������� ������
// ������������ ���� DBase4
//---------------------------------------------------------------------------
class TStorageDbase: public TStorage
{
protected:
    void Close();
public:
    TStorageDbase();
    TStorageDbase(String fileName);
    void Open(bool ReadOnly = true);
    Variant GetFieldValue(TStorageField* Field);
    void SetFieldValue(Variant Value);
    void Commit();
    void Append();


    bool Eor();     // End Of Records

    void NextTable();
    void NextRecord();
    //void NextField();

    TDbaseField* AddField();
    TStorageField* AddField(TStorageField* Field);
    void AddTable(const TDbaseTable& Table);

    std::vector<TStorageField> GetFieldDefs();
    void SetFieldDefs(std::vector<TStorageField>);

    AnsiString GetTable();

    void LoadFieldDefs();


    // ������������
    //CopyDbaseFields(TDbaseField* field);


    // ������������ 2
    CopyFieldsFrom(TStorage* storage);
    CopyFieldsToDbf(TStorage* storage);



private:
    void Create();  // ������� ���� DBF
    std::vector<TDbaseTable> Tables;    // ������ ������
    TDbf* pTable;   // ������� ���������
};




//---------------------------------------------------------------------------
#endif
