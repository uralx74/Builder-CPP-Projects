#include "Logger.h"

Singleton* Singleton::p_instance = 0;
SingletonDestroyer Singleton::destroyer;

//---------------------------------------------------------------------------
//
SingletonDestroyer::~SingletonDestroyer()
{
    delete p_instance;
}

//---------------------------------------------------------------------------
//
void SingletonDestroyer::initialize(Singleton* p)
{
    p_instance = p;
}

//---------------------------------------------------------------------------
//
Singleton& Singleton::getInstance()
{
    if(!p_instance) {
        p_instance = new Singleton();
        destroyer.initialize(p_instance);     
    }
    return *p_instance;
}

//---------------------------------------------------------------------------
// Вывод лог-строки
int __fastcall Singleton::WriteLog(AnsiString MessageStr, int LineNumber)
{
    AnsiString sDate = FormatDateTime("hh:mm:ss", Now());

    if (LogEdit) {
        MessageStr = "[" + sDate + "] " + MessageStr;
        if (LineNumber == -1) {
            LineNumber = LogEdit->Lines->Add(MessageStr);
        } else {
            LogEdit->Lines->Strings[LineNumber] = MessageStr;
        }
        Application->ProcessMessages();
    }
    return LineNumber;
}

//---------------------------------------------------------------------------
//
void __fastcall Singleton::SetControl(TRichEdit* Edit)
{
    LogEdit = Edit;
}











//Singleton * Singleton::p_instance = 0;

/*

//---------------------------------------------------------------------------
// Вывод лог-строки
int __fastcall TLogger::WriteLog(AnsiString MessageStr, int LineNumber)
{
    AnsiString sDate = FormatDateTime("hh:mm:ss", Now());

    if (LogEdit) {
        MessageStr = "[" + sDate + "] " + MessageStr;
        if (LineNumber == -1) {
            LineNumber = LogEdit->Lines->Add(MessageStr);
        } else {
            LogEdit->Lines->Strings[LineNumber] = MessageStr;
        }
        Application->ProcessMessages();
    }
    return LineNumber;
}

//---------------------------------------------------------------------------
// Вывод лог-строки
/*int __fastcall TTransferModule::WriteLog(AnsiString MessageStr, TColor Color, int LineNumber)
{

    LogEdit->SelAttributes->Color = Color;
    //SQLTextRichEdit->SelAttributes->Style = SQLTextRichEdit->SelAttributes->Style >> fsBold;


    AnsiString sDate = FormatDateTime("hh:mm:ss", Now());

    if (LogEdit) {
        MessageStr = "[" + sDate + "] " + MessageStr;
        if (LineNumber == -1) {
            LineNumber = LogEdit->Lines->Add(MessageStr);
        } else {
            LogEdit->Lines->Strings[LineNumber] = MessageStr;
            //LogEdit->Lines->;
            //            LogEdit->Lines
            //int SelStart = SendMessage(LogEdit, EM_LINEINDEX, LineNumber - 1, 0);


            //LogEdit->SelStart = 0;

            //LogEdit->SelText = LogEdit->Lines->Strings[LineNumber];

            //LogEdit->SelLength = LogEdit->Lines->Strings[LineNumber].Length();
            //LogEdit->SelAttributes->Color = Color;


            //LogEdit->SelLength = LogEdit->Lines->Strings[LineNumber].Length();
            //int h = SendMessage(LogEdit, EM_LINEINDEX, LineNumber - 1, 0);
            //SelLength := Length(Lines[ARow - 1]);
            //SelAttributes.Color := AColor;
            //SelLength := 0;

        }
        Application->ProcessMessages();
    }
    return LineNumber;
} */

