//---------------------------------------------------------------------------
#ifndef StorageOraH
#define StorageOraH

#include <vector>
#include <map>
#include "Ora.hpp"
#include "DBAccess.hpp"
#include "MemDS.hpp"
#include "Storage.h"


// ��������� ��� �������� ���������� ����� � Oracle (���������/�������)
class TOraField : public TStorageField {
};

class TStorageTableOra : public TStorageTable
{
public:
    AnsiString Table;       // ��� ������� (������������ ���� ����� �������������� ��������)
    AnsiString Server;
    AnsiString Username;
    AnsiString Password;
};



// ��������� ��� �������� ����������
class TOraProcTable : public TStorageTableOra
{
public:
    AnsiString Procedure;   // ��� ���������
};

// ��������� ��� �������� ����������
class TOraSqlTable : public TStorageTableOra
{
public:
    AnsiString Sql;     // ��� ����� � Sql-��������
};





//---------------------------------------------------------------------------
// class TStorageOra
// base class for Oracle source/destination
//---------------------------------------------------------------------------

class TStorageOra: public TStorage
{
protected:
    void Close();

protected:
    ~TStorageOra();

    void OpenConnection(AnsiString Server, AnsiString Username, AnsiString Password);
    TOraSession* dbSession;
    TOraQuery* dbQuery;
    void TruncateTable(TStorageTableOra* Table);
    void PrepareQuery();


};

//---------------------------------------------------------------------------
// class TStorageOraProc: TStorage
// ����� ������� ��������� ������
// ������������ ��������� � �� Oracle
//---------------------------------------------------------------------------
class TStorageOraProc: public TStorageOra
{
public:
    TStorageOraProc() {};
    void Open(bool ReadOnly = true);
    //Variant Get(AnsiString Field);
    void SetFieldValue(Variant Value);
    void Post();
    void NextField();
    //AnsiString GetSrcField();
    //bool IsActiveField();
    TOraField* AddField();
    //void AddField(const TOraField& Field);
    void AddTable(const TOraProcTable& Table);
    AnsiString GetTable();
private:
    std::vector<TOraProcTable> Tables;    // ������ ������ (��������)
};



//---------------------------------------------------------------------------
// class TStorageOraSql: TStorage
// ����� ������� ���������/��������� ������
// ������������ ������� � �� Oracle
//---------------------------------------------------------------------------
class TStorageOraSql: public TStorageOra
{
public:
    TStorageOraSql() {};
    void Open(bool ReadOnly = true);
    //Variant Get(AnsiString Field);      // ���������� �������� �� ����� ����
    Variant GetFieldValue(TStorageField* Field);
    //void Set(Variant Value);      // ������������� �������� ��������� ����
    void SetFieldValue(Variant Value);      // ������������� �������� ��������� ����
    void Append();
    void Commit();
    void Post();
    bool Eor();     // End Of Records
    void NextTable();
    void NextRecord();
    //AnsiString GetSrcField();
    //bool IsActiveField();
    TOraField* AddField();
    void AddTable(const TOraSqlTable &Table);
    AnsiString GetTable();
    //bool FindField(AnsiString fieldName);

private:
    std::vector<TOraSqlTable> Tables;    // ������ ����� ��� ������� � ���� DBF
};







//---------------------------------------------------------------------------
#endif
