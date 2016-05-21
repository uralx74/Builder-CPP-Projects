//---------------------------------------------------------------------------
#ifndef VIGENERECIPHERH
#define VIGENERECIPHERH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
// Vigenere Cipher
// vsovchinnikov
// 2016-05-20
//---------------------------------------------------------------------------

class TVigenereCipher {
private:
    AnsiString _Abc;

public:
    TVigenereCipher();
    ~TVigenereCipher();
	AnsiString __fastcall Encrypt(AnsiString SrcStr, AnsiString KeyStr);
	AnsiString __fastcall Decrypt(AnsiString SrcStr, AnsiString KeyStr);
};

//---------------------------------------------------------------------------
#endif
