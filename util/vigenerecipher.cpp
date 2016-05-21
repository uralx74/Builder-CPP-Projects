//---------------------------------------------------------------------------
#include "VigenereCipher.h"

TVigenereCipher::TVigenereCipher()
{
    // ������������� ��������
    _Abc = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ�������������������������������������Ũ��������������������������1234567890~@#$%^&*_+|!\"�;:?()/'.,{}[]<>\\ \n\r\t";
}

TVigenereCipher::~TVigenereCipher()
{
}

//---------------------------------------------------------------------------
// ����������
AnsiString __fastcall TVigenereCipher::Encrypt(AnsiString SrcStr, AnsiString KeyStr)
{
    char* Abc = _Abc.c_str();
    int AbcLength = strlen(Abc);

    char* Src = SrcStr.c_str();
    char* Key = KeyStr.c_str();
    int n = strlen(Src);

    char* Dst = new char[n+1];
    Dst[n] = '\0';

    int n_key = strlen(Key);
    char* KeyExt = new char[n+1];
    KeyExt[n] = '\0';

    for (int i = 0; i < n; i++) {       // ��������� ����, ����� ����� ������ ���� ����� ����� ���������� ������
        KeyExt[i] = Key[i%n_key];
    }

    for(int i = 0; i < n; i++) {        // ����������
        int n1 = 0;
        int n2 = 0;
        for(int j = 0; j < AbcLength; j++) {
            if (Abc[j] == Src[i]) {
                n1 = j;
            }
            if (Abc[j] == KeyExt[i]) {
                n2 = j;
            }
        }
        int n3 = (n1 + n2) % AbcLength;
        Dst[i] = Abc[n3];
    }

    AnsiString result = AnsiString(Dst);
    delete[] Dst;
    delete[] KeyExt;

    return result;
}

//---------------------------------------------------------------------------
// �����������
AnsiString __fastcall TVigenereCipher::Decrypt(AnsiString SrcStr, AnsiString KeyStr)
{

    char* Abc = _Abc.c_str();
    int AbcLength = strlen(Abc);

    char* Src = SrcStr.c_str();
    char* Key = KeyStr.c_str();
    int n = strlen(Src);

    char* Dst = new char[n+1];
    Dst[n] = '\0';

    int n_key = strlen(Key);
    char* KeyExt = new char[n+1];
    KeyExt[n] = '\0';

    for (int i = 0; i < n; i++) {       // ��������� ����, ����� ����� ������ ���� ����� ����� ���������� ������
        KeyExt[i] = Key[i%n_key];
    }

    for(int i = 0; i < n; i++) {        // ����������
        int n1 = 0;
        int n2 = 0;
        for(int j = 0; j < AbcLength; j++) {
            if (Abc[j] == Src[i]) {
                n1 = j;
            }
            if (Abc[j] == KeyExt[i]) {
                n2 = j;
            }
        }
        int n3 = (AbcLength - n2 + n1)% AbcLength;
        Dst[i] = Abc[n3];
    }

    AnsiString result = AnsiString(Dst);
    delete[] Dst;
    delete[] KeyExt;

    return result;
}

