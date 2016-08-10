//---------------------------------------------------------------------------
#ifndef StorageH
#define StorageH

#include <vector>
//#include <typeinfo>

class TStorage;  // ����������� ����������

//---------------------------------------------------------------------------
// ����
// TStorageField
//---------------------------------------------------------------------------
class TStorageField {
public:
    bool active;        // ������� ������������� ����������� ���� (���� false, ���� ���������, �� �� �����������)
    bool enable;        // ������� ����, ���������� �� ������ ��������� ��� ���� (���� false, ���� �� ���������)
    String name;    // ��� ���� (�� 10 ��������).
    String name_src;    // ��� ���� �� ���������� ��

//private:
    bool required;      // ������� �������������� ������� ��������������� ���� � ���������
    bool linked;

protected:
    //int FieldType;
    //virtual Copy(TStorageField *Field);
    //GetName();
    //GetLength();
    //GetDecimals();
};

//---------------------------------------------------------------------------
// �������
// TStorageTable
//---------------------------------------------------------------------------
class TStorageTable {
public:
    TStorageTable();
    bool Truncate;
};


//---------------------------------------------------------------------------
// ���������
// class TStorage
//---------------------------------------------------------------------------

class TStorage
{
public:
    friend TStorageField;

    TStorage();
    bool LinkSource(TStorage* Storage);
    bool FindField(AnsiString fieldName);

    virtual void Open(bool ReadOnly = true) {};
    virtual Variant GetFieldValue(TStorageField* Field) {};
    virtual void SetFieldValue(Variant Value) {};
    virtual void Commit() {};
    virtual void Append();   // ��������� ������ ������ ����� (������������ � DBase)
    virtual void Post() {};     // ��������� ������ (������������ � OraProc)
    virtual void Rollback();     // ��������� ������ (������������ � OraProc)

    virtual void Close();

    virtual bool Eot();     // End Of Tables
    virtual bool Eor() {};     // End Of Records
    virtual bool Eof();     // End Of Fields

    virtual void NextTable();
    virtual void NextRecord();
    virtual void NextField();

    virtual TStorageField* GetField();   // ���������� ��� ��������������� ���� � ���������
    bool IsActiveField();
    bool IsLinkedField();
    bool IsActiveTable() { return Active; };
    bool IsModified() { return Modified; };
    //virtual void SetReadOnly(bool ReadOnlyFlag){ this->ReadOnly = ReadOnlyFlag; };
    virtual int GetRecordCount(){ return RecordCount; };
    virtual int GetRecordIndex(){ return RecordIndex; };

    // �������������� �������
    virtual AnsiString GetTable() = 0;     // ���������� ������������ ��������� ��������� ������
    virtual AnsiString getTableStage();     // ���������� ������� ���� ��������� ������
    virtual AnsiString getRecordStage();     // ���������� ������� ���� ��������� ������

    virtual TStorageField* AddField(TStorageField* Field) {return NULL;};
    virtual void LoadFieldDefs() {};    // ��������� �������� �����

    // ������ ����������� �����
    // ������������ ������� ���������������
    virtual CopyFieldsFrom(TStorage* storage);
    virtual CopyFieldsToDbf(TStorage* storage);
    virtual CopyFieldsToExcel(TStorage* storage);
    //virtual CopyFieldsToOraProc(TStorage* storage);

    virtual SetTemplate(TStorage* storage, bool deleteAfterUse = true);

protected:
    void FullCopyFields(TStorage* src, TStorage* dst);
    int TableIndex;
    int TableCount;
    int FieldCount;
    int FieldIndex;
    int RecordCount;
    int RecordIndex;
    bool Active;    // ������� ����, ��� ��������/�������� ������ � ����� � ����������/������ ������
    bool ReadOnly;
    bool Modified;

    std::vector<TStorageField*> Fields;

    TStorage* templateStorage;    // ������ (������� ������ ��������� �������)
    bool delTemplateStorage;

};


//---------------------------------------------------------------------------
#endif
