//---------------------------------------------------------------------------

#include <vcl.h>
#include "CommandLine.h"
#include "Messages.h"
#include "Logger.h"

#pragma hdrstop
USERES("Joiner.res");
USEFORM("Unit1.cpp", Form1);
USEUNIT("..\util\MSExcelWorks.cpp");
USEUNIT("..\util\MSXMLWorks.cpp");
USEUNIT("..\util\CommandLine.cpp");
USEUNIT("TransferModule.cpp");
USEUNIT("Storage.cpp");
USEUNIT("res\Logger.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

    TCommandLine commandline;
    bool clHelp = commandline.GetFlag("-help","-h");
    if (clHelp) {
        MessageBoxInf(
            "�������������:"
            "\njoin [-c=����_�_�����] [-du=my_user1 -dp=my_password1] [-su=my_user2 -sp=my_password2] "
            "[-h]"

            "\n"
            "\n���������:"
            "\n-help | -h\t������� � ��������� (��� ����)"
            "\n-config | -c=<����_�_�����>\t������ ������ ���� � ����� ��������� �������-��������"
            "\n-srcuser | -su\t��� ������������ � �� ���������"
            "\n-srcpassword | -sp\t������ ������� � �� ���������"
            "\n-dstuser| -du\t��� ������������ � �� ����������"
            "\n-dstpassword | -dp\t������ ������� � �� ����������"
            "\n-getpassword | -gp\t���������� ������������� �������� ����� ������������ � ������"
            "\n-getmachinepassword | -gmp\t���������� ������������� �������� ����� ������������ � ������, � ��������� � ������� �������"
        );
        return 0;
    }

    try
    {
         Application->Initialize();
         Application->CreateForm(__classid(TForm1), &Form1);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
