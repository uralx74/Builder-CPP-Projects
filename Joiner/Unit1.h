//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "TransferModule.h"
#include "Logger.h"
#include "encoder.h"
#include "xmlutil.h"
#include <Dialogs.hpp>
#include <Menus.hpp>

                     

//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TButton *ButtonStart;
    TButton *OpenConfigButton;
    TRichEdit *LogRichEdit;
    TOpenDialog *OpenDialog1;
    TPopupMenu *PopupMenu1;
    TMenuItem *N1;
    TButton *ExitButton;
    void __fastcall ButtonStartClick(TObject *Sender);
    void __fastcall OpenConfigButtonClick(TObject *Sender);
    void __fastcall LogRichEditContextPopup(TObject *Sender,
          TPoint &MousePos, bool &Handled);
    void __fastcall N1Click(TObject *Sender);
    void __fastcall ExitButtonClick(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormActivate(TObject *Sender);
private:
    TLogger* Logger;
    TCommandLine* CommandLine;
    void UpdateInterface();
    void ShowCode(int ScopeType);
    void StartTransfer();

public:
    __fastcall TForm1(TComponent* Owner);
    TTransferModule TransferModule;
};
  
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
