//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Waiting.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDBaseControl"
#pragma link "LMDBaseGraphicControl"
#pragma link "LMDBaseMeter"
#pragma link "LMDControl"
#pragma link "LMDCustomProgress"
#pragma link "LMDGraphicControl"
#pragma link "LMDProgress"
#pragma link "ESColorMemo"
#pragma resource "*.dfm"

TWaitingPanel *WaitingPanel;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
__fastcall TWaitingPanel::TWaitingPanel(TComponent* Owner) : TForm(Owner)
{
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TWaitingPanel::FormShow(TObject *Sender)
{
	Timer->Interval = 200;
	Timer->Enabled = true;
	WaitingLMDProgress->Position = 0;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TWaitingPanel::TimerTimer(TObject *Sender)
{
	if(WaitingLMDProgress->Position == 100) WaitingLMDProgress->Position = 0;
	else WaitingLMDProgress->StepAdd();

}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TWaitingPanel::FormHide(TObject *Sender)
{
	Timer->Enabled = false;
}
//---------------------------------------------------------------------------

