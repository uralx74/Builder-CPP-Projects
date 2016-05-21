#include "CommandLine.h"

//---------------------------------------------------------------------------
//
__fastcall TCommandLine::TCommandLine()
{
    Parse();
}

//---------------------------------------------------------------------------
//
__fastcall TCommandLine::~TCommandLine()
{
    startparams.clear();
}

//---------------------------------------------------------------------------
// ������ ���������� � ���������� � std::map
void __fastcall TCommandLine::Parse()
{
    int n = ParamCount();

    AnsiString paramname = "";
    AnsiString paramvalue = "";

    for (int i = 1; i <= n; i++) {
        AnsiString sParamStr = Trim(ParamStr(i));

        if (sParamStr[1] == '-') {

            // ���� ���������� �������� = ��������
            // ���� �����, �� ��� �������� �� ���������
            // ����� - ��������-�������������
            int eqPos = sParamStr.Pos("=");
            if (eqPos > 0) {
                paramname = sParamStr.SubString(1, eqPos-1);
                paramvalue = sParamStr.SubString(eqPos+1, sParamStr.Length()-eqPos);
            } else {
                paramname = sParamStr;
                paramvalue = "true";
            }
            startparams[paramname] = paramvalue;

        } else { // ���� �������� ��� �����
        }
    }
}

//---------------------------------------------------------------------------
// ���������� �������� ��������� �� �������� ��� ��������� �����
String __fastcall TCommandLine::GetValue(AnsiString Name, AnsiString AltName)
{
    if (Name != "" && startparams[Name] != "")
        return startparams[Name];

    if (AltName != "" && startparams[AltName] != "")
        return startparams[AltName];

    return "";
}

//---------------------------------------------------------------------------
// ���������� �������� ��������� �� �������� ��� ��������� �����
bool __fastcall TCommandLine::GetFlag(AnsiString Name, AnsiString AltName)
{
    if (Name != "" && startparams[Name] != "")
        return true;

    if (AltName != "" && startparams[AltName] != "")
        return true;

    return false;
}

