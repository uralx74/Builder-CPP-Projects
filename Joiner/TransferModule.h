//---------------------------------------------------------------------------
// TransferModule.h
// author: vsovchinnikov
// 2016-05-16
//
// Модуль для реализации основных функций программы Joiner:
// 1. Выполняет загрузку конфигурационных данных из файла с настройками в формате .xml
// 2. Выполняет перенос данных из Хранилища-источника (класс TStorage) в Хранилище-приемник.
//---------------------------------------------------------------------------
#ifndef TransferModuleH
#define TransferModuleH

#include "c:\PROGRS\current\util\taskutils.h"
#include "c:\PROGRS\current\util\appver.h"
#include "c:\PROGRS\current\util\MSXMLWorks.h"
#include "c:\PROGRS\current\util\CommandLine.h"
#include <ComCtrls.hpp>
#include "Storage.h"
#include "Logger.h"

//---------------------------------------------------------------------------
class TTransferModule
{
private:	// User declarations
public:		// User declarations
    __fastcall TTransferModule();

    bool __fastcall LoadParameters();
    void __fastcall Start();
    void __fastcall Transfer(TStorage* Src, TStorage* Dst);

    // Источник и приемник
    TStorage* SrcStor;
    TStorage* DstStor;

    //Singleton& Logger;
    Singleton* Logger;
};

//---------------------------------------------------------------------------
#endif

