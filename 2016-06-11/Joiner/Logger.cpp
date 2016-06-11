#include "Logger.h"

TLogger* TLogger::p_instance = 0;
TLoggerDestroyer TLogger::destroyer;

//---------------------------------------------------------------------------
// Singleton
TLoggerDestroyer::~TLoggerDestroyer()
{
    delete p_instance;
}

//---------------------------------------------------------------------------
// Singleton
void TLoggerDestroyer::initialize(TLogger* p)
{
    p_instance = p;
}

//---------------------------------------------------------------------------
// Singleton
TLogger& TLogger::getInstance()
{
    if(!p_instance) {
        p_instance = new TLogger();
        destroyer.initialize(p_instance);
    }
    return *p_instance;
}

//---------------------------------------------------------------------------
//
TLogger::TLogger()
{
    LogEditPtr = NULL;
    LogFilePtr = NULL;
    LogHist = new TStringList();

}

//---------------------------------------------------------------------------
//
TLogger::~TLogger()
{
    fclose(LogFilePtr);
    LogFilePtr = NULL;

    delete LogHist;
    LogHist = NULL;
}

//---------------------------------------------------------------------------
// Вывод лог-строки
int __fastcall TLogger::WriteLog(AnsiString MessageStr, int LineNumber)
{
    AnsiString sDate = FormatDateTime("hh:mm:ss", Now());
    MessageStr = "[" + sDate + "] " + MessageStr;

    LogHist->Add(MessageStr);   // Добавляем строку в историю

    if (LogEditPtr) {
        if (LineNumber == -1) {
            LineNumber = LogEditPtr->Lines->Add(MessageStr);
        } else {
            LogEditPtr->Lines->Strings[LineNumber] = MessageStr;
        }
        LogEditPtr->Update();
        Application->ProcessMessages();
    }
    if (LogFilePtr) {
        fprintf(LogFilePtr,  "%s\n", MessageStr);
    }
    return LineNumber;
}

//---------------------------------------------------------------------------
// Устанавливает активный контрол для вывода информации
// Если сохранилась история, то выводит ее
void __fastcall TLogger::AddConsole(TRichEdit* Edit)
{
    LogEditPtr = Edit;

    // Выводим историю
    if (LogHist->Count > 0)
        LogEditPtr->Lines->AddStrings(LogHist);
}

//---------------------------------------------------------------------------
// Устанавливает активный файл для вывода информации
// Если сохранилась история, то выводит ее
void __fastcall TLogger::AddConsole(AnsiString LogFileName, bool Rewrite)
{
    if (Rewrite)
        LogFilePtr = std::fopen(LogFileName.c_str(), "wt");
    else
        LogFilePtr = std::fopen(LogFileName.c_str(), "at");

    // Выводим историю
    for (int i=0; i < LogHist->Count; i++) {
        fprintf(LogFilePtr,  "%s\n", LogHist->Strings[i]);
    }
}





//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
