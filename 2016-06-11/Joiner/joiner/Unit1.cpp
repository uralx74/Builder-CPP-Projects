//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "dbf"
#pragma link "dbf"
#pragma link "DBAccess"
#pragma link "MemDS"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
    //AppPath = ExtractFilePath(Application->ExeName);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    Form1->Caption = "Joiner " + AppFullVersion;
    RichEdit1->Text = "";

    //TLogger* TLogger::Instance();
    //Singleton& Logger = Singleton::getInstance();

    Singleton* Logger = &Singleton::getInstance();
    Logger->SetControl(RichEdit1);
    Logger->WriteLog("Программа запущена.");
}

//---------------------------------------------------------------------------
// Вывод строки статуса
void __fastcall TForm1::SetStatusText(AnsiString sStatus)
{
    StatusBar1->Panels->Items[0]->Text = sStatus;
}

void __fastcall TForm1::Button1Click(TObject *Sender)
{
    TransferModule.Start();
}

//---------------------------------------------------------------------------
// Открыть файл конфигурации
void __fastcall TForm1::Button2Click(TObject *Sender)
{
//
}



