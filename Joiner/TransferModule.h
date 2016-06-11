//---------------------------------------------------------------------------
// TransferModule.h
// author: vsovchinnikov
// 2016-05-16
//
// ������ ��� ���������� �������� ������� ��������� Joiner:
// 1. ��������� �������� ���������������� ������ �� ����� � ����������� � ������� .xml
// 2. ��������� ������� ������ �� ���������-��������� (����� TStorage) � ���������-��������.
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

    // �������� � ��������
    TStorage* SrcStor;
    TStorage* DstStor;

    //Singleton& Logger;
    Singleton* Logger;
};

//---------------------------------------------------------------------------
#endif

