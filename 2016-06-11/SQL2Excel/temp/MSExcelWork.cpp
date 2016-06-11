#include "MSWordWorks.h"

//#pragma link "Word_2K_SRVR"

//---------------------------------------------------------------------------

bool MSWordWorks::OpenWordDocument(String DocumentFileName, bool fAsTemplate)
{
	try {
        // �������� ������� Word.Application
        WordApp = CreateOleObject("Word.Application.8");
        //WordApp  = Variant::CreateObject("word.application");

        // ��������� ����� ������ ��������������.
        WordApp.OlePropertySet("DisplayAlerts", false);

		//WordApp.OlePropertySet("CheckSpellingAsYouType", false);

        // ����������� Word.Application
        WordApp.OlePropertySet("Visible", false);

        // ������������� ������ �� ���������
	  	Documents = WordApp.OlePropertyGet("Documents");

        // ����� ��������� �� �����
        if (fAsTemplate) {
        	// ������ ����� ����������� �������� �������� ������ Item = 1
        	Documents.OleProcedure("Add", DocumentFileName, false,0);
        	Document = Documents.OleFunction("Item",1); // ������ � ���������
        } else {
        	Document = Documents.OleFunction("Open", DocumentFileName);
        }

        GetDocumentObjects();

        return true;
    } catch (...) {
    	return false;
    }
}

bool MSWordWorks::LinkDocument(String DocFileName)
{
	try {
        // �������� ������� Word.Application
        // ����� ��������� �� �����
        //Document = Documents.OleFunction("Open" ,"C:\\_project\\InsertPicToWord\\tmp\\test2.doc");
        Document = Documents.OleFunction("Open", DocFileName);

        return true;
    } catch (...) {
    	return false;
    }
}

bool MSWordWorks::LinkDocument(int DocItemNumber)
{
	try {
        // ����� �������� �� �������� ������
        Document = WordApp.OlePropertyGet("Documents").OleFunction("Item", DocItemNumber);
        GetDocumentObjects();
		return true;
    } catch (...) {
    	return false;
    }
}

bool MSWordWorks::LinkDocument()
{
	try {
        // ����� ��������� ���������
        Document = WordApp.OlePropertyGet("ActiveDocument");
        GetDocumentObjects();
        return true;
    } catch (...) {
    	return false;
    }
}

// ������������� �������� ���������
bool MSWordWorks::GetDocumentObjects()
{
        // ������������� ������ �� ������� ��������� (������ ���� � ��������� ���� �������)
        Tables = Document.OlePropertyGet("Tables");
		// ������������� ������� Fields
        Fields = Document.OlePropertyGet("FormFields");
 		// ������������� ������� Paragraphs
		Paragraphs = Document.OlePropertyGet("Paragraphs");
		// ������������� ������� Bookmarks
        Bookmarks = Document.OlePropertyGet("Bookmarks");
		// ������������� ������� Fields
        Fields = Document.OlePropertyGet("FormFields");
        // �������� Range (� ����������� � Selection)
        //Range = Selection.OlePropertyGet("Range");
        // �������� Range (���� ��������)
        CurrentRange = Document.OleFunction("Range");

        CurrentSelection = WordApp.OlePropertyGet("Selection");


        return true;

}

// �������/����� ���������
void MSWordWorks::SetVisible(bool fVisible)
{
	// ����������� Word.Application
	WordApp.OlePropertySet("Visible", fVisible);
}

// ������� ������ � �������� (�������� ���������� ������������)
bool MSWordWorks::SetTextToBookmark(String BookmarkName, WideString Text)
{
	// ����� Bookmark �� �����
	Variant Bookmark = Bookmarks.OleFunction("Item", (OleVariant)BookmarkName);
	// ������� ������
	Bookmark.OlePropertyGet("Range").OlePropertySet("Text", Text);
}

// ������� ����������� � �������� (����� ���������� ������������)
bool MSWordWorks::SetPictureToBookmark(String BookmarkName, String PictureFileName, int Width, int Height)
{
        /*// ����� Bookmark �� �����
        Variant Bookmark = Bookmarks.OleFunction("Item", (OleVariant)BookmarkName);
        // �������� ����������� �� �����
        Bookmark.OlePropertyGet("Range").OlePropertyGet("InlineShapes").OleProcedure("AddPicture", PictureFileName, false, true);


		/**/
		//������ �������
		// ����������� ���������� ������� � ���������
        Variant Selection = WordApp.OlePropertyGet("Selection");
 		// ������� � �������� �� �� �����
        Selection.OleProcedure("GoTo",(int)-1, 0, 0, WideString(BookmarkName));
        // �������� ����������� �� ����� � ���� Selection
        Selection.OlePropertyGet("InlineShapes").OleProcedure("AddPicture", PictureFileName, false, true);
        /*/

		// �������� ����� �������!!!!
        Variant InlineShape, PictureShape;
        InlineShape =  Document.OlePropertyGet("InlineShapes").OleFunction("Item", 1);
        //InlineShape = Field.OlePropertyGet("Range").OlePropertyGet("InlineShapes").OleFunction("Item", i);

        // ������������ � Shape
        PictureShape = InlineShape.OleFunction("ConvertToShape");
        // ������������ ����������� ����� �������
        PictureShape.OleFunction("ZOrder", 5);
        // ������������� ������ �����������
        if (Width != 0 && Height != 0) {
        	PictureShape.OlePropertySet("Width", Width);
        	PictureShape.OlePropertySet("Height", Height);
        }    */
}

// ������� ������ � ���� (���� ���������� �������)
bool MSWordWorks::SetTextToField(String FieldName, WideString Text)
{
        // ����� Field �� �����
        Variant Field = Fields.OleFunction("Item", (OleVariant)FieldName);
        // ������� ������
        Field.OlePropertyGet("Range").OlePropertySet("Text", Text);
}

// ������� ������� � ���� (���� ��������)
bool MSWordWorks::SetPictureToField(String FieldName, String PictureFileName, int Width, int Height)
{
        // ����� Field �� �����
        Variant Field = Fields.OleFunction("Item", (OleVariant)FieldName);
        // �������� ����������� �� �����
        Field.OlePropertyGet("Range").OlePropertyGet("InlineShapes").OleProcedure("AddPicture", PictureFileName, false, true);





/*
		//������ �������
		// ������� � �������� �� �� �����
        Variant InlineShape, PictureShape;
		// �������� ����� ������������ �����������
        int i;
        i = Field.OlePropertyGet("Range").OlePropertyGet("InlineShapes").OlePropertyGet("Count");
		i = Document.OlePropertyGet("InlineShapes").OlePropertyGet("Count");
        InlineShape =  Document.OlePropertyGet("InlineShapes").OleFunction("Item", i);
        //InlineShape = Field.OlePropertyGet("Range").OlePropertyGet("InlineShapes").OleFunction("Item", i);

        // ������������ � Shape
        PictureShape = InlineShape.OleFunction("ConvertToShape");
        // ������������ ����������� ����� �������
        PictureShape.OleFunction("ZOrder", 5);
        // ������������� ������ �����������
        if (Width != 0 || Height != 0) {
        	PictureShape.OlePropertySet("Width", Width);
        	PictureShape.OlePropertySet("Height", Height);
        }
        // ������������� ������������ �� �����
//        PictureShape.OleFunction("IncrementLeft",30);
//        PictureShape.OleFunction("IncrementTop",40);*/
}

// ����� ������ � �������
bool MSWordWorks::FindTextForReplace(String Text, String ReplaceText, bool fReg)
{
        // ����������� ���������� ������� � ���������
        Variant Selection = WordApp.OlePropertyGet("Selection");

        // ����� ������ �� ����� ���������
		Variant Find = Selection.OlePropertyGet("Find");
        Find.OleProcedure("Execute", Text/*�����, ������� ����� ������*/, fReg/*��������� �������e*/,
        	false/*������ ������ �����*/,false/*��������� ������������� �������*/,false/*������ ������������ ���*/,
        	false/*������ ��� ����������*/,true/*������ ������*/,1/*��������� ������ ����� �����*/,
        	false/* ������� ������� */, ReplaceText/*�� ��� ��������*/,2/*�������� ���*/);   // ���� ������� ��������

        /*
        Find.OleProcedure("ClearFormatting");                                         // ���� ������� �� ��������, ���� �����������
        Find.OlePropertyGet("Replacement").OleProcedure("ClearFormatting");
        Find.OlePropertySet("Text",Text);
        Find.OlePropertyGet("Replacement").OlePropertySet(Text,ReplaceText);
        Find.OlePropertySet("Forward",True);
        Find.OlePropertySet("Wrap",1);
        Find.OlePropertySet("Format",False);
        Find.OlePropertySet("MatchCase",False);
        Find.OlePropertySet("MatchWholeWord",False);
        Find.OlePropertySet("MatchWildcards",False);
        Find.OlePropertySet("MatchSoundsLike",False);
        Find.OlePropertySet("MatchAllWordForms",False);
        Find.OleProcedure("Execute",2);   /**/
}

// ����������� ������ � ������ ��������� (����������!)
void MSWordWorks::MoveUpCursor()
{
	//Variant Selection = Document.OlePropertyGet("Selection");
	// ����������� ������
	CurrentSelection.OleProcedure("MoveUp", 7, 1);
}

// ��������� � ����
bool MSWordWorks::SaveAsDocument(String FileName)
{
    try {
        // ���������� ��������� � ����
		Document.OleProcedure("SaveAs",FileName);
		return true;
    } catch (...) {
		return false;
    }
}



bool MSWordWorks::SetActiveDocument()
{
	// ��������� ���������
    Document.OleProcedure("Activate");
}


bool MSWordWorks::CreateTable(int nCols, int nRows)
{
	// ������� ������� � ������� Range
    Tables.OleProcedure("Add", CurrentRange, (OleVariant) nCols, (OleVariant) nRows);

	// ����� ������������ �������
	//Table = Tables.OleFunction("Item", 1);
	//RowCount = Table.OlePropertyGet("Rows").OlePropertyGet("Count");
	//ColCount = Table.OlePropertyGet("Columns").OlePropertyGet("Count");
}

bool MSWordWorks::GoToBookmark(String BookmarkName)
{
	// ��������� ���������
	CurrentSelection.OleProcedure("GoTo",(int)-1, 0, 0, WideString(BookmarkName));
}

bool MSWordWorks::GoToText(String Text, bool fReg, bool fWord)
{
		// ����������� ���������� ������� � ���������
        //Variant Selection = WordApp.OlePropertyGet("Selection");
        //CurrentSelection = WordApp.OlePropertyGet("Selection");

        CurrentSelection = Document.OleFunction("Range");

        // ����� ������ �� ����� ���������
		Variant Find = CurrentSelection.OlePropertyGet("Find");
        Find.OleProcedure("Execute", Text/*�����, ������� ����� ������*/, fReg/*��������� �������e*/,
        	fWord/*������ ������ �����*/,false/*��������� ������������� �������*/,false/*������ ������������ ���*/,
        	false/*������ ��� ����������*/,true/*������ ������*/,1/*��������� ������ ����� �����*/,
        	false/* ������� ������� */, 0/*�� ��� ��������*/,0/*�������� ���*/);   // ���� ������� ��������

//	CurrentSelection.OleProcedure("GoTo",(int)-1, 0, 0, WideString(BookmarkName));
}

bool MSWordWorks::InsertPicture(String PictureFileName, int Width, int Height)
{
     // �������� ����������� �� ����� � ������� CurrentSelection
     CurrentSelection.OlePropertyGet("InlineShapes").OleProcedure("AddPicture", "C:\\_project\\InsertPicToWord\\tmp\\podpis.bmp", false, true);
}

bool MSWordWorks::InsertText(WideString Text)
{
     // �������� ����� � ������� CurrentSelection
     CurrentSelection.OleProcedure("TypeText", Text);
}

bool MSWordWorks::PasteFromClipboard()
{
     // �������� �� �������
	WordApp.OlePropertyGet("Selection").OleFunction("Paste");
    return true;
}

bool MSWordWorks::CloseApplication()
{
	// �������� ���������� Word (� ������� �� ���������� ���������)
	WordApp.OleProcedure("Quit");
	return true;
}

bool MSWordWorks::CloseDocument()
{
	Document.OleFunction("Close");
   	return true;
}

int MSWordWorks::GetPagesCount()
{
	// wdPropertyPages = 14
	return Document.OlePropertyGet("BuiltInDocumentProperties").OlePropertyGet("Item", 14).OlePropertyGet("Value");
}
