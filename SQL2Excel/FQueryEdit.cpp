//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "FQueryEdit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormQueryEdit *FormQueryEdit;

//---------------------------------------------------------------------------
__fastcall TFormQueryEdit::TFormQueryEdit(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
//
void __fastcall TFormQueryEdit::FormShow(TObject *Sender)
{

    AnsiString Str = "SELECT * FROM spr_task_sql2excel order by taborder,razdel,sortorder,queryname";
    TOraQuery *OraQuery = OpenOraQuery(Form1->EsaleSession, Str);

    if (OraQuery == NULL) {
        delete OraQuery;
        return;
    }

    // Загрузка из OraQuery в списки
    Form1->DataSetToQueryList(OraQuery, QueryList, TabList);



   /* std::vector<int> tab_iterator = queryitem.begin();
    while (tab_iterator != TabList.end()) {
                                             */
    ChangeList.reserve(QueryList.size());

    for (unsigned int i = 0; i < ChangeList.capacity(); i++) {
        ChangeList[i] = 0;
    }


    pTabList = &TabList;

    FillTabLV();
    FillQueryLV();

    //FormFill();
}

//---------------------------------------------------------------------------
//
void TFormQueryEdit::FillQueryLV()
{

    if (TabLV->Selected == NULL)
        return;

    int index = TabLV->Selected->Index;
    pCurrentTab = &(*pTabList)[index];

//        int k = TabLV->Selected[0].Index;

    QueryLV->Items->BeginUpdate();
    QueryLV->Items->Clear();

    //TABITEM* pTabItem = &(*pTabList)[index];


   /* std::vector<QUERYITEM*> tab_iterator = queryitem.begin();
    while (tab_iterator != TabList.end()) {
        TListItem *Item = QueryLV->Items->Add();
        Item->Caption = pCurrentTab->queryitem[i]->queryname;
        Item->Caption = pCurrentTab->queryitem[i]->queryname;
      //total += *the_iterator++;
    }  */

    for (unsigned int i = 0; i < pCurrentTab->queryitem.size(); i++) {
        TListItem *Item = QueryLV->Items->Add();
        Item->Caption = pCurrentTab->queryitem[i]->queryname;
            /*TListItem *Item = ListView1->Items->Item[i];
            Item->Caption = sName;
            TreeView1->Items->Add(NULL, sName);  */
    }
    QueryLV->Items->EndUpdate();

    if (QueryLV->Items->Count > 0) {
        QueryLV->Items->Item[0]->Selected = true;
        FillOptions();
    }




    //AnsiString curTabCaption = TabControl1->Tabs->Strings[TabControl1->TabIndex];

   // ListBox1->Items->BeginUpdate();
   // ListBox1->Clear();
/*    for (int i = 0; i < Form1->QueryList.size(); i++) {
        // Заполняем ListBox запросами

        AnsiString s = Form1->QueryList[i].razdel;

//        if (s == curTabCaption) {                   // Ищем вкладку по заголовку
            AnsiString sName = Form1->QueryList[i].queryname;   // QueryName
       //     AnsiString sFields = QueryList[i].fieldslist; // Fields
        //    ListBox1->Items->Add(sName + "\\n" + sFields);

            TabLV->Items->Add();

            /*TListItem *Item = ListView1->Items->Item[i];
            Item->Caption = sName;

            TreeView1->Items->Add(NULL, sName);
                                                */
//        }

/*    ListBox1->Items->EndUpdate();
    if (ListBox1->Items->Count > 0)
        ListBox1->ItemIndex = 0;


 //   TListItem *ListItem;

    ListView1->Items->Clear();
	for (unsigned int i = 0; i < Parameters[SelIndex].size(); i++) {
        PARAMRECORD *record = &Parameters[SelIndex][i];
        ListView1->Items->Add();
        TListItem *Item = ListView1->Items->Item[ListView1->Items->Count-1];
        Item->Caption = record->label.c_str();
        Item->SubItems->Add(record->display.c_str());
    }

*/


}


//---------------------------------------------------------------------------
//
void TFormQueryEdit::FillTabLV()
{
    TabLV->Items->BeginUpdate();
    TabLV->Items->Clear();
    // Заполняем ListBox запосами
    for (unsigned int i = 0; i < pTabList->size(); i++) {
        TABITEM* pTabItem = &(*pTabList)[i];
        TListItem *Item = TabLV->Items->Add();
        Item->Caption = pTabItem->name;
    }
    TabLV->Items->EndUpdate();
}

//---------------------------------------------------------------------------
//
void __fastcall TFormQueryEdit::TabLVClick(TObject *Sender)
{
    //int paramindex = TabLV->Selected->Index;
/*    PARAMRECORD *param;
    param = &CurrentQueryItem->Parameters[paramindex];
*/
    FillQueryLV();
}
//---------------------------------------------------------------------------
//
void __fastcall TFormQueryEdit::QueryLVClick(TObject *Sender)
{
    //
    FillOptions();
}
//---------------------------------------------------------------------------
//
void TFormQueryEdit::FillOptions()
{
    //

    if (QueryLV->Selected == NULL)
        return;

    int queryindex = QueryLV->Selected->Index;
    QUERYITEM* pQueryItem;


    pQueryItem = pCurrentTab->queryitem[queryindex];
    SqlTextREdt->Text = pCurrentTab->queryitem[queryindex]->querytext;


    ParametersLB->Clear();
    for (unsigned int i=0; i < pQueryItem->UserParams.size(); i++)
    {
        ParametersLB->Items->Add(pQueryItem->UserParams[i].name);
    }
    //Edit2->Text = pTabItem->queryitem[queryindex]->querytext;

    if (ParametersLB->Items->Count > 0) {
        ParametersLB->ItemIndex = 0;
        FillParameters();
    }
}

//---------------------------------------------------------------------------
//
void TFormQueryEdit::FillParameters()
{
    int queryindex = QueryLV->Selected->Index;
    int paramindex = ParametersLB->ItemIndex;

    QUERYITEM* pQueryItem = pCurrentTab->queryitem[queryindex];
    PARAMRECORD *pParamItem = &pQueryItem->UserParams[paramindex];


    VarTypeCB->ItemIndex = VarTypeCB->Items->IndexOf(pParamItem->type);
    VarNameEdt->Text = pParamItem->name;
    VarDispNameEdt->Text = pParamItem->label;
    VarValueCB->Text = pParamItem->value;
    if (pParamItem->dbindex != "")
    {
        int dbindex = StrToInt(pParamItem->dbindex);
        VarDBIndexCB->ItemIndex = dbindex;
    } else {

    }
    VarValueFormatCB->Text = pParamItem->format;
    //VarVisibleChkBox->Checked = pParamItem->visible;


    if (pParamItem->listitem.size() > 0) {
        VarListValSGrid->RowCount = pParamItem->listitem.size();
        for (unsigned int i=0; i < pParamItem->listitem.size(); i++) {
            VarListValSGrid->Cells[0][i] = pParamItem->listitem[i].label;
            VarListValSGrid->Cells[1][i] = pParamItem->listitem[i].value;
        }
    } else {
        VarListValSGrid->RowCount = 0;
    }

    //VarTypeCB->Text = pParamItem->type;
    //Add(pQueryItem->Parameters[i].name);
}


void __fastcall TFormQueryEdit::BitBtn15Click(TObject *Sender)
{

    ChangeList[1] = chRewrite;


    for (unsigned int i = 0; i < ChangeList.capacity(); i++) {
        switch (ChangeList[i]) {
        case chNone:
            continue;
        case chRewrite:
            ChangeRewrite(i);

            break;
        case chAdd:
            break;
        case chDel:
            break;


        }

    }
}

//---------------------------------------------------------------------------
//
void __fastcall TFormQueryEdit::ChangeRewrite(int itemindex)
{
    for (unsigned int i = 0; i < QueryList.size(); i++) {
        if (QueryList[i].queryid == "12") {
            itemindex = i;
            break;
        }
    }

    QUERYITEM* item = &QueryList[itemindex];


    //AnsiString Str = "UPDATE spr_task_sql2excel SET taborder=:taborder, razdel=:razdel, sortorder=:sortorder, queryname=:queryname"
    //    " WHERE SPR_TASK_SQL2EXCEL_ID=:id";
    AnsiString Str = "UPDATE spr_task_sql2excel SET"
        " queryname=:queryname, "
        " sqltext=:sqltext "
//        " razdel=:razdel, "
//        " sortorder=:sortorder, "
//        " taborder=:taborder "
        " WHERE SPR_TASK_SQL2EXCEL_ID=:id";


    TOraQuery* OraQuery = new TOraQuery(NULL);
    OraQuery->Session = Form1->EsaleSession;

    OraQuery->SQL->Add(Str);

    OraQuery->Prepare();
    OraQuery->ParamByName("id")->Value = 12;
    OraQuery->ParamByName("queryname")->AsString = item->queryname;
    OraQuery->ParamByName("sqltext")->AsString = item->querytext;
//    OraQuery->ParamByName("taborder")->AsString = item->taborder;
//    OraQuery->ParamByName("razdel")->AsString = item->razdel;
//    OraQuery->ParamByName("sortorder")->AsString = item->sortorder;


    OraQuery->ExecSQL();
    OraQuery->Session->Commit();
    OraQuery->Close();

    delete OraQuery;
}


/*
void __fastcall TFormQueryEdit::ChangeAdd(int itemindex)
{

INSERT INTO PEOPLE(ID, NM, FM, OT)
  2     VALUES(5, NULL, NULL, NULL)


} */





void __fastcall TFormQueryEdit::Button1Click(TObject *Sender)
{
    if (QueryLV->Selected == NULL)
        return;

    int queryindex = QueryLV->Selected->Index;

    //ChangeList[]
    QUERYITEM* item = pCurrentTab->queryitem[queryindex];

    item->querytext = SqlTextREdt->Text;
    item->queryname = QueryLV->Selected->Caption;
    //item->querytext = SqlTextREdt->Text;


}
//---------------------------------------------------------------------------


void __fastcall TFormQueryEdit::VarShowCodeBtnClick(TObject *Sender)
{
    int queryindex = QueryLV->Selected->Index;
    //int paramindex = ParametersLB->ItemIndex;

    QUERYITEM* pQueryItem = pCurrentTab->queryitem[queryindex];
    //PARAMRECORD *pParamItem = &pQueryItem->Parameters[paramindex];
    AnsiString sParams;


    sParams = "<?xml version=\"1.0\"?>\n"
        "<parameters>\n";

    for (unsigned int i=0; i < pQueryItem->UserParams.size(); i++) {
        PARAMRECORD *pParamItem = &pQueryItem->UserParams[i];

        sParams += "<parameter";
        sParams += " type=\"" +pParamItem->type + "\"";
        sParams += " name=\"" +pParamItem->name + "\"";
        sParams += " label=\"" +pParamItem->label + "\"";
        sParams += " value=\"" +pParamItem->value_src + "\"";
        sParams += " format=\"" +pParamItem->format + "\"";
        sParams += " src=\"" +pParamItem->src + "\"";
        sParams += " dbindex=\"" +pParamItem->dbindex + "\"";
        sParams += ">\n";

        if (pParamItem->type == "list" && pParamItem->src == "") {
            //std::vector <PARAMLISTITEM>* paramlistitem = &pParamItem->listitem;

            //if (pParamItem->src != "") {

            //} else {
                //for (int j=0; j<paramlistitem->size(); j++) {
                for (unsigned int j = 0; j < pParamItem->listitem.size(); j++) {
                    sParams += "<item";
                    sParams += " value=\"" + pParamItem->listitem[j].value + "\"";
                    sParams += " label=\"" + pParamItem->listitem[j].label + "\"";
                    //sParams += " value=\"" + paramlistitem[j].value ;
                    //sParams += " label=\"" + paramlistitem[j].label;
                    sParams += "/>\n" ;
                //}

            }
        }


        sParams += "</parameter>\n";



     /*   <parameter type="list" name="sezon" value="0" label="??????????">
        <item value="1" label="????????"/>
        <item value="2" label="?? ????????"/>
        <item value="3" label="???"/>
    </parameter>              */
    }

    sParams += "</parameters>";

    ShowMessage(sParams);


}
//---------------------------------------------------------------------------



void __fastcall TFormQueryEdit::FormCreate(TObject *Sender)
{
    ImageList1->GetBitmap(0, TabDelBitBtn->Glyph);
    ImageList1->GetBitmap(1, TabAddBitBtn->Glyph);
    ImageList1->GetBitmap(2, TabUpBitBtn->Glyph);
    ImageList1->GetBitmap(3, TabDownBitBtn->Glyph);

    ImageList1->GetBitmap(0, ParamDelBitBtn->Glyph);
    ImageList1->GetBitmap(1, ParamAddBitBtn->Glyph);
    ImageList1->GetBitmap(2, ParamUpBitBtn->Glyph);
    ImageList1->GetBitmap(3, ParamDownBitBtn->Glyph);


}
//---------------------------------------------------------------------------

void __fastcall TFormQueryEdit::ParametersLBClick(TObject *Sender)
{
    FillParameters();    
}
//---------------------------------------------------------------------------


