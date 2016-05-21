#include "TransferModule.h"

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TTransferModule::TTransferModule()
{
    //Logger = Singleton::getInstance();

}

//---------------------------------------------------------------------------
// ������
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
    Logger->WriteLog("��������� ����������� ������ ��������.");
    TDateTime StartTime = Now();

    try {
        Dst->Open();   // ��������� ��������
    } catch (Exception &e) {
        Logger->WriteLog("������ � ��������� \"" + Dst->GetTable() + "\". " + e.Message);
        Dst->Close();
        Logger->WriteLog("��������� ����������� ������ ���������.");
        return;
    }

    try {
        Src->Open();    // ��������� ��������, ���� ������, �� ����� � ����� ���������� ��������� ��������
    } catch (Exception &e) {
        Logger->WriteLog("������ � ��������� \"" + Src->GetTable()+ "\" (" + Src->GetTableStage() + "). " + e.Message);
    }


    int DstRecordCountFirst = Dst->GetRecordCount();

    while(!Src->Eot()) { // ���� �� ��������

        int log_n;

        if (!Src->IsActiveTable()) {
            try {
                Src->NextTable();
            } catch (Exception &e) {
                Logger->WriteLog("������ � ��������� \"" + Src->GetTable() + "\". " + e.Message);
                continue;
            }
        }

        // ���� �������� ������ �������
        log_n = Logger->WriteLog("����������� > \"" + Src->GetTable() + "\" (" + Src->GetTableStage() + ")");

        try {
            int DstRecordCount = Dst->GetRecordCount();
            while(!Src->Eor()) {    // ���� �� �������
                //Logger->WriteLog("����������� > " + Src->GetTable() + " " + Src->GetStage(), log_n);
                Dst->Append();

                while(!Dst->Eof()) {    // ���� �� �������� � ���������
                    //AnsiString ssssss = Dst->GetSrcField();
                    if (Dst->IsActiveField()) {
                        try {
                            Variant val = Src->Get( Dst->GetSrcField() );
                            Dst->Set(val);
                        } catch (Exception &e) {
                            Logger->WriteLog("������ � ��������� \"" + Src->GetTable() + "\" (" + Src->GetTableStage() + ")" + ". " + e.Message /*+ Dst->GetSrcField()*/, log_n);
                            throw Exception("");
                        }
                        //Dst->Set( Src->Get( Dst->GetSrcField() ));
                    }
                    Dst->NextField();
                }

                Dst->Post();  // ��������� ������
                Src->NextRecord();
            }

            Dst->Commit();

            // ���������, ��� �� ���� ���� Commit � ���������
            // ����� ���� ������������ RecordIndex ��� RecordCount
            int LoadedRecordsCount = Dst->GetRecordCount() - DstRecordCount;
            Logger->WriteLog("��������� " + IntToStr(LoadedRecordsCount) + " ������� �� \"" + Src->GetTable() + "\"", log_n);
            DstRecordCount = Dst->GetRecordCount();

            /*if (LoadedRecordsCount > 0) {
                //Logger->WriteLog("�������� " + Src->GetTable(), log_n);
                Logger->WriteLog("�������� " + Src->GetTable(), log_n);
                DstRecordCount = Dst->GetRecordCount();
            } else {
                Logger->WriteLog("��������� 0 ������� �� " + Src->GetTable(), log_n);
            }*/

        } catch (Exception &e) {
            if (e.Message != "")
                Logger->WriteLog("������������� ������. ��������: \"" + Src->GetTable() + "\" (" + Src->GetTableStage() + "), �������� \"" + Dst->GetTable()+ "\". " + e.Message /*+ Dst->GetSrcField()*/);
        }

        try {
            Src->NextTable();
        } catch (Exception &e) {
            Logger->WriteLog("������ � ��������� \"" + Src->GetTable() + "\" (" + Src->GetTableStage() + "). " + e.Message);
            continue;
        }
    }

    �������� �����
    TDateTime StopTime = EncodeDate(2016, 05, 23);
    //TDateTime StopTime = Now() ;
    int TotalSec = SecsPerDay * (StopTime - StartTime);
    int dd = TotalSec/SecsPerDay;
    int hh = (TotalSec - dd * 24 * 3600) / 3600;
    int mm = (TotalSec / 60) % 60;
    int ss = TotalSec % 60;
    AnsiString sTotalTime = IntToStr(ss) + " ���";
    if (hh+dd+mm > 0)
        sTotalTime = IntToStr(mm) + " ��� " + sTotalTime;
    if (hh+dd > 0)
        sTotalTime = IntToStr(hh) + " ��� " + sTotalTime;
    if (dd > 0)
        sTotalTime = IntToStr(dd) + " ��� " + sTotalTime;



    int DstRecordCountTotal = Dst->GetRecordCount() - DstRecordCountFirst;
    // ���������, ��� �� ���� ���� Commit � ���������
    // ����� ���� ������������ RecordIndex ��� RecordCount
    if (Dst->IsModified())
        Logger->WriteLog("����� ��������� " + IntToStr(DstRecordCountTotal) + " ������� �� " + sTotalTime+ ". ��������� �������� � \"" + Dst->GetTable() + "\"");

    Src->Close();
    Dst->Close();

    Logger->WriteLog("��������� ����������� ������ ���������.");
}

//---------------------------------------------------------------------------
// ��������� ��������� �� ��������� ������
// � ����� ������������
bool __fastcall TTransferModule::LoadParameters()
{
    // ��������� ��������� ������
    AnsiString clConfig;    // ���� � ����� ������������
    AnsiString clSrcUsername;  // ��� ������������ ���� ������
    AnsiString clSrcPassword;  // ������ � ���� ������
    AnsiString clDstUsername;  // ��� ������������ ���� ������
    AnsiString clDstPassword;  // ������ � ���� ������

    TCommandLine commandline;
    clConfig = commandline.GetValue("-config","-c");
    clDstUsername = commandline.GetValue("-dstuser","-du");
    clDstPassword = commandline.GetValue("-dstpassword","-dp");
    clSrcUsername = commandline.GetValue("-srcuser","-su");
    clSrcPassword = commandline.GetValue("-srcpassword","-sp");
    //clLogfile = commandline.GetValue("-logfile","-l");
    //clSilent = commandline.GetValue("-silent","-s");

    // ��������������� ������������� ���� � ����� � ���������� ����
    clConfig = ExpandFileName(clConfig);

    int LogLine = Logger->WriteLog("�������� ����� ������������ > " + clConfig);

    if (!FileExists(clConfig)) {
        Logger->WriteLog("������. ���� ������������ �� ������ " + clConfig, LogLine);
        return false;
    }

    MSXMLWorks msxml;
    msxml.LoadXMLFile(clConfig);

    if (msxml.GetParseError() != "") {
        Logger->WriteLog("������: " + msxml.GetParseError());
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
                        Table.File = ExpandFileName(msxml.GetAttributeValue(subnode, "file"));  // ��� ����� ����� ���� ������ ������
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

                        // � ���������� ����������. 1. ��������� ������ ������������; 2. ����������-������������
                        Table.Username = msxml.GetAttributeValue(subnode, "username", clSrcUsername);
                        Table.Password = msxml.GetAttributeValue(subnode, "password", clSrcPassword);
                        //Table.Username = clSrcUsername;
                        //Table.Password = clSrcPassword;
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
                        Table.File = ExpandFileName(msxml.GetAttributeValue(subnode, "file"));  // ��� ����� ����� ���� ������ ������
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
                Table.Username = clDstUsername;
                Table.Password = clDstPassword;
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

            } else if (sExportType == "oratable") {   // �������� - ������� � �� Oracle

                TStorageOraSql* StorOraTable = new TStorageOraSql();
                DstStor = (TStorage*) StorOraTable;
                DstStor->SetReadOnly(false);

                TOraSqlTable Table;
                Table.Server = msxml.GetAttributeValue(subnode, "server");
                Table.Username = clDstUsername;
                Table.Password = clDstPassword;
                Table.Sql = ""; // �����!
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

                        StorOraTable->AddField(field);      // �������� �������� ��
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

    Logger->WriteLog("���� ������������ �������� " + clConfig, LogLine);
    return true;
}


