//---------------------------------------------------------------------------
// Storage.h
// author: vsovchinnikov
//---------------------------------------------------------------------------
#ifndef StorageH
#define StorageH

#include <vector>
#include <map>
#include "Ora.hpp"
#include "dbf.hpp"
#include "Dbf_Lang.hpp"
#include <Db.hpp>
#include <ComCtrls.hpp>
#include "DBAccess.hpp"
#include "MemDS.hpp"
#include "c:\PROGRS\current\util\MSExcelWorks.h"
      

//---------------------------------------------------------------------------
// ��������� ��� �����
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// ��������� ��� �������� ���������� ���� (�������) DBASE
typedef struct {    // ��� �������� ��������� dbf-�����
    String type;    // ��� fieldtype is a single character [C,D,F,L,M,N]
    String name;    // ��� ���� (�� 10 ��������).
    int length;         // ����� ����
    int decimals;       // ����� ���������� �����
    bool active;        // ������� ������������� ����������� ���� (���� false, ���� ���������, �� �� �����������)
    bool enable;        // ������� ����, ���������� �� ������ ��������� ��� ���� (���� false, ���� �� ���������)
    String name_src;    // ��� ���� �� ���������� ��

    // Character 	 1-255
    // Date	  8
    // Logical	  1
    // Memo	  10
    // Numeric	1-30
    // Decimals is 0 for non-numeric, number of decimals for numeric.
} TDbaseField;

// ��������� ��� �������� ���������� ����� � Oracle (���������/�������)
typedef struct {    // ��� �������� ��������� dbf-�����
    String name;    // ��� ���� (�� 10 ��������).
    bool active;        // ������� ������������� ����������� ���� (���� false, ���� ���������, �� �� �����������)
    bool enable;        // ������� ����, ���������� �� ������ ��������� ��� ���� (���� false, ���� �� ���������)
    String name_src;    // ��� ���� �� ���������� ��
} TOraField;

//---------------------------------------------------------------------------
// ��������� ��� �������� ����� � MS Excel
typedef struct {    // ��� �������� ��������� dbf-�����
    String name;    // ��� ���� (�� 10 ��������).
    bool active;        // ������� ������������� ����������� ���� (���� false, ���� ���������, �� �� �����������)
    bool enable;        // ������� ����, ���������� �� ������ ��������� ��� ���� (���� false, ���� �� ���������)
    String name_src;    // ��� ���� �� ���������� ��
    String format;      // ������ ������ � MS Excel
} TExcelField;




//---------------------------------------------------------------------------
// ��������� ��� �������� ������
//---------------------------------------------------------------------------

// ��������� ��� �������� ����������
typedef struct {
    AnsiString File;
    bool Truncate;
} TDbaseTable;

// ��������� ��� �������� ����������
typedef struct {
    AnsiString Server;
    AnsiString Username;
    AnsiString Password;
    AnsiString Procedure;   // ��� ���������
    AnsiString Table;       // ��� ������� (������������ ���� ����� �������������� ��������)
    bool Truncate;
} TOraProcTable;

// ��������� ��� �������� ����������
typedef struct {
    AnsiString Server;
    AnsiString Username;
    AnsiString Password;
    AnsiString Sql;     // ��� ����� � Sql-��������
    AnsiString Table;   // ��� ������� (������������ ���� ����� �������������� ��������)
    bool Truncate;
} TOraSqlTable;

// ��������� ��� �������� ����������
typedef struct {
    AnsiString File;
    bool Truncate;
} TExcelTable;

//---------------------------------------------------------------------------
// class TStorage
//---------------------------------------------------------------------------

class TStorage
{
public:
    TStorage();
    virtual void Open() {};
    virtual Variant Get(AnsiString Field) {};
    virtual void Set(Variant Value) {};
    virtual void Commit() {};
    virtual void Append() {};   // ��������� ������ ������ ����� (������������ � DBase)
    virtual void Post() {};     // ��������� ������ (������������ � OraProc)
    virtual void Close() = 0;

    virtual bool Eot();     // End Of Tables
    virtual bool Eor() {};     // End Of Records
    virtual bool Eof();     // End Of Fields

    virtual void NextTable();
    virtual void NextRecord() {};
    virtual void NextField();

    virtual AnsiString GetSrcField() = 0;   // ���������� ��� ��������������� ���� � ���������
    virtual bool IsActiveField() = 0;
    virtual bool IsActiveTable() { return Active; };
    virtual bool IsModified() { return Modified; };
    virtual void SetReadOnly(bool ReadOnlyFlag){ this->ReadOnly = ReadOnlyFlag; };
    virtual int GetRecordCount(){ return RecordCount; };
     
    // �������������� �������
    virtual AnsiString GetTable() = 0;     // ���������� ������������ ��������� ��������� ������
    virtual AnsiString GetTableStage();     // ���������� ������� ���� ��������� ������
      
protected:
    int TableIndex;
    int TableCount;
    int FieldCount;
    int FieldIndex;
    int RecordCount;
    int RecordIndex;
    bool Active;    // ������� ����, ��� ��������/�������� ������ � ����� � ����������/������ ������
    bool ReadOnly;
    bool Modified;
    //TSTORAGESTATUS StorageStatus;
};



//---------------------------------------------------------------------------
// class TStorageDbase: TStorage
// ����� ������� ���������/��������� ������
// ������������ ���� DBase4
//---------------------------------------------------------------------------
class TStorageDbase: TStorage
{
public:
    TStorageDbase();
    void Open();
    Variant Get(AnsiString Field);
    void Set(Variant Value);
    void Commit();
    void Append();
    void Close();

    bool Eor();     // End Of Records

    void NextTable();
    void NextRecord();
    //void NextField();

    AnsiString GetSrcField();
    bool IsActiveField();

    void AddField(const TDbaseField& Field);
    void AddTable(const TDbaseTable& Table);

    AnsiString GetTable();
private:
    void Create();  // ������� ���� DBF
    std::vector<TDbaseTable> Tables;    // ������ ����� ��� ������� � ���� DBF
    std::vector<TDbaseField> Fields;

    TDbf* pTable;   // ������� ���������
};



//---------------------------------------------------------------------------
// class TStorageOra
// base class for Oracle source/destination
//---------------------------------------------------------------------------

class TStorageOra: public TStorage
{
public:
    void Close();
    
protected:
    ~TStorageOra();

protected:
    void OpenConnection(AnsiString Server, AnsiString Username, AnsiString Password);
    TOraSession* OraSession;
    TOraQuery* OraQuery;

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
    void Open();
    //Variant Get(AnsiString Field);
    void Set(Variant Value);
    void Post();
    void NextField();
    AnsiString GetSrcField();
    bool IsActiveField();
    void AddField(const TOraField& Field);
    void AddTable(const TOraProcTable& Table);
    AnsiString GetTable();
private:
    std::vector<TOraField> Fields;      // ������ ����� ���������
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
    void Open();
    Variant Get(AnsiString Field);      // ���������� �������� �� ����� ����
    void Set(Variant Value);      // ������������� �������� ��������� ����
    void Append();
    void Commit();
    void Post();
    bool Eor();     // End Of Records
    void NextTable();
    void NextRecord();
    AnsiString GetSrcField();
    bool IsActiveField();
    void AddField(const TOraField& Field);
    void AddTable(const TOraSqlTable &Table);
    AnsiString GetTable();
private:
    std::vector<TOraField> Fields;
    std::vector<TOraSqlTable> Tables;    // ������ ����� ��� ������� � ���� DBF
};


//---------------------------------------------------------------------------
// class TStorageExcel: TStorage
// ����� ������� ���������/��������� ������
// ������������ �������, �������� � ����� MS Excel
//---------------------------------------------------------------------------

class TStorageExcel: TStorage
{
public:
    TStorageExcel(){};
    ~TStorageExcel();
    void Open();
    Variant Get(AnsiString Field);
    void Set(Variant Value);
    void Commit();
    void Append();
    void Close();

    //bool Eot();     // End Of Tables
    bool Eor();     // End Of Records
    //bool Eof();     // End Of Fields

    //void NextTable();
    void NextRecord();
    //void NextField();


    AnsiString GetSrcField();
    bool IsActiveField() {};


    void AddField(const TExcelField& Field);
    void AddTable(const TExcelTable& Table);

    AnsiString GetTable();

private:
    void OpenExcel();

    std::vector<TExcelField> Fields;
    std::vector<TExcelTable> Tables;    // ������ ����� ��� �������

    MSExcelWorks* msexcel;
    Variant Worksheet;
    Variant Workbook;
    Variant Range;
    std::map<AnsiString, int> FieldsList;   // ������ �����
             
};


//---------------------------------------------------------------------------
#endif
