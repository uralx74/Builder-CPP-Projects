//---------------------------------------------------------------------------

#ifndef _DEBUG 
#define DEBUG_MESSAGE
#endif  
#ifdef _DEBUG  
#define DEBUG_MESSAGE(msg) OutputDebugString(msg) 
#endif


#include <vcl.h>
#include <conio.h>
#include "CommandLine.h"
#include "Messages.h"
#include "Logger.h"
#include "TransferModule.h"

#pragma hdrstop
USERES("Joiner.res");
USEFORM("Unit1.cpp", Form1);
USEUNIT("..\util\MSExcelWorks.cpp");
USEUNIT("..\util\MSXMLWorks.cpp");
USEUNIT("..\util\CommandLine.cpp");
USEUNIT("TransferModule.cpp");
USEUNIT("Storage.cpp");
USEUNIT("Logger.cpp");
USEUNIT("..\util\VigenereCipher.cpp");
USEUNIT("..\util\TransposCipher.cpp");
USEUNIT("Encoder.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    TLogger* Logger = &TLogger::getInstance();
    Logger->WriteLog("Joiner " + AppFullVersion + " ��������� ��������.");

    //TCommandLine commandline;
    TCommandLine* cl = &TCommandLine::getInstance();

    // ������� ������� � ���������, ���� ���������
    if (cl->GetFlag("-help","-h")) {
        MessageBoxInf(
            "�������������:"
            "\nJoiner [-c=����_�_�����] [-du=my_user1 -dp=my_password1] [-su=my_user2 -sp=my_password2] "
            "[-h]"

            "\n"
            "\n���������:"
            "\n-h | -help\t������� � ��������� (��� ����)"
            "\n-i | -info\t���������� �� ������ ���������"
            "\n"
            "\n-c | -config = <����_�_�����>\t������ ������ ���� � ����� ��������� �������-��������"
            "\n"
            "\n-su | -srcuser\t��� ������������ � �� ���������"
            "\n-sp | -srcpassword\t������ ������� � �� ���������"
            "\n-du | -dstuser\t��� ������������ � �� ����������"
            "\n-dp | -dstpassword\t������ ������� � �� ����������"
            "\n-gp | -getpassword\t���������� ������������� �������� ����� ������������ � ������"
            "\n"
            "\n-ac | -accesscode\t���������� ������������� �������� ����� ������������ � ������"
            "\n-acp | -accesscodepersonal\t���������� ������������� �������� ����� ������������ � ������, � ��������� � ������������"
            "\n-acw | -accesscodeworkstation\t���������� ������������� �������� ����� ������������ � ������, � ��������� � ������� �������"
            "\n"
            "\n-s | -silent\t����� �����"
            "\n-a | -auto\t�������������� ����� ������� (�� ��������� � ������ -s)"
            "\n-l | -log = <����_�_���_�����>\t������ ���� � ���-�����"
            "\n-lr | -logrewrite \t�������������� ���-����"
        );
        return 0;
    }

    // ������� ���������� �� ������, ���� ���������
    if (cl->GetFlag("-info","-i")) {
        MessageBoxInf(
            "��������� ��� ������� ������\nJoiner v." + AppVersion + " (" + AppBuild + ")"
            "\n"
            "\nCopyright � 2014-2016"
            "\n"
            "\n����������� ������ ��� \"���������������\""
            "\n"
            "\n�����: �������-����������� ������ ��"
            "\n������������ ������� ��� \"���������������\""
            "\n�.�.����������"
            "\n"
            "\n e-mail: x74@list.ru"
        );
        return 0;
    }

    // ������������� ���-����, ���� ����� � ����������
    AnsiString sLogFileName = cl->GetValue("-log","-l");
    if (sLogFileName != "") {
        try {
            sLogFileName = ExpandFileName(sLogFileName);
            Logger->AddConsole(sLogFileName, cl->GetFlag("-logrewrite","-lr")); // ���� GetFlag, �� ���-���� ����������������
        } catch (...) {
        }
    }

    // ��������� � ����� ������, ���� ����� ����
    if (cl->GetFlag("-silent","-s")) {
        CoInitialize(NULL);     // COM libruary initialize
        TTransferModule TransferModule;
        TransferModule.Start();
        CoUninitialize();
        return 0;
    }

    try
    {
        Application->Initialize();
        Application->CreateForm(__classid(TForm1), &Form1);
        Application->Run();
        Logger->WriteLog("��������� ���������.");
    }
    catch (Exception &exception)
    {
        Logger->WriteLog("��������� ��������� � �������. " + exception.Message);
        Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
