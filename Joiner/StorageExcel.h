//---------------------------------------------------------------------------
#ifndef StorageExcelH
#define StorageExcelH

#include <vector>
#include <map>
//#include "MemDS.hpp"
#include "Storage.h"

#include "c:\PROGRS\current\util\MSExcelWorks.h"


class TStorageExcel;


//---------------------------------------------------------------------------
// ��������� ��� �������� ����� � MS Excel
class TExcelField : public TStorageField {    // ��� �������� ��������� dbf-�����
public:
    String format;      // ������ ������ � MS Excel
};

// ��������� ��� �������� ����������
class TExcelTable : public TStorageTable
{
public:
    AnsiString File;
};

//---------------------------------------------------------------------------
// class TStorageExcel: TStorage
// ����� ������� ���������/��������� ������
// ������������ �������, �������� � ����� MS Excel
//---------------------------------------------------------------------------

class TStorageExcel: TStorage
{
protected:
    void Close();

public:
    TStorageExcel(){};
    ~TStorageExcel();
    void Open(bool ReadOnly = true);
    Variant GetFieldValue(TStorageField* Field);
    void SetFieldValue(Variant Value);
    void Commit();
    void Append();


    //bool Eot();     // End Of Tables
    bool Eor();     // End Of Records
    //bool Eof();     // End Of Fields

    //void NextTable();
    void NextRecord();
    //void NextField();


    TExcelField* AddField();
    void AddTable(const TExcelTable& Table);

    AnsiString GetTable();

    CopyFieldsFrom(TStorage* storage);

private:
    CopyFieldsToExcel(TStorage* storage);

    std::vector<TExcelTable> Tables;    // ������ ����� ��� �������

    MSExcelWorks* msexcel;
    Variant Worksheet;
    Variant Workbook;
    Variant Range;
    std::map<AnsiString, int> FieldsList;   // ������ �����
};



//---------------------------------------------------------------------------
#endif
