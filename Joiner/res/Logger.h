//---------------------------------------------------------------------------
#ifndef LoggerH
#define LoggerH

#include <Classes.hpp>
#include <Controls.hpp>
#include <ComCtrls.hpp>

class Singleton;  // опережающее объ€вление


class SingletonDestroyer
{
private:
    Singleton* p_instance;
public:    
    ~SingletonDestroyer();
    void initialize( Singleton* p );
};
  
class Singleton
{
private:
    static Singleton* p_instance;
    static SingletonDestroyer destroyer;

    TRichEdit* LogEdit;
protected:
    Singleton() { }
    Singleton( const Singleton& );
    Singleton& operator=( Singleton& );
    ~Singleton() { }
    friend class SingletonDestroyer;
public:
    static Singleton& getInstance();

    int __fastcall WriteLog(AnsiString MessageStr, int LineNumber = -1);
    void __fastcall SetControl(TRichEdit* Edit);
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

