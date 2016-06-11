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
    Logger = &Singleton::getInstance();

    if (LoadParameters()) {
        Transfer(SrcStor, DstStor);
    }
}

//---------------------------------------------------------------------------
//
void __fastcall TTransferModule::Transfer(TStorage* Src, TStorage* Dst)
{
    Logger->WriteLog("Процедура копирования данных запущена.");

    Src->Open(true);    // Открываем только на чтение
    Dst->Open(false);   // Открываем На запись

    while(!Src->Eot()) {

        int log_n;
        switch (Src->GetStatus()) {
        case SS_EMPTY: {
                break;
            }
        case SS_FILE_NOT_FOUND: {
                Logger->WriteLog("Файл не найден " + Src->GetTable());
                Src->NextTable();
                continue;
                //break;
            }
        case SS_FILE_IS_OPENED: {
                log_n = Logger->WriteLog("Загружается > " + Src->GetTable());
                break;
            }
        }

        while(!Src->Eor()) {
            Dst->Append();

            while(!Dst->Eof()) {
                //AnsiString ssssss = Dst->GetSrcField();

                if (Dst->IsActiveField()) {

                    try {
                        Variant val = Src->Get( Dst->GetSrcField() );
                        Dst->Set(val);
                    } catch (...) {
                        break;
                        //Logger->WriteLog("Ошибка. Поле не найдено в источнике " + Dst->GetSrcField());
                    }

                    //Dst->Set( Src->Get( Dst->GetSrcField() ));
                }
                Dst->NextField();
            }

            // Переделать - должно быть два статуса - 1 общий (ок / ошибка), 2-наименование ошибки
            if (Src->GetStatus() == SS_FIELD_NOT_FOUND) {
                Logger->WriteLog("Ошибка. Поле \"" + Dst->GetSrcField() + "\" в источнике. Загрузка не производится.");
                //Src->NextTable();

                break;
            }

            Dst->Post();  // Нужно для процедуры
            Src->NextRecord();
        }

        if (Src->GetStatus() != SS_FIELD_NOT_FOUND) {
            Logger->WriteLog("Загружен " + Src->GetTable(), log_n);
            Dst->Commit();
        }
        Src->NextTable();
    }

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

    TCommandLine commandline;
    clConfig = commandline.GetValue("-config","-c");
    clDstUsername = commandline.GetValue("-dstuser","-du");
    clDstPassword = commandline.GetValue("-dstpassword","-dp");
    clSrcUsername = commandline.GetValue("-srcuser","-su");
    clSrcPassword = commandline.GetValue("-srcpassword","-sp");
    //clLogfile = commandline.GetValue("-logfile","-l");
    //clSilent = commandline.GetValue("-silent","-s");

    // Преобразовываем относительный путь к файлу в абсолютный путь
    clConfig = ExpandFileName(clConfig);

    int LogLine = Logger->WriteLog("Загрузка файла конфигурации > " + clConfig);

    if (!FileExists(clConfig)) {
        Logger->WriteLog("Ошибка. Файл конфигурации не найден " + clConfig, LogLine);
        return false;
    }

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

                        //ImportParamsDbase.Table.push_back(file);

                        StorDbase->AddTable(ExpandFileName(msxml.GetAttributeValue(subnode, "file")));
                    }
                    subnode = msxml.GetNextNode(subnode);
                }
            } else if (sImportType == "orasql") {
                TStorageOraSql* StorOraSql = new TStorageOraSql();
                SrcStor = (TStorage*) StorOraSql;

                while (!subnode.IsEmpty()) {
                    if (msxml.GetNodeName(subnode) == "orasql") {
                        TOraSqlTable Table;
                        Table.Server = msxml.GetAttributeValue(subnode, "server");
                        Table.Username = clSrcUsername;
                        Table.Password = clSrcPassword;
                        Table.Sql = ExpandFileName(msxml.GetAttributeValue(subnode, "sql"));
                        StorOraSql->AddTable(Table);
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

                StorDbase->AddTable(ExpandFileName(msxml.GetAttributeValue(subnode, "file")));

                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TDBaseField field;
                        field.type = LowerCase(msxml.GetAttributeValue(node_fields, "type"));
                        field.name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                        field.length = msxml.GetAttributeValue(node_fields, "length", 0);
                        field.decimals = msxml.GetAttributeValue(node_fields, "decimals", 0);
                        field.active = msxml.GetAttributeValue(node_fields, "active", true);
                        field.enable = msxml.GetAttributeValue(node_fields, "enable", true);
                        field.name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", field.name));
                        if (field.name_src == "" )
                            field.name_src = field.name;
                        StorDbase->Fields.push_back(field);  // AddField
                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }
            } else if (sExportType == "oraproc") {   //
                TStorageOraProc* StorOraProc = new TStorageOraProc();
                DstStor = (TStorage*) StorOraProc;

                TOraProcTable Table;
                Table.Server = msxml.GetAttributeValue(subnode, "server");
                Table.Username = clDstUsername;
                Table.Password = clDstPassword;
                Table.Procedure = msxml.GetAttributeValue(subnode, "procedure");
                Table.Table = msxml.GetAttributeValue(subnode, "table");
                Table.Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                StorOraProc->AddTable(Table);


                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TOraProcField field;
                        field.name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                        field.active = msxml.GetAttributeValue(node_fields, "active", true);
                        field.enable = msxml.GetAttributeValue(node_fields, "enable", true);
                        field.name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name_src", field.name));
                        if (field.name_src == "" )
                            field.name_src = field.name;

                        StorOraProc->Fields.push_back(field);   // AddField
                        //ExportParamsOraProc.Fields.push_back(field);

                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }

            } else if (sExportType == "oratable") {   //

                TStorageOraTable* StorOraTable = new TStorageOraTable();
                DstStor = (TStorage*) StorOraTable;

                TOraTableTable Table;
                Table.Server = msxml.GetAttributeValue(subnode, "server");
                Table.Username = clDstUsername;
                Table.Password = clDstPassword;
                //Table.Procedure = ExpandFileName(msxml.GetAttributeValue(subnode, "sql"));
                Table.Table = msxml.GetAttributeValue(subnode, "table");
                StorOraTable->AddTable(Table);

                //StorOraProc->AddTable(msxml.GetAttributeValue(subnode, "procedure"));
                //StorOraProc->Procedure = msxml.GetAttributeValue(subnode, "procedure");
                //StorOraProc->Server = msxml.GetAttributeValue(subnode, "server");
                //StorOraProc->Truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                //StorOraProc->Username = clDstUsername;
                //StorOraProc->Password = clDstPassword;

                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TOraProcField field;
                        field.name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                        field.active = msxml.GetAttributeValue(node_fields, "active", true);
                        field.enable = msxml.GetAttributeValue(node_fields, "enable", true);
                        field.name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name", field.name));

                        StorOraTable->Fields.push_back(field);      // Возможно заменить на AddField
                    }
                    node_fields = msxml.GetNextNode(node_fields);
                }



                /*ExportParamsOraTable.server = msxml.GetAttributeValue(subnode, "server");
                ExportParamsOraTable.truncate = msxml.GetAttributeValue(subnode, "truncate", false);
                //ExportParamsOraTable.drop = msxml.GetAttributeValue(subnode, "drop", false);
                ExportParamsOraTable.table = msxml.GetAttributeValue(subnode, "table");
                ExportParamsOraTable.username = clDstUsername;
                ExportParamsOraTable.password = clDstPassword;

                // Параметры для создания таблицы
                ExportParamsOraTable.tablespace = msxml.GetAttributeValue(subnode, "tablespace");  // USERS
	            ExportParamsOraTable.initrans = msxml.GetAttributeValue(subnode, "pctfree");     // 10
	            ExportParamsOraTable.initrans = msxml.GetAttributeValue(subnode, "initrans");    // 1
	            ExportParamsOraTable.maxtrans = msxml.GetAttributeValue(subnode, "maxtrans");    // 255
	            ExportParamsOraTable.initial = msxml.GetAttributeValue(subnode, "initial");     //64K
	            ExportParamsOraTable.minextents = msxml.GetAttributeValue(subnode, "minextents");  // 1
	            ExportParamsOraTable.maxextents = msxml.GetAttributeValue(subnode, "maxextents");  // unlimited


                Variant node_fields = msxml.GetFirstNode(subnode);
                while (!node_fields.IsEmpty()) {
                    if (msxml.GetNodeName(node_fields) == "field") {
                        TOraProcField field;
                        field.name = LowerCase(msxml.GetAttributeValue(node_fields, "name"));
                        field.active = msxml.GetAttributeValue(node_fields, "active", true);
                        field.enable = msxml.GetAttributeValue(node_fields, "enable", true);
                        field.name_src = LowerCase(msxml.GetAttributeValue(node_fields, "name", field.name));
                        ExportParamsOraTable.Fields.push_back(field);
                    }
                    node_fields = msxml.GetNextNode(node_fields);
                } */
            }
        }
        node = msxml.GetNextNode(node);
    }

    Logger->WriteLog("Файл конфигурации загружен " + clConfig, LogLine);
    return true;
}




