#include "MSWordWorks.h"

//#pragma link "Word_2K_SRVR"

//---------------------------------------------------------------------------

bool MSWordWorks::OpenWordDocument(String DocumentFileName, bool fAsTemplate)
{
	try {
        // Создание объекта Word.Application
        WordApp = CreateOleObject("Word.Application.8");
        //WordApp  = Variant::CreateObject("word.application");

        // Отключить режим показа предупреждений.
        WordApp.OlePropertySet("DisplayAlerts", false);

		//WordApp.OlePropertySet("CheckSpellingAsYouType", false);

        // Отображение Word.Application
        WordApp.OlePropertySet("Visible", false);

        // Инициализация ссылки на документы
	  	Documents = WordApp.OlePropertyGet("Documents");

        // Выбор документа из файла
        if (fAsTemplate) {
        	// Каждый вновь создаваемый документ получает индекс Item = 1
        	Documents.OleProcedure("Add", DocumentFileName, false,0);
        	Document = Documents.OleFunction("Item",1); // Доступ к документу
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
        // Создание объекта Word.Application
        // Выбор документа из файла
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
        // Выбор докумета из открытых файлов
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
        // Выбор документа активного
        Document = WordApp.OlePropertyGet("ActiveDocument");
        GetDocumentObjects();
        return true;
    } catch (...) {
    	return false;
    }
}

// Инициализация объектов Документа
bool MSWordWorks::GetDocumentObjects()
{
        // Инициализация ссылки на таблицы документа (только если в документе есть таблицы)
        Tables = Document.OlePropertyGet("Tables");
		// Инициализация объекта Fields
        Fields = Document.OlePropertyGet("FormFields");
 		// Инициализация объекта Paragraphs
		Paragraphs = Document.OlePropertyGet("Paragraphs");
		// Инициализация объекта Bookmarks
        Bookmarks = Document.OlePropertyGet("Bookmarks");
		// Инициализация объекта Fields
        Fields = Document.OlePropertyGet("FormFields");
        // Создание Range (в соответсвии с Selection)
        //Range = Selection.OlePropertyGet("Range");
        // Создание Range (весь документ)
        CurrentRange = Document.OleFunction("Range");

        CurrentSelection = WordApp.OlePropertyGet("Selection");


        return true;

}

// Скрытие/показ документа
void MSWordWorks::SetVisible(bool fVisible)
{
	// Отображение Word.Application
	WordApp.OlePropertySet("Visible", fVisible);
}

// Вставка текста в закладку (закладка заменяется изображением)
bool MSWordWorks::SetTextToBookmark(String BookmarkName, WideString Text)
{
	// Выбор Bookmark по имени
	Variant Bookmark = Bookmarks.OleFunction("Item", (OleVariant)BookmarkName);
	// Вставка текста
	Bookmark.OlePropertyGet("Range").OlePropertySet("Text", Text);
}

// Вставка изображения в закладку (текст заменяется изображением)
bool MSWordWorks::SetPictureToBookmark(String BookmarkName, String PictureFileName, int Width, int Height)
{
        /*// Выбор Bookmark по имени
        Variant Bookmark = Bookmarks.OleFunction("Item", (OleVariant)BookmarkName);
        // Вставить изображение из файла
        Bookmark.OlePropertyGet("Range").OlePropertyGet("InlineShapes").OleProcedure("AddPicture", PictureFileName, false, true);


		/**/
		//Второй вариант
		// Определение выбранного участка в документе
        Variant Selection = WordApp.OlePropertyGet("Selection");
 		// Переход к закладке по ее имени
        Selection.OleProcedure("GoTo",(int)-1, 0, 0, WideString(BookmarkName));
        // Вставить изображение из файла в поле Selection
        Selection.OlePropertyGet("InlineShapes").OleProcedure("AddPicture", PictureFileName, false, true);
        /*/

		// возможно нужно удалить!!!!
        Variant InlineShape, PictureShape;
        InlineShape =  Document.OlePropertyGet("InlineShapes").OleFunction("Item", 1);
        //InlineShape = Field.OlePropertyGet("Range").OlePropertyGet("InlineShapes").OleFunction("Item", i);

        // Конвертируем в Shape
        PictureShape = InlineShape.OleFunction("ConvertToShape");
        // Расположение изображения перед текстом
        PictureShape.OleFunction("ZOrder", 5);
        // Устанавливаем размер изображения
        if (Width != 0 && Height != 0) {
        	PictureShape.OlePropertySet("Width", Width);
        	PictureShape.OlePropertySet("Height", Height);
        }    */
}

// Вставка текста в поле (поле заменяется текстом)
bool MSWordWorks::SetTextToField(String FieldName, WideString Text)
{
        // Выбор Field по имени
        Variant Field = Fields.OleFunction("Item", (OleVariant)FieldName);
        // Вставка текста
        Field.OlePropertyGet("Range").OlePropertySet("Text", Text);
}

// Вставка рисунка в поле (поле остается)
bool MSWordWorks::SetPictureToField(String FieldName, String PictureFileName, int Width, int Height)
{
        // Выбор Field по имени
        Variant Field = Fields.OleFunction("Item", (OleVariant)FieldName);
        // Вставить изображение из файла
        Field.OlePropertyGet("Range").OlePropertyGet("InlineShapes").OleProcedure("AddPicture", PictureFileName, false, true);





/*
		//Второй вариант
		// Переход к закладке по ее имени
        Variant InlineShape, PictureShape;
		// Получаем номер вставленного изображения
        int i;
        i = Field.OlePropertyGet("Range").OlePropertyGet("InlineShapes").OlePropertyGet("Count");
		i = Document.OlePropertyGet("InlineShapes").OlePropertyGet("Count");
        InlineShape =  Document.OlePropertyGet("InlineShapes").OleFunction("Item", i);
        //InlineShape = Field.OlePropertyGet("Range").OlePropertyGet("InlineShapes").OleFunction("Item", i);

        // Конвертируем в Shape
        PictureShape = InlineShape.OleFunction("ConvertToShape");
        // Расположение изображения перед текстом
        PictureShape.OleFunction("ZOrder", 5);
        // Устанавливаем размер изображения
        if (Width != 0 || Height != 0) {
        	PictureShape.OlePropertySet("Width", Width);
        	PictureShape.OlePropertySet("Height", Height);
        }
        // Устанавливаем расположение на листе
//        PictureShape.OleFunction("IncrementLeft",30);
//        PictureShape.OleFunction("IncrementTop",40);*/
}

// Поиск текста с заменой
bool MSWordWorks::FindTextForReplace(String Text, String ReplaceText, bool fReg)
{
        // Определение выбранного участка в документе
        Variant Selection = WordApp.OlePropertyGet("Selection");

        // Поиск текста по всему документу
		Variant Find = Selection.OlePropertyGet("Find");
        Find.OleProcedure("Execute", Text/*Текст, который будем менять*/, fReg/*учитывать регистрe*/,
        	false/*Только полное слово*/,false/*Учитывать универсальные символы*/,false/*Флажок Произносится как*/,
        	false/*Флажок Все словоформы*/,true/*Искать вперед*/,1/*Активация кнопки Найти далее*/,
        	false/* Задание формата */, ReplaceText/*На что заменить*/,2/*Заменить все*/);   // Этот вариант работает

        /*
        Find.OleProcedure("ClearFormatting");                                         // Этот вариант НЕ работает, надо разбираться
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

// Переместить курсор в начало документа (недоделано!)
void MSWordWorks::MoveUpCursor()
{
	//Variant Selection = Document.OlePropertyGet("Selection");
	// Переместить курсор
	CurrentSelection.OleProcedure("MoveUp", 7, 1);
}

// Сохранить в файл
bool MSWordWorks::SaveAsDocument(String FileName)
{
    try {
        // Сохранение документа в файл
		Document.OleProcedure("SaveAs",FileName);
		return true;
    } catch (...) {
		return false;
    }
}



bool MSWordWorks::SetActiveDocument()
{
	// Активация документа
    Document.OleProcedure("Activate");
}


bool MSWordWorks::CreateTable(int nCols, int nRows)
{
	// Создане таблицы в области Range
    Tables.OleProcedure("Add", CurrentRange, (OleVariant) nCols, (OleVariant) nRows);

	// Выбор существующей таблицы
	//Table = Tables.OleFunction("Item", 1);
	//RowCount = Table.OlePropertyGet("Rows").OlePropertyGet("Count");
	//ColCount = Table.OlePropertyGet("Columns").OlePropertyGet("Count");
}

bool MSWordWorks::GoToBookmark(String BookmarkName)
{
	// Активация документа
	CurrentSelection.OleProcedure("GoTo",(int)-1, 0, 0, WideString(BookmarkName));
}

bool MSWordWorks::GoToText(String Text, bool fReg, bool fWord)
{
		// Определение выбранного участка в документе
        //Variant Selection = WordApp.OlePropertyGet("Selection");
        //CurrentSelection = WordApp.OlePropertyGet("Selection");

        CurrentSelection = Document.OleFunction("Range");

        // Поиск текста по всему документу
		Variant Find = CurrentSelection.OlePropertyGet("Find");
        Find.OleProcedure("Execute", Text/*Текст, который будем менять*/, fReg/*учитывать регистрe*/,
        	fWord/*Только полное слово*/,false/*Учитывать универсальные символы*/,false/*Флажок Произносится как*/,
        	false/*Флажок Все словоформы*/,true/*Искать вперед*/,1/*Активация кнопки Найти далее*/,
        	false/* Задание формата */, 0/*На что заменить*/,0/*Заменить все*/);   // Этот вариант работает

//	CurrentSelection.OleProcedure("GoTo",(int)-1, 0, 0, WideString(BookmarkName));
}

bool MSWordWorks::InsertPicture(String PictureFileName, int Width, int Height)
{
     // Вставить изображение из файла в позицию CurrentSelection
     CurrentSelection.OlePropertyGet("InlineShapes").OleProcedure("AddPicture", "C:\\_project\\InsertPicToWord\\tmp\\podpis.bmp", false, true);
}

bool MSWordWorks::InsertText(WideString Text)
{
     // Вставить текст в позицию CurrentSelection
     CurrentSelection.OleProcedure("TypeText", Text);
}

bool MSWordWorks::PasteFromClipboard()
{
     // Вставить из буффера
	WordApp.OlePropertyGet("Selection").OleFunction("Paste");
    return true;
}

bool MSWordWorks::CloseApplication()
{
	// Закрытие приложения Word (с запосом на сохранение документа)
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
