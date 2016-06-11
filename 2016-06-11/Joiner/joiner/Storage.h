//---------------------------------------------------------------------------
#ifndef StorageH
#define StorageH


#include <vector>
#include "Ora.hpp"
#include "dbf.hpp"
#include "Dbf_Lang.hpp"
#include <Db.hpp>
#include <ComCtrls.hpp>
#include "DBAccess.hpp"
#include "MemDS.hpp"


typedef enum {
    SS_EMPTY = 0,
    SS_FILE_NOT_FOUND,  // SS_TABLE_NOT_FOUND
    SS_FILE_IS_OPENED,
    SS_FIELD_NOT_FOUND
} TSTORAGESTATUS;

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
} TDBaseField;

// ��������� ��� �������� ���������� ����� ���������
typedef struct {    // ��� �������� ��������� dbf-�����
    String name;    // ��� ���� (�� 10 ��������).
    bool active;        // ������� ������������� ����������� ���� (���� false, ���� ���������, �� �� �����������)
    bool enable;        // ������� ����, ���������� �� ������ ��������� ��� ���� (���� false, ���� �� ���������)
    String name_src;    // ��� ���� �� ���������� ��
} TOraProcField;


// �������� ���������� ��� ��������� � ����
// ��������� ��� �������� �������
typedef struct {
    AnsiString Server;
    AnsiString Username;
    AnsiString Password;
    AnsiString Table;     // ��� �������
} TOraTableTable;

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
    AnsiString Sql;     // ���� Sql-�������
} TOraSqlTable;


//---------------------------------------------------------------------------
// class TStorage
//---------------------------------------------------------------------------

class TStorage
{
public:
    TStorage();
    //virtual void Create() = 0;
    virtual bool Open(bool ReadOnly = true) {};
    virtual Variant Get(AnsiString Field) {};
    //virtual void Set(AnsiString Field, Variant Value) = 0;
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

    virtual AnsiString GetSrcField() = 0;
    virtual bool IsActiveField() = 0;

    // �������������� �������
    virtual AnsiString GetTable() = 0;     // ���������� ������������ ��������� ���������/��������� ������
    //virtual AnsiString GetRecord() = 0;     //
    virtual TSTORAGESTATUS GetStatus() { return StorageStatus;};     //



    //virtual void dial(char *number) { cout << �????? ?????? � << number << endl; }

protected:
    int TableIndex;
    int TableCount;
    int FieldCount;
    int FieldIndex;
    int RecordCount;
    int RecordIndex;

    TSTORAGESTATUS StorageStatus;
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
    void Create();
    bool Open(bool ReadOnly = true);
    Variant Get(AnsiString Field);
    void Set(Variant Value);
    void Commit();
    void Append();
    void Close();

    //bool Eot();     // End Of Tables
    bool Eor();     // End Of Records
    //bool Eof();     // End Of Fields

    void NextTable();
    void NextRecord();
    //void NextField();

    AnsiString GetSrcField();
    bool IsActiveField();


    void AddTable(AnsiString FileName);

    AnsiString GetTable();

    std::vector<TDBaseField> Fields;
private:
    //AnsiString file;
    std::vector<AnsiString> Tables;    // ������ ����� ��� ������� � ���� DBF
    TDbf* pTable;
};








//---------------------------------------------------------------------------
// class TStorageOra
// base class for Oracle source/destination
//---------------------------------------------------------------------------

class TStorageOra: public TStorage
{
public:
 /*   virtual void Create() = 0;
    virtual void Open() {};
    virtual Variant Get(AnsiString Field) {};
    //virtual void Set(AnsiString Field, Variant Value) = 0;
    virtual void Set(Variant Value) = 0;
    virtual void Commit() {};
    virtual void Append() {};   // ��������� ������ ������ ����� (������������ � DBase)
    virtual void Post() {};     // ��������� ������ (������������ � OraProc)
    virtual void Close() = 0;

    virtual bool Eot() {};     // End Of Tables
    virtual bool Eor() {};     // End Of Records
    virtual bool Eof() = 0;     // End Of Fields

    virtual void NextTable() {};
    virtual void NextRecord() {};
    virtual void NextField() = 0;

    virtual AnsiString GetSrcField() = 0;
    virtual bool IsActiveField() = 0;

    // �������������� �������
    virtual AnsiString GetTable() = 0;     // ���������� ������������ ��������� ���������/��������� ������
    //virtual AnsiString GetRecord() = 0;     //

*/

    //virtual void dial(char *number) { cout << �????? ?????? � << number << endl; }
    void Close();

    //AnsiString Server;
    //AnsiString Username;    // ��� ������������ ��
    //AnsiString Password;    // ������ ������������ ��

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
    TStorageOraProc();
    ~TStorageOraProc();

    //void Create();
    bool Open(bool ReadOnly = true);
    //Variant Get(AnsiString Field);

    void Set(Variant Value);
    //void Commit();
    void Post();

    //bool Eot();     // End Of Tables
    //bool Eor();     // End Of Records
    bool Eof();     // End Of Fields

    //void NextTable();
    //void NextRecord();
    void NextField();

    AnsiString GetSrcField();
    bool IsActiveField();
    void AddTable(const TOraProcTable& Table);
    AnsiString GetTable();



    std::vector<TOraProcField> Fields;      // ������ ����� ���������

private:
    std::vector<TOraProcTable> Tables;    // ������ ����� ��� ������� � ���� DBF

    //int TableIndex;
    //int FieldIndex;

};





//---------------------------------------------------------------------------
// class TStorageOraTable: TStorage
// ����� ������� ���������/��������� ������
// ������������ ������� � �� Oracle
//---------------------------------------------------------------------------
class TStorageOraTable: public TStorageOra

{
public:
    TStorageOraTable();
    ~TStorageOraTable();

    //void Create();
    bool Open(bool ReadOnly);
    Variant Get(AnsiString Field);

    void Set(Variant Value);
    void Append();
    void Commit();
    //void Post();
    //void Close();

    //bool Eot();     // End Of Tables
    bool Eor();     // End Of Records
    //bool Eof();     // End Of Fields

    //void NextTable();
    void NextRecord();
    //void NextField();


    AnsiString GetSrcField();
    bool IsActiveField();

    void AddTable(const TOraTableTable &Table);

    AnsiString GetTable();

 

    //bool Truncate;
    AnsiString Table;


    std::vector<TOraProcField> Fields;

private:
    std::vector<TOraTableTable> Tables;    // ������ ����� ��� ������� � ���� DBF
};




//---------------------------------------------------------------------------
// class TStorageOraSql: TStorage
// ����� ������� ���������/��������� ������
// ������������ ������� � �� Oracle
//---------------------------------------------------------------------------
class TStorageOraSql: public TStorageOra
{
public:
    TStorageOraSql();
    ~TStorageOraSql();

    //void Create();
    bool Open(bool ReadOnly = true);

    Variant Get(AnsiString Field);

    //void Set(Variant Value);
    void Commit();
    void Post();

    bool Eor();     // End Of Records

    void NextTable();
    void NextRecord();

    AnsiString GetSrcField();
    bool IsActiveField();


    void AddTable(const TOraSqlTable &Table);

    AnsiString GetTable();
                
    //bool Truncate;

    std::vector<TOraProcField> Fields;

private:
    std::vector<TOraSqlTable> Tables;    // ������ ����� ��� ������� � ���� DBF
};



//---------------------------------------------------------------------------
#endif
