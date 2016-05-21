/*
  ��������� ��� �������� ������ �� ��� ������ �:
  1. MS Excel
  2. MS Word
  3. DBF

  �����: �������-����������� 2 ���. ��� �� ���
  �.�.����������


  ��� ��������� ������:
  1. ����������� �������� ��� ����������� �������� � ���������� �������, �� � �������
  �����������.
  2. ����������� �������� ��� ����������� ������� ���������� ������� ��������.
  3. ����������� �������� ��� ����������� ���������� ������ �������������.
  4. ����������� �������� ��� ����������� ������� ��������� �� ���������� � ������������
  �������� �������� ������ �� �����.

*/
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "fmain.h"

USERES("SQL2Excel.res");
USEFORM("FMain.cpp", Form1);
USEUNIT("ThreadSelect.cpp");
USEFORM("FormWait.cpp", Form_Wait);
USEFORM("FShowQuery.cpp", FormShowQuery);
USEFORM("..\util\FormLogin\FormLogin.cpp", LoginForm);
USEUNIT("..\util\MSExcelWorks.cpp");
USEUNIT("..\util\MSXMLWorks.cpp");
USEUNIT("..\util\MSWordWorks.cpp");
USEUNIT("..\util\CommandLine.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
/*    map  <String,String> startparams;
    startparams["-username"] = "";
    startparams["-password"] = "";
    startparams["-query"] = "";
    startparams["-mail"] = "";
    startparams["-file"] = "false";
    startparams["-failure"] = "";
    startparams["-subject"] = "";
    startparams["-param"] = "";

    map <String,int> argcount;
    argcount["-username"] = 1;
    argcount["-password"] = 1;
    argcount["-query"] = 1;
    argcount["-mail"] = 1;
    argcount["-file"] = 1;
    argcount["-failure"] = 0;
    argcount["-subject"] = 1;
    argcount["-param"] = 2;

    map <String,String> startparamsshort;
    startparamsshort["-u"] = "-username";
    startparamsshort["-p"] = "-password";
    startparamsshort["-q"] = "-query";
    startparamsshort["-m"] = "-mail";
    startparamsshort["-f"] = "-file";
    startparamsshort["-s"] = "-subject";

    bool bKeyAuto = false;
    int n = ParamCount();


    AnsiString paramname = "";

    for (int i = 1; i <= n; i++) {
        AnsiString sParamStr = Trim(ParamStr(i));
        if (paramname == "") {
            //if (startparams.find(sParamStr)!=startparams.end())
            if (startparams.count(sParamStr) > 0 )
                paramname = sParamStr;
            else if (startparamsshort.count(sParamStr) > 0)
                paramname = startparamsshort[sParamStr];

            if (paramname == "")
                continue;

            if (argcount[paramname] == 0) {
                paramname = "";
                startparams[paramname] = "true";
            }
        } else {
            argcount[paramname];
            startparams[paramname]=sParamStr;
            paramname = "";
        }
    }

/*    for(std::map<String,String>::iterator it = startparams.begin(); it != startparams.end(); ++it) {
        String s = it->first; //it->second << endl;///????? ?? ?????
    }
*/

  /*  AnsiString sParam = startparams["-username"];
    AnsiString sParam2 = startparams["-password"];                         */


	try
	{
        Application->Initialize();
		Application->CreateForm(__classid(TForm1), &Form1);
        Application->CreateForm(__classid(TFormShowQuery), &FormShowQuery);
        Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------



