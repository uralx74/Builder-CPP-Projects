//---------------------------------------------------------------------------
#ifndef StorageH
#define StorageH

#include <vector>
//#include <typeinfo>

class TStorage;  // опережающее объявление

//---------------------------------------------------------------------------
// Поле
// TStorageField
//---------------------------------------------------------------------------
class TStorageField {
public:
    bool active;        // Признак необходимости заполненеия поля (если false, поле создается, но не заполняется)
    bool enable;        // Признак того, необходимо ли вообще учитывать это поле (если false, поле не создается)
    String name;    // Имя поля (до 10 символов).
    String name_src;    // Имя поля из копируемой БД

//private:
    bool required;      // Признак обязательности наличия сопоставленного поля в источнике
    bool linked;

protected:
    //int FieldType;
    //virtual Copy(TStorageField *Field);
    //GetName();
    //GetLength();
    //GetDecimals();
};

//---------------------------------------------------------------------------
// Таблица
// TStorageTable
//---------------------------------------------------------------------------
class TStorageTable {
public:
    TStorageTable();
    bool Truncate;
};


//---------------------------------------------------------------------------
// Хранилище
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
    virtual void Append();   // Добавляет пустую запись перед (используется в DBase)
    virtual void Post() {};     // Фиксирует запись (используется в OraProc)
    virtual void Rollback();     // Фиксирует запись (используется в OraProc)

    virtual void Close();

    virtual bool Eot();     // End Of Tables
    virtual bool Eor() {};     // End Of Records
    virtual bool Eof();     // End Of Fields

    virtual void NextTable();
    virtual void NextRecord();
    virtual void NextField();

    virtual TStorageField* GetField();   // Возвращает имя сопоставленного поля в источнике
    bool IsActiveField();
    bool IsLinkedField();
    bool IsActiveTable() { return Active; };
    bool IsModified() { return Modified; };
    //virtual void SetReadOnly(bool ReadOnlyFlag){ this->ReadOnly = ReadOnlyFlag; };
    virtual int GetRecordCount(){ return RecordCount; };
    virtual int GetRecordIndex(){ return RecordIndex; };

    // Информационные функции
    virtual AnsiString GetTable() = 0;     // Возвращает наименование активного хранилища данных
    virtual AnsiString getTableStage();     // Возвращает текущий этап обработки данных
    virtual AnsiString getRecordStage();     // Возвращает текущий этап обработки данных

    virtual TStorageField* AddField(TStorageField* Field) {return NULL;};
    virtual void LoadFieldDefs() {};    // Загружает описание полей

    // Методы копирования полей
    // Используется двойная диспетчеризация
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
    bool Active;    // Признак того, что источник/приемник открыт и готов к считыванию/записи данных
    bool ReadOnly;
    bool Modified;

    std::vector<TStorageField*> Fields;

    TStorage* templateStorage;    // Шаблон (берется первая доступная таблица)
    bool delTemplateStorage;

};


//---------------------------------------------------------------------------
#endif
