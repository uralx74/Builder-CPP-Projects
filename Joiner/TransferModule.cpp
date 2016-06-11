#include "TransferModule.h"

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TTransferModule::TTransferModule()
{
    //Logger = Singleton::getInstance();

}

//---------------------------------------------------------------------------
// Запуск
void __fastcall TTransferModule::Start()
{
    Logger = &TLogger::getInstance();
    CommandLine = &TCommandLine::getInstance();

    if (LoadParameters()) {
        Transfer(SrcStor, DstStor);
    }
}

//---------------------------------------------------------------------------
//
void __fastcall TTransferModule::Transfer(TStorage* Src, TStorage* Dst)
{
    Logger->WriteLog("Процедура копирования данных запущена.");
    TDateTime StartTime = Now();

    try {
        Dst->Open();   // Открываем приемник
    } catch (Exception &e) {
        Logger->WriteLog("Ошибка в приемнике \"" + Dst->GetTable() + "\". " + e.Message);
        Dst->Close();
        Logger->WriteLog("Процедура копирования данных завершена.");
        return;
    }

    try {
        Src->Open();    // Открываем источник, если ошибка, то далее в цикле выбирается следующий источник
    } catch (Exception &e) {
        Logger->WriteLog("Ошибка в источнике \"" + Src->GetTable()+ "\" (" + Src->GetTableStage() + "). " + e.Message);
    }


    int DstRecordCountFirst = Dst->GetRecordCount();

    while(!Src->Eot()) { // Цикл по таблицам

        int log_n;

        if (!Src->IsActiveTable()) {
            try {
                Src->NextTable();
                if (Src->Eot())
                    break;
            } catch (Exception &e) {
                Logger->WriteLog("Ошибка в источнике \"" + Src->GetTable() + "\". " + e.Message);
                continue;
            }
        }

        // Если источник открыт успешно
        log_n = Logger->WriteLog("Загружается > \"" + Src->GetTable() + "\" (" + Src->GetTableStage() + ")");

        try {
            int DstRecordCount = Dst->GetRecordCount();
            while(!Src->Eor()) {    // Цикл по строкам
                //Logger->WriteLog("Загружается > " + Src->GetTable() + " " + Src->GetStage(), log_n);
                Dst->Append();

                while(!Dst->Eof()) {    // Цикл по столбцам в приемнике
                    if (Dst->IsActiveField()) {
                        try {
                            Variant Value = Src->Get( Dst->GetSrcField() );
                            Dst->Set(Value);
                        } catch (Exception &e) {
                            Logger->WriteLog("Ошибка в источнике \"" + Src->GetTable() + "\" (" + Src->GetTableStage() + ")" + ". " + e.Message /*+ Dst->GetSrcField()*/, log_n);
                            throw Exception("");
                        }
                    }
                    Dst->NextField();
                }

                Dst->Post();  // Фиксирует строку
                Src->NextRecord();
            }

            Dst->Commit();

            // Проверять, был ли хоть один Commit в приемнике
            // может быть использовать RecordIndex или RecordCount
            int LoadedRecordsCount = Dst->GetRecordCount() - DstRecordCount;

            Logger->WriteLog("Загружено " + IntToStr(LoadedRecordsCount) + " записей из \"" + Src->GetTable() + "\"", log_n);
            DstRecordCount = Dst->GetRecordCount();

        } catch (Exception &e) {
            if (e.Message != "")
                Logger->WriteLog("Непредвиденая ошибка. Источник: \"" + Src->GetTable() + "\" (" + Src->GetTableStage() + "), приемник \"" + Dst->GetTable()+ "\". " + e.Message /*+ Dst->GetSrcField()*/);
        }

        try {
            Src->NextTable();
        } catch (Exception &e) {
            Logger->WriteLog("Ошибка в источнике \"" + Src->GetTable() + "\" (" + Src->GetTableStage() + "). " + e.Message);
            continue;
        }
    }

    // Формируем строку - секундомер
    TDateTime StopTime = Now() ;
    int TotalSec = SecsPerDay * (StopTime - StartTime);
    int hh = (TotalSec ) / 3600;
    int mm = (TotalSec - hh * 3600) / 60;
    int ss = TotalSec % 60;
    AnsiString sTotalTime = IntToStr(ss) + " сек";
    if (mm > 0)
        sTotalTime = IntToStr(mm) + " мин " + sTotalTime;
    if (hh > 0)
        sTotalTime = IntToStr(hh) + " час " + sTotalTime;

 
    int DstRecordCountTotal = Dst->GetRecordCount() - DstRecordCountFirst;

    // Проверять, был ли хоть один Commit в приемнике
    // может быть использовать RecordIndex или RecordCount
    if (Dst->IsModified())
        Logger->WriteLog("Всего загружено " + IntToStr(DstRecordCountTotal) + " записей за " + sTotalTime+ ". Результат сохранен в \"" + Dst->GetTable() + "\"");

        
    Src->Close();
    Dst->Close();

    Logger->WriteLog("Процедура копирования данных завершена.");
}

//---------------------------------------------------------------------------
// Загружает параметры из командной строки
// и файла конфигурации
bool __fastcall TTransferModule::LoadParameters()
{
    // Параметры командной строки
    AnsiString clConfig;    // Путь к файлу конфигурации
    AnsiString clSrcUsername;  // Имя пользователя базы данных
    AnsiString clSrcPassword;  // Пароль к базе данных
    AnsiString clDstUsername;  // Имя пользователя базы данных
    AnsiString clDstPassword;  // Пароль к базе данных

    clConfig = CommandLine->GetValue("-config","-c");
    clDstUsername = CommandLine->GetValue("-dstuser","-du");
    clDstPassword = CommandLine->GetValue("-dstpassword","-dp");
    clSrcUsername = CommandLine->GetValue("-srcuser","-su");
    clSrcPassword = CommandLine->GetValue("-srcpassword","-sp");
    //clLogfile = CommandLine->GetValue("-logfile","-l");
    //clSilent = CommandLine->GetValue("-silent","-s");

    // Преобразовываем относительный путь к файлу в абсолютный путь
    clConfig = ExpandFileName(clConfig);

    int LogLine = Logger->WriteLog("Загрузка файла конфигурации > " + clConfig);

    if (!FileExists(clConfig)) {
        Logger->WriteLog("Ошибка. Файл конфигурации не найден " + clConfig, LogLine);
        return false;
    }

    CoInitialize(NULL);
    MSXMLWorks msxml;
    msxml.LoadXMLFile(clConfig);

    if (msxml.GetParseError() != "") {
        Logger->WriteLog("Ошибка: " + msxml.GetParseError());
        return false;
    }

    Variant RootNode = msxml.GetRootNode();
    Variant node = msxml.GetFirstNode(RootNode);

    while (!node.IsEmpty())
    {
        String sNodeName = msxml.GetNodeName(node);
        if (sNodeName == "import")  {
            Variant subnode = msxml.GetFirstNode(node);
            AnsiString sImportType = msxml.GetNodeName(subnode);

            if (sImportType == "dbase4") {
                TStorageDbase* StorDbase = new TStorageDbase();
                SrcStor = (TStorage*) StorDbase;

                while (!subnode.IsEmpty()) {
                    if (msxml.GetNodeName(subnode) == "dbase4") {
                        TDbaseTable Table;
                        Table.File = ExpandFileName(msxml.GetAttributeValue(subnode, "file"));  // Имя файла может быть задано маской
                        StorDbase->AddTable(Table);
                    }
                    subnode = msxml.GetNextNode(subnode);
                }
            } else if (sImportType == "orasql" || sImportType == "oratable") {
                TStorageOraSql* StorOraSql = new TStorageOraSql();
                SrcStor = (TStorage*) StorOraSql;

                while (!subnode.IsEmpty()) {
                    if (msxml.GetNodeName(subnode) == "orasql" || sImportType == "oratable") {
                        TOraSqlTable Table;
                        Table.Server = msxml.GetAttributeValue(subnode, "server");

                        // В дальнейшем переделать. 1. Командная строка приоритетней; 2. Шифрование-дешифрование
                        // Тоже самое в OraProc и в приемник

                        // Если задано имя пользователя и пароль в командной строке
                        if (clSrcUsername != "" || clSrcPassword != "") {
                            Table.Username = clSrcUsername;
                            Table.Password = clSrcPassword;
                        } else {
                            AnsiString code = msxml.GetAttributeValue(subnode, "code");
                            if (code != "") {
                                TEncoder encoder;
                                try {
                                    encoder.Decode(code, Table.Username, Table.Password );
                                } catch (...) {
                                    Logger->WriteLog("Ошибка. Не удалось расшифровать пароль к источнику \"" + Table.Server + "\"");
                                    return false;
                                }
                            } else {    // Если задано не зашифрованное значение
                                Table.Username = msxml.GetAttributeValue(subnode, "username");
                                Table.Password = msxml.GetAttributeValue(subnode, "password", clSrcPassword);
                            }
                        }


                        if (sImportType == "orasql")
                            Table.Sql = ExpandFileName(msxml.GetAttributeValue(subnode, "sql"));
                        Table.Table = msxml.GetAttributeValue(subnode, "table");
                        StorOraSql->AddTable(Table);
                    }
                    subnode = msxml.GetNextNode(subnode);
                }
            } else if (sImportType == "excel") {
                TStorageExcel* StorageExcel = new TStorageExcel();
                SrcStor = (TStorage*) StorageExcel;

                while (!subnode.IsEmpty()) {
                    if (msxml.GetNodeName(subnode) == "excel") {
                        TExcelTable Table;
                        Table.File = ExpandFileName(msxml.GetAttributeValue(subnode, "file"));  // Имя файла может быть задано маской
                        StorageExcel->AddTable(Table);
                    }
                    subnode = msxml.GetNextNode(subnode);
                }
            }
        } else if (sNodeName == "export") {

            Variant subnode = msxml.GetFirstNode(node);
            AnsiString sExportType = msxml.GetNodeName(subnode);
            if (sExportType == "dbase4") {

                TStorageDbase* StorDbase = new TStorageDbase();
                DstStor = (TStorage*) StorDbase;
                DstStor->SetReadOnly(false);

                TDbaseTable Table;
                Table.File = ExpandFileName(msxml.GetAttributeValue(subnode, "file"));
                Table.Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                StorDbase->AddTable(Table);

                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TDbaseField field;
                        field.type = LowerCase(msxml.GetAttributeValue(node_fields, "type"));
                        field.name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                        field.length = msxml.GetAttributeValue(node_fields, "length", 0);
                        field.decimals = msxml.GetAttributeValue(node_fields, "decimals", 0);
                        field.active = msxml.GetAttributeValue(node_fields, "active", true);
                        field.enable = msxml.GetAttributeValue(node_fields, "enable", true);
                        field.name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", field.name));
                        if (field.name_src == "" )
                            field.name_src = field.name;
                        StorDbase->AddField(field); // AddField
                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }
            } else if (sExportType == "oraproc") {   //
                TStorageOraProc* StorOraProc = new TStorageOraProc();
                DstStor = (TStorage*) StorOraProc;

                TOraProcTable Table;
                Table.Server = msxml.GetAttributeValue(subnode, "server");

                //!!! Фрагмент дублируется - необходим рефакторинг!
                // Если задано имя пользователя и пароль в командной строке
                if (clSrcUsername != "" || clSrcPassword != "") {
                    Table.Username = clSrcUsername;
                    Table.Password = clSrcPassword;
                } else {
                    AnsiString code = msxml.GetAttributeValue(subnode, "code");
                    if (code != "") {
                        TEncoder encoder;
                        try {
                            encoder.Decode(code, Table.Username, Table.Password );
                        } catch (...) {
                            Logger->WriteLog("Ошибка. Не удалось расшифровать пароль к источнику \"" + Table.Server + "\"");
                            return false;
                        }
                    } else {    // Если задано не зашифрованное значение
                        Table.Username = msxml.GetAttributeValue(subnode, "username");
                        Table.Password = msxml.GetAttributeValue(subnode, "password", clSrcPassword);
                    }
                }

                Table.Procedure = msxml.GetAttributeValue(subnode, "procedure");
                Table.Table = msxml.GetAttributeValue(subnode, "table");
                Table.Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                StorOraProc->AddTable(Table);


                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TOraField field;
                        field.name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                        field.active = msxml.GetAttributeValue(node_fields, "active", true);
                        field.enable = msxml.GetAttributeValue(node_fields, "enable", true);
                        field.name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", field.name));
                        if (field.name_src == "" )
                            field.name_src = field.name;

                        StorOraProc->AddField(field);   // AddField
                        //ExportParamsOraProc.Fields.push_back(field);

                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }

            } else if (sExportType == "oratable") {   // Приемник - Таблица в БД Oracle

                TStorageOraSql* StorOraTable = new TStorageOraSql();
                DstStor = (TStorage*) StorOraTable;
                DstStor->SetReadOnly(false);

                TOraSqlTable Table;
                Table.Server = msxml.GetAttributeValue(subnode, "server");


                // Если задано имя пользователя и пароль в командной строке
                if (clSrcUsername != "" || clSrcPassword != "") {
                    Table.Username = clSrcUsername;
                    Table.Password = clSrcPassword;
                } else {
                    AnsiString code = msxml.GetAttributeValue(subnode, "code");
                    if (code != "") {
                        TEncoder encoder;
                        try {
                            encoder.Decode(code, Table.Username, Table.Password );
                        } catch (...) {
                            Logger->WriteLog("Ошибка. Не удалось расшифровать пароль к источнику \"" + Table.Server + "\"");
                            return false;
                        }
                    } else {    // Если задано не зашифрованное значение
                        Table.Username = msxml.GetAttributeValue(subnode, "username");
                        Table.Password = msxml.GetAttributeValue(subnode, "password", clSrcPassword);
                    }
                }

                Table.Sql = ""; // Важно!
                Table.Table = msxml.GetAttributeValue(subnode, "table");
                Table.Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                StorOraTable->AddTable(Table);

                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TOraField field;
                        field.name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                        field.active = msxml.GetAttributeValue(node_fields, "active", true);
                        field.enable = msxml.GetAttributeValue(node_fields, "enable", true);
                        field.name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", field.name));

                        StorOraTable->AddField(field);      // Возможно заменить на
                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }
            } else if (sExportType == "excel") {

                TStorageExcel* StorageExcel = new TStorageExcel();
                DstStor = (TStorage*) StorageExcel;
                DstStor->SetReadOnly(false);

                TExcelTable Table;
                Table.File = ExpandFileName(msxml.GetAttributeValue(subnode, "file"));
                Table.Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                StorageExcel->AddTable(Table);

                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TExcelField field;
                        field.name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                        field.active = msxml.GetAttributeValue(node_fields, "active", true);
                        field.enable = msxml.GetAttributeValue(node_fields, "enable", true);
                        field.name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", field.name));
                        if (field.name_src == "" )
                            field.name_src = field.name;
                        StorageExcel->AddField(field); // AddField
                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }
            }
        }
        node = msxml.GetNextNode(node);
    }

    Logger->WriteLog("Файл конфигурации загружен " + clConfig, LogLine);
    return true;

}


