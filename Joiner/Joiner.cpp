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
            "Использование:"
            "\njoin [-c=путь_к_файлу] [-du=my_user1 -dp=my_password1] [-su=my_user2 -sp=my_password2] "
            "[-h]"

            "\n"
            "\nПараметры:"
            "\n-help | -h\tСправка о программе (это окно)"
            "\n-config | -c=<путь_к_файлу>\tЗадает полный путь к файлу настройки импорта-экспорта"
            "\n-srcuser | -su\tИмя пользователя в БД источника"
            "\n-srcpassword | -sp\tПароль доступа к БД источника"
            "\n-dstuser| -du\tИмя пользователя в БД назначения"
            "\n-dstpassword | -dp\tПароль доступа к БД назначения"
            "\n-getpassword | -gp\tОтображает зашифрованные значения имени пользователя и пароля"
            "\n-getmachinepassword | -gmp\tОтображает зашифрованные значения имени пользователя и пароля, с привязкой к рабочей станции"
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
