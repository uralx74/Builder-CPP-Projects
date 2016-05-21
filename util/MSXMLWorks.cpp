#include "MSXMLWorks.h"

//---------------------------------------------------------------------------
//
MSXMLWorks::MSXMLWorks()
{
    //Variant xmlObj = CreateOleObject("Microsoft.XMLDOM");
    //Variant xmlDoc = CreateOleObject("MSXML.DOMDocument");
    xmlDoc = CreateOleObject("Msxml2.DOMDocument.3.0");
    xmlDoc.OlePropertySet("Async", false);
}

//---------------------------------------------------------------------------
//
void __fastcall MSXMLWorks::LoadXMLFile(AnsiString XMLFileName)
{
    xmlDoc.OlePropertyGet("Load", XMLFileName.c_str());
}

//---------------------------------------------------------------------------
//
void __fastcall MSXMLWorks::LoadXMLText(AnsiString XMLText)
{
    //StringToOleStr(XMLText);
    xmlDoc.OlePropertyGet("LoadXML", XMLText.c_str());
}


//---------------------------------------------------------------------------
// ���������, ���������� �� �������
Variant MSXMLWorks::GetAttribute(Variant Node, AnsiString AttributeName)
{
    return Node.OlePropertyGet("attributes").OleFunction("getNamedItem", AttributeName);
    //return attribute.IsEmpty();
}

//---------------------------------------------------------------------------
// ���������� ���������� ��������� ����
AnsiString __fastcall MSXMLWorks::GetAttributeValue(Variant Node, int AttributeIndex)
{
    return Node.OlePropertyGet("attributes").OlePropertyGet("item",AttributeIndex).OlePropertyGet("Value");
}

//---------------------------------------------------------------------------
// ���������� �������� �������� �� �����
AnsiString __fastcall MSXMLWorks::GetAttributeValue(Variant Node, AnsiString AttributeName)
{
    Variant attribute = Node.OlePropertyGet("attributes").OleFunction("getNamedItem", AttributeName);
    if (!attribute.IsEmpty())
        return attribute.OlePropertyGet("text");
    else
        return "";

    // ������ ������
    //return Node.OleFunction("GetAttribute", StringToOleStr(AttributeName));
}


//---------------------------------------------------------------------------
// ���������� �������� ��������,
// ���� ������� �����������, �� ���������� �������� DefaultValue
AnsiString __fastcall MSXMLWorks::GetAttributeValue(Variant Node, AnsiString AttributeName, String DefaultValue)
{
    AnsiString attribute = Trim(GetAttributeValue(Node, AttributeName));
    if (attribute != "") {
        return attribute;
    } else {
        return DefaultValue;
    }
}

//---------------------------------------------------------------------------
// ���������� �������� ��������,
// ���� ������� �����������, �� ���������� �������� DefaultValue
int __fastcall MSXMLWorks::GetAttributeValue(Variant Node, AnsiString AttributeName, int DefaultValue)
{
    AnsiString attribute = Trim(GetAttributeValue(Node, AttributeName));  // ������ �������
    if (attribute != "") {
        try {
            return StrToInt(attribute);
        } catch (...) {
            return DefaultValue;
        }
    } else
        return DefaultValue;
}

//---------------------------------------------------------------------------
// ���������� �������� ��������,
// ���� ������� �����������, �� ���������� �������� DefaultValue
bool __fastcall MSXMLWorks::GetAttributeValue(Variant Node, AnsiString AttributeName, bool DefaultValue)
{
    AnsiString attribute = LowerCase(Trim(GetAttributeValue(Node, AttributeName)));  // ������ �������

    if (attribute == "true")
        return true;
    else if (attribute == "false")
        return false;
    else
        return DefaultValue;
}

/*//---------------------------------------------------------------------------
// ���������� �������� ��������
AnsiString MSXMLWorks::GetValueAttribute(Variant Attribute)
{
    return Attribute.OlePropertyGet("Value");
}  */

//---------------------------------------------------------------------------
// ���������� ���������� ��������� ����
int __fastcall MSXMLWorks::GetAttributesCount(Variant Node)
{
    return Node.OlePropertyGet("attributes").OlePropertyGet("length");
}

//---------------------------------------------------------------------------
//
Variant __fastcall MSXMLWorks::GetRootNode()
{
    return xmlDoc.OlePropertyGet("DocumentElement");
}

//---------------------------------------------------------------------------
//
AnsiString __fastcall MSXMLWorks::GetNodeName(Variant Node)
{
    return Node.OlePropertyGet("NodeName");
}


//---------------------------------------------------------------------------
// ��������� ������ �������� ����
Variant __fastcall MSXMLWorks::GetFirstNode(Variant Node)
{
    return Node.OlePropertyGet("firstChild");
}

//---------------------------------------------------------------------------
// ���������� ��������� ���� �� ����������
Variant __fastcall MSXMLWorks::GetNextNode(Variant Node)
{
    return Node.OlePropertyGet("nextSibling");
}

//---------------------------------------------------------------------------
// ��������� ������� ������ ������� XML
AnsiString __fastcall MSXMLWorks::GetParseError()
{
    if( xmlDoc.OlePropertyGet("parseError").OlePropertyGet("errorCode")!=0 )
    {
        return AnsiString(xmlDoc.OlePropertyGet("parseError").OlePropertyGet("reason"));
    } else {
        return "";
    }
}
