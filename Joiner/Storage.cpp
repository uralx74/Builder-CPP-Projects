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
// Возвращает соответствующее активному полю - имя поля в таблице-источнике
TStorageField* TStorage::GetField()
{
    return Fields[FieldIndex];
    //return Fields[FieldIndex]->name_src;
}

//---------------------------------------------------------------------------
// Возвращает true - если поле поле подлежит записи
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
// Задает templateStorage в качестве источника структуры
// Используется когда Хранилище представляет приемник
TStorage::SetTemplate(TStorage* storage, bool deleteAfterUse)
{
    templateStorage = storage;
    delTemplateStorage = deleteAfterUse;
}


//---------------------------------------------------------------------------
// Тестирование 2
TStorage::CopyFieldsFrom(TStorage* storage)
{
    // Конвертация
}

//
TStorage::CopyFieldsToDbf(TStorage* storage)
{
    // Конвертация
    Modified = true;
}

//
TStorage::CopyFieldsToExcel(TStorage* storage)
{
    // Конвертация
    bool Modified = true;
}



//---------------------------------------------------------------------------
// Полное копирование полей
// Вызывается только для источников одинакового типа!
void TStorage::FullCopyFields(TStorage* src, TStorage* dst)
{
    // Копируем из src в dst
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
// Возвращает true если достигнут конец списка таблиц
bool TStorage::Eot()
{
    return TableIndex >= TableCount;
}

//---------------------------------------------------------------------------
// Возвращает true если достигнут конец списка полей
bool TStorage::Eof()
{
    return FieldIndex  >= FieldCount;
}

//---------------------------------------------------------------------------
// Переходит к следующей таблице
void TStorage::NextTable()
{
    if (!Eot()) {
        TableIndex ++;
        FieldIndex = 0;
    }
}

//---------------------------------------------------------------------------
// Переходит к следующей записи таблицы
void TStorage::NextRecord()
{
    FieldIndex = 0;
    RecordIndex++;
}

//---------------------------------------------------------------------------
// Переходит к следующему полю таблицы
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





