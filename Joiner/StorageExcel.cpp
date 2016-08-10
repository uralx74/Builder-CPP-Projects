#include "pch.h"
#pragma hdrstop

#include "StorageExcel.h"


//---------------------------------------------------------------------------
//  TStorageExcel
//---------------------------------------------------------------------------
TStorageExcel::~TStorageExcel()
{
    Close();
}

//---------------------------------------------------------------------------
// Добавляет таблицу в список (имя файла, может быть задано маской)
void TStorageExcel::AddTable(const TExcelTable& Table)
{
    TSearchRec SearchRec;
    FindFirst(Table.File, faAnyFile, SearchRec);

    if (SearchRec.Name != "") {     // Если файл найден, то формируем новый элемент-Таблицу
        AnsiString FilePath = ExtractFilePath(Table.File);
        do {
            TExcelTable NewTable;
            NewTable.File = FilePath + SearchRec.Name;
            NewTable.Truncate = Table.Truncate;
            Tables.push_back(NewTable);
            TableCount++;
        } while ( FindNext(SearchRec) == 0);
    } else {    // Если файл не найден, добавляем таблицу как есть
        Tables.push_back(Table);
        TableCount++;
    }
    FindClose(SearchRec);
}

//---------------------------------------------------------------------------
// Открытие таблицы
void TStorageExcel::Open(bool ReadOnly)
{
    this->ReadOnly = ReadOnly;

    // Продумать случаи открытия - создание пустого файла!!!!
    msexcel = new MSExcelWorks();
    if (ReadOnly) {
        if (FileExists(Tables[TableIndex].File)) {
            try {
                msexcel->OpenApplication();
                Workbook = msexcel->OpenDocument(Tables[TableIndex].File);
                Worksheet = msexcel->GetSheet(Workbook, 1);
            } catch (...) {
                throw Exception("Can't to open file  \"" + Tables[TableIndex].File + "\".");
            }
        } else {
            throw Exception("File not found \"" + Tables[TableIndex].File + "\".");
        }
    } else {  // Если открываем как приемник, то в любом случае создаем новый файл
        if (FileExists(Tables[TableIndex].File)) {
            try {
                msexcel->OpenApplication();
                Workbook = msexcel->OpenDocument(Tables[TableIndex].File);
            } catch (...) {
                Close();
                throw Exception("Can't to open file  \"" + Tables[TableIndex].File + "\".");
            }
            if (msexcel->IsReadOnly(Workbook)) {
                Close();
                throw Exception("Can't to open file  \"" + Tables[TableIndex].File + "\" for writing.");
            }
            Worksheet = msexcel->GetSheet(Workbook, 1);

            for(int i = 0; i < Fields.size(); i++) {
                AnsiString sCellValue = msexcel->ReadCell(Worksheet, 1, i+1);
                if (Fields[i]->name != sCellValue) {
                    Close();
                    throw Exception("Field names \"" + Fields[i]->name + "\" and \"" + sCellValue +"\" do not match.");
                }
            }
            if (Tables[TableIndex].Truncate) {
                //msexcel->SetVisibleExcel();
                msexcel->ClearWorksheet(Worksheet);
                // Создаем "структуру" таблицы (шапку)
                for(int i = 0; i < Fields.size(); i++) {
                    msexcel->WriteToCell(Worksheet, Fields[i]->name, 1, i+1, "@");
                }
            }
        } else {
            try {
                msexcel->OpenApplication();
                Workbook = msexcel->OpenDocument();
                Worksheet = msexcel->GetSheet(Workbook, 1);

                // Создаем "структуру" таблицы (шапку)
                for(int i = 0; i < Fields.size(); i++) {
                    msexcel->WriteToCell(Worksheet, Fields[i]->name, 1, i+1, "@");
                }
                msexcel->SaveDocument(Workbook, Tables[TableIndex].File);
                //Modified = true;
            } catch (...) {
                throw Exception("Can't to create file  \"" + Tables[TableIndex].File + "\".");
            }
        }
    }

    // Подсчет столбцов
    if (ReadOnly) {
        int j = 1;
        String sCellValue;
        while ( (sCellValue = msexcel->ReadCell(Worksheet, 1, j)) != "") {
            FieldsList[LowerCase(sCellValue)] = j;
            j++;
        }
        FieldCount = j - 1;
    } else {
        FieldCount = Fields.size();
    }

    // Подсчет строк
    int i = 2;
    String sCellValue;
    while ( (sCellValue = msexcel->ReadCell(Worksheet, i, 1)) != "") {
        i++;
    }
    RecordCount = i - 2;

    Active = true;
}

//---------------------------------------------------------------------------
// Закрывает таблицу
void TStorageExcel::Close()
{
    if (msexcel != NULL) {
        Active = false;
        msexcel->CloseApplication();
        delete msexcel;
        msexcel = NULL;

        // В последующем обернуть эти переменные
        Workbook = Unassigned;   // эквивалентно VarClear(WorkBooks)
        Worksheet = Unassigned;   // эквивалентно VarClear(WorkBooks)
    }
    TStorage::Close();
}

//---------------------------------------------------------------------------
//
bool TStorageExcel::Eor()
{
    return RecordIndex >= RecordCount;
}

//---------------------------------------------------------------------------
// Возвращает наименование активного источника/приемника данных
AnsiString TStorageExcel::GetTable()
{
    if (!Eot()) {
        return Tables[TableIndex].File;
    }
}

//---------------------------------------------------------------------------
// Возвращает значение поля
Variant TStorageExcel::GetFieldValue(TStorageField* Field)
{
    AnsiString fieldName = LowerCase(Field->name_src);
    if (FieldsList.find(fieldName) != FieldsList.end() ) {
        return msexcel->ReadCell(Worksheet, RecordIndex+2, FieldsList[fieldName]);
    } else {
        throw Exception("Field not found " + fieldName + ".");
    }
}

//---------------------------------------------------------------------------
// Переходит к следующей записи таблицы
void TStorageExcel::NextRecord()
{
    if (!Eor()) {
        RecordIndex++;
        FieldIndex = 0;
    }
}

//---------------------------------------------------------------------------
// Добавляет новую запись в таблицу
void TStorageExcel::Append()
{
    RecordIndex = RecordCount;
    TStorage::Append();
}

//---------------------------------------------------------------------------
// Устанавливает значение активного поля
void TStorageExcel::SetFieldValue(Variant Value)
{
    if (Fields[FieldIndex]->active && Fields[FieldIndex]->enable) {
        msexcel->WriteToCell(Worksheet, Value, RecordIndex+2, FieldIndex+1, ((TExcelField*)Fields[FieldIndex])->format);
    }
}

//---------------------------------------------------------------------------
// Добавляет новое поле в список полей
TExcelField* TStorageExcel::AddField()
{
    TExcelField* Field = new TExcelField();
    Fields.push_back(dynamic_cast<TStorageField*>(Field));
    FieldCount++;
    return Field;
}

//---------------------------------------------------------------------------
// Фиксирует изменения (сохраняет файл)
void TStorageExcel::Commit()
{
    //Здесь сделать 
    if (ReadOnly) {
        throw Exception("Can't commit the storage because it is read-only.");
        //return;
    }

    msexcel->SaveDocument(Workbook);
    Modified = true;
}

//---------------------------------------------------------------------------
// Копирование полей из источника
// Используется двойная диспетчеризация
TStorageExcel::CopyFieldsFrom(TStorage* storage)
{
    storage->CopyFieldsToExcel(this);
}

//---------------------------------------------------------------------------
// Полное копирование полей из Excel в Excel
TStorageExcel::CopyFieldsToExcel(TStorage* storage)
{
    TStorage::FullCopyFields(this, storage);
}



