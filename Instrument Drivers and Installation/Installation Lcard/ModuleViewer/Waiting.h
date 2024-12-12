//---------------------------------------------------------------------------
#ifndef _Waiting_H_
#define _Waiting_H_
//---------------------------------------------------------------------------
	#include <Classes.hpp>
	#include <Controls.hpp>
	#include <StdCtrls.hpp>
	#include <Forms.hpp>
	#include "LMDBaseControl.hpp"
	#include "LMDBaseGraphicControl.hpp"
	#include "LMDBaseMeter.hpp"
	#include "LMDControl.hpp"
	#include "LMDCustomProgress.hpp"
	#include "LMDGraphicControl.hpp"
	#include "LMDProgress.hpp"
	#include <ExtCtrls.hpp>
	#include "ESColorMemo.hpp"

	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	class TWaitingPanel : public TForm
	{
		__published:	// IDE-managed Components
			TLMDProgress *WaitingLMDProgress;
			TTimer *Timer;
			TESColorMemo *ESColorMemo1;
			void __fastcall TimerTimer(TObject *Sender);
			void __fastcall FormShow(TObject *Sender);
			void __fastcall FormHide(TObject *Sender);

		private:	// User declarations

		public:		// User declarations
			__fastcall TWaitingPanel(TComponent* Owner);
	};
	//---------------------------------------------------------------------------
	extern PACKAGE TWaitingPanel *WaitingPanel;
	//---------------------------------------------------------------------------

#endif		// _Waiting_H_
