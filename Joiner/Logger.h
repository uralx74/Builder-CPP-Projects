//---------------------------------------------------------------------------
#ifndef LoggerH
#define LoggerH

#include <Classes.hpp>
#include <Controls.hpp>
#include <ComCtrls.hpp>
#include <fstream>

class TLogger;  // опережающее объ€вление


class TLoggerDestroyer
{
private:
    TLogger* p_instance;
public:    
    ~TLoggerDestroyer();
    void initialize( TLogger* p );



};

class TLogger
{

private:
    static TLogger* p_instance;
    static TLoggerDestroyer destroyer;

    TRichEdit* LogEditPtr;  // ”казатель на лог-RichEdit
    std::FILE* LogFilePtr;  // ”казатель на лог-файл
    TStringList* LogHist;      // »стори€ лог-сообщений

protected:
    TLogger();
    ~TLogger();
    friend class TLoggerDestroyer;      // for access to p_instance

public:
    static TLogger& getInstance();

    int __fastcall WriteLog(AnsiString MessageStr, int LineNumber = -1);
    void __fastcall AddConsole(TRichEdit* Edit);
    void __fastcall AddConsole(AnsiString LogFileName, bool Rewrite = false);

};













    /*

// Singleton.h
class Singleton
{
private: 
    Singleton() {}
    Singleton( const Singleton&);  
    Singleton& operator=( Singleton& );
public:
    static Singleton& getInstance() {
        static Singleton  instance;
        return instance;
    }    
};    */


   /*
class TLogger
{
public:
    int __fastcall WriteLog(AnsiString MessageStr, int LineNumber = -1);

    static const TLogger& Instance()
    {
        static TLogger theSingleInstance;
        return theSingleInstance;
    }

    //TRichEdit* LogEdit;
private:
    TLogger(){}
    TLogger(const TLogger& root);
    TLogger& operator=(const TLogger&);
};
       */

//---------------------------------------------------------------------------
#endif

