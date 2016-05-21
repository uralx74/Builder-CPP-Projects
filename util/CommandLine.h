//---------------------------------------------------------------------------
#ifndef COMMANDLINE
#define COMMANDLINE

/*******************************************************************************
	Класс для работы с параметрами командной строки
    Версия от 18.06.2015


*******************************************************************************/

#include "system.hpp"
#include <utilcls.h>
#include <map>
#include "Comobj.hpp"

//#include <fstream.h>
//#include "taskutils.h"

class TCommandLine
{

private:
    void __fastcall Parse();

public:
	__fastcall TCommandLine();
	__fastcall ~TCommandLine();

    String __fastcall GetValue(AnsiString Name, AnsiString AltName);
    bool __fastcall GetFlag(AnsiString Name, AnsiString AltName);

 	std::map <String,String> startparams;
};


//---------------------------------------------------------------------------
#endif
