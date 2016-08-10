// -c="c:\7-test sqltext\nasel_ccb_ft\change_cal_period.xml" -l="log_nasel_ccb_ft.log" -a -aeq
// -c="c:\6\kavabunga\kavabunga.xml" -l="%dlog.log" -a -accce
//

#include "pch.h"
#pragma hdrstop
#include "TransferModule.h"

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TTransferModule::TTransferModule()
{
    //Logger = Singleton::getInstance();
    //_IsCancel = false;
}

//---------------------------------------------------------------------------
// ������ ������� ������
void __fastcall TTransferModule::Cancel()
{
    //_IsCancel = true;
}

//---------------------------------------------------------------------------
// ������
void __fastcall TTransferModule::Start()
{
    Logger = &TLogger::getInstance();

    //pParentThread = Thread;
    TXmlLoader* pXmlLoader = new TXmlLoader();

    if (pXmlLoader->LoadParameters()) {
        Transfer(pXmlLoader->SrcStor, pXmlLoader->DstStor);
    }

    delete pXmlLoader;
}

//---------------------------------------------------------------------------
//
void __fastcall TTransferModule::Transfer(TStorage* Src, TStorage* Dst)
{
    if (!Src || !Dst) {
        Logger->WriteLog("��������� ����������� ������� ���� ��������, ��� ��� " +
            ((String)(!Src? "��������" : "��������")) + " �� �����.");
        return;
    }

    Logger->WriteLog("��������� ����������� ������ ��������.");
    TDateTime StartTime = Now();


    int log_n;
    log_n = Logger->WriteLog("�������� ��������� >");
    try {
        Dst->Open(false);   // ��������� ��������
        Logger->WriteLog("�������� ������ �������.", log_n);
    } catch (Exception &e) {
        Logger->WriteLog("�� ������� ������� �������� \"" + Dst->GetTable() + "\". " + e.Message, log_n);
        Dst->Close();
        Logger->WriteLog("��������� ����������� ������ ���������.");
        return;
    }

    log_n = Logger->WriteLog("�������� ��������� >");
    try {
        Src->Open(true);    // ��������� ��������, ���� ������, �� ����� � ����� ���������� ��������� ��������
        Logger->WriteLog("�������� ������ �������.", log_n);
    } catch (Exception &e) {
        Logger->WriteLog("�� ������� ������� �������� \"" + Src->GetTable()+ "\" (" + Src->getTableStage() + "). " + e.Message, log_n);
    }

    int DstRecordCountFirst = Dst->GetRecordCount();



    //!!!!!!!!!!!!!!!!!!!!!!!!!!
    /*Sleep(1000);
    if (_IsCancel) {
        log_n = Logger->WriteLog("����������");
        throw Exception("Start()");
    }*/

    while(!Src->Eot()) { // ���� �� ��������
        if (!Src->IsActiveTable()) {
            try {
                Src->NextTable();
                if (Src->Eot())
                    break;
            } catch (Exception &e) {
                Logger->WriteLog("�� ������� ������� �������� \"" + Src->GetTable() + "\". " + e.Message);
                continue;
            }
        }

        //TDateTime timer = Now();
        // ���� �������� ������ �������
        log_n = Logger->WriteLog("����������� > \"" + Src->GetTable() + "\" (" + Src->getTableStage() + "; " + Src->getRecordStage()+")");

        try {
            // ����� ������� ���������� ���������.
            // ����� ����� ;
            // � ��������� ���������� �������� �� ����, ������� ���� �������������� ���� � ���������
            // ������� ������ �����, ������� ���������� (required), �� ������� ��� � ���������
            // � ����������� �� ������ ��������� ������, ���� ����� ���� �����������, �� ������������� ���� ��� ���������� 

            Dst->LinkSource(Src);

            int DstRecordCount = Dst->GetRecordCount();
            while(!Src->Eor()) {    // ���� �� �������
                //String sss = Src->getRecordStage();
                //if (Src->GetRecordIndex() % 100 == 0) {
                    //Logger->WriteLog("����������� > \"" + Src->GetTable() + "\" (" + Src->getTableStage() + "; " + Src->getRecordStage() + ")",log_n);
                    //Logger->WriteLog("����������� > \"" + Src->GetTable() + "\" (" + Src->getTableStage() + "; " + Src->getRecordStage() + ")");

                //}
                //Logger->WriteLog("����������� > " + Src->GetTable() + " " + Src->GetStage(), log_n);

                Dst->Append();

                while(!Dst->Eof()) {    // ���� �� �������� � ���������
                    if (Dst->IsLinkedField() && Dst->IsActiveField()) {
                        Variant Value;

                        try {
                            TStorageField *field = Dst->GetField();
                            Value = Src->GetFieldValue(field);
                        } catch (Exception &e) {
                            Logger->WriteLog("������ � ��������� \"" + Src->GetTable() + "\" (" + Src->getTableStage() + ")" + ". " + e.Message /*+ Dst->GetSrcField()*/, log_n);
                            throw Exception("");
                        }
                        try {
                            Dst->SetFieldValue(Value);
                        } catch (Exception &e) {
                            Logger->WriteLog("������ � ��������� \"" + Dst->GetTable() + "\" (" + Dst->getTableStage() + ")" + ". " + e.Message /*+ Dst->GetSrcField()*/, log_n);
                            throw Exception("");
                        }
                    }
                    Dst->NextField();
                }

                //if (Dst->IsModified()) {
                //    Dst->Post();
                //} else {
                //    Dst->Rollback();
                //}

                Dst->Post();  // ��������� ������
                Src->NextRecord();
            }

            Dst->Commit();

            // ���������, ��� �� ���� ���� Commit � ���������
            // ����� ���� ������������ RecordIndex ��� RecordCount
            int LoadedRecordsCount = Dst->GetRecordCount() - DstRecordCount;

            Logger->WriteLog("��������� " + IntToStr(LoadedRecordsCount) + " ������� �� \"" + Src->GetTable() + "\"", log_n);
            DstRecordCount = Dst->GetRecordCount();

        } catch (Exception &e) {
            if (e.Message != "")
                Logger->WriteLog("������������� ������. ��������: \"" + Src->GetTable() + "\" (" + Src->getTableStage() + "), �������� \"" + Dst->GetTable()+ "\". " + e.Message /*+ Dst->GetSrcField()*/);
        }

        try {
            Src->NextTable();
        } catch (Exception &e) {
            Logger->WriteLog("������ � ��������� \"" + Src->GetTable() + "\" (" + Src->getTableStage() + "). " + e.Message);
            continue;
        }
    }

    // ��������� ������ - ����������
    TDateTime StopTime = Now() ;
    int TotalSec = SecsPerDay * (StopTime - StartTime);
    int hh = (TotalSec ) / 3600;
    int mm = (TotalSec - hh * 3600) / 60;
    int ss = TotalSec % 60;
    AnsiString sTotalTime = IntToStr(ss) + " ���";
    if (mm > 0)
        sTotalTime = IntToStr(mm) + " ��� " + sTotalTime;
    if (hh > 0)
        sTotalTime = IntToStr(hh) + " ��� " + sTotalTime;

 
    int DstRecordCountTotal = Dst->GetRecordCount() - DstRecordCountFirst;

    // ���������, ��� �� ���� ���� Commit � ���������
    // ����� ���� ������������ RecordIndex ��� RecordCount
    if (Dst->IsModified()) {
        Logger->WriteLog("����� ��������� " + IntToStr(DstRecordCountTotal) + " ������� �� " + sTotalTime+ ".");
        Logger->WriteLog("��������� �������� � \"" + Dst->GetTable() + "\".");
    }

    Src->Close();
    Dst->Close();

    Logger->WriteLog("��������� ����������� ������ ���������.");
}


