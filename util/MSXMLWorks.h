//---------------------------------------------------------------------------
#ifndef XMLWORKS
#define XMLWORKS

/*******************************************************************************
	Класс для работ с OLE-обектом MSXml.Application
    Версия от 08.10.2014


*******************************************************************************/

#include "system.hpp"
#include <utilcls.h>
#include "Comobj.hpp"
#include <fstream.h>
#include "taskutils.h"

class MSXMLWorks
{

private:

public:
    MSXMLWorks();
    void __fastcall LoadXMLFile(AnsiString XMLFileName);
    void __fastcall LoadXMLText(AnsiString XMLText);

    Variant __fastcall GetRootNode();
    AnsiString __fastcall GetNodeName(Variant Node);
    Variant __fastcall GetFirstNode(Variant Node);
    Variant __fastcall GetNextNode(Variant Node);

    AnsiString __fastcall GetAttributeValue(Variant Node, AnsiString AttributeName, String DefaultValue);
    bool __fastcall GetAttributeValue(Variant Node, AnsiString AttributeName, bool DefaultValue);
    int __fastcall GetAttributeValue(Variant Node, AnsiString AttributeName, int DefaultValue);

    AnsiString __fastcall GetAttributeValue(Variant Node, int AttributeIndex);
    AnsiString __fastcall GetAttributeValue(Variant Node, AnsiString AttributeName);
    Variant GetAttribute(Variant Node, AnsiString AttributeName);

    //AnsiString __fastcall GetValueAttribute(Variant Attribute);
    int __fastcall GetAttributesCount(Variant Node);

    AnsiString __fastcall GetParseError();

    Variant xmlDoc;
};

//---------------------------------------------------------------------------
#endif
