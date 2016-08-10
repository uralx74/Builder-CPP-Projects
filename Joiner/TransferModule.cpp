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
// Отмена запуска потока
void __fastcall TTransferModule::Cancel()
{
    //_IsCancel = true;
}

//---------------------------------------------------------------------------
// Запуск
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
        Logger->WriteLog("Процедура копирования неможет быть запущена, так как " +
            ((String)(!Src? "источник" : "приемник")) + " не задан.");
        return;
    }

    Logger->WriteLog("Процедура копирования данных запущена.");
    TDateTime StartTime = Now();


    int log_n;
    log_n = Logger->WriteLog("Открытие приемника >");
    try {
        Dst->Open(false);   // Открываем приемник
        Logger->WriteLog("Приемник открыт успешно.", log_n);
    } catch (Exception &e) {
        Logger->WriteLog("Не удалось открыть приемник \"" + Dst->GetTable() + "\". " + e.Message, log_n);
        Dst->Close();
        Logger->WriteLog("Процедура копирования данных завершена.");
        return;
    }

    log_n = Logger->WriteLog("Открытие источника >");
    try {
        Src->Open(true);    // Открываем источник, если ошибка, то далее в цикле выбирается следующий источник
        Logger->WriteLog("Источник открыт успешно.", log_n);
    } catch (Exception &e) {
        Logger->WriteLog("Не удалось открыть источник \"" + Src->GetTable()+ "\" (" + Src->getTableStage() + "). " + e.Message, log_n);
    }

    int DstRecordCountFirst = Dst->GetRecordCount();



    //!!!!!!!!!!!!!!!!!!!!!!!!!!
    /*Sleep(1000);
    if (_IsCancel) {
        log_n = Logger->WriteLog("ПРЕРЫВАНИЕ");
        throw Exception("Start()");
    }*/

    while(!Src->Eot()) { // Цикл по таблицам
        if (!Src->IsActiveTable()) {
            try {
                Src->NextTable();
                if (Src->Eot())
                    break;
            } catch (Exception &e) {
                Logger->WriteLog("Не удалось открыть источник \"" + Src->GetTable() + "\". " + e.Message);
                continue;
            }
        }

        //TDateTime timer = Now();
        // Если источник открыт успешно
        log_n = Logger->WriteLog("Загружается > \"" + Src->GetTable() + "\" (" + Src->getTableStage() + "; " + Src->getRecordStage()+")");

        try {
            // Здесь сделать подготовку приемника.
            // Чтото вроде ;
            // В приемнике необходимо отметить те поля, которым есть сопоставленные поля в источнике
            // Вывести список полей, которые необходимы (required), но которых нет в источнике
            // В зависимости от ключей командной строки, если такие поля обнаружатся, то останавливать цикл или продолжать 

            Dst->LinkSource(Src);

            int DstRecordCount = Dst->GetRecordCount();
            while(!Src->Eor()) {    // Цикл по строкам
                //String sss = Src->getRecordStage();
                //if (Src->GetRecordIndex() % 100 == 0) {
                    //Logger->WriteLog("Загружается > \"" + Src->GetTable() + "\" (" + Src->getTableStage() + "; " + Src->getRecordStage() + ")",log_n);
                    //Logger->WriteLog("Загружается > \"" + Src->GetTable() + "\" (" + Src->getTableStage() + "; " + Src->getRecordStage() + ")");

                //}
                //Logger->WriteLog("Загружается > " + Src->GetTable() + " " + Src->GetStage(), log_n);

                Dst->Append();

                while(!Dst->Eof()) {    // Цикл по столбцам в приемнике
                    if (Dst->IsLinkedField() && Dst->IsActiveField()) {
                        Variant Value;

                        try {
                            TStorageField *field = Dst->GetField();
                            Value = Src->GetFieldValue(field);
                        } catch (Exception &e) {
                            Logger->WriteLog("Ошибка в источнике \"" + Src->GetTable() + "\" (" + Src->getTableStage() + ")" + ". " + e.Message /*+ Dst->GetSrcField()*/, log_n);
                            throw Exception("");
                        }
                        try {
                            Dst->SetFieldValue(Value);
                        } catch (Exception &e) {
                            Logger->WriteLog("Ошибка в приемнике \"" + Dst->GetTable() + "\" (" + Dst->getTableStage() + ")" + ". " + e.Message /*+ Dst->GetSrcField()*/, log_n);
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
                Logger->WriteLog("Непредвиденая ошибка. Источник: \"" + Src->GetTable() + "\" (" + Src->getTableStage() + "), приемник \"" + Dst->GetTable()+ "\". " + e.Message /*+ Dst->GetSrcField()*/);
        }

        try {
            Src->NextTable();
        } catch (Exception &e) {
            Logger->WriteLog("Ошибка в источнике \"" + Src->GetTable() + "\" (" + Src->getTableStage() + "). " + e.Message);
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
    if (Dst->IsModified()) {
        Logger->WriteLog("Всего загружено " + IntToStr(DstRecordCountTotal) + " записей за " + sTotalTime+ ".");
        Logger->WriteLog("Результат сохранен в \"" + Dst->GetTable() + "\".");
    }

    Src->Close();
    Dst->Close();

    Logger->WriteLog("Процедура копирования данных завершена.");
}


