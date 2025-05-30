//---------------------------------------------------------------------------
#ifndef _About_H_
#define _About_H_
//---------------------------------------------------------------------------
	//
	#include <Classes.hpp>
	#include <Controls.hpp>
	#include <StdCtrls.hpp>
	#include <Forms.hpp>
	#include <Buttons.hpp>
	#include <Graphics.hpp>
	#include "LMDCustomBevelPanel.hpp"
	#include "LMDCustomControl.hpp"
	#include "LMDCustomGroupBox.hpp"
	#include "LMDCustomPanel.hpp"
	#include "LMDCustomPanelFill.hpp"
	#include "LMDCustomParentPanel.hpp"
	#include "LMDGroupBox.hpp"
	#include "LMDButton.hpp"
	#include "LMDCustomButton.hpp"
	#include "LMDBaseControl.hpp"
	#include "LMDBaseGraphicControl.hpp"
	#include "LMDBaseLabel.hpp"
	#include "LMDControl.hpp"
	#include "LMDCustomLabel.hpp"
	#include "LMDLabel.hpp"
	//
	#include "PJVersionInfo.hpp"
	#include "ESColorMemo.hpp"
	#include "URLLink.hpp"

	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	class TAboutProgramPanel : public TForm
	{
		__published:	// IDE-managed Components
			TLMDGroupBox *VersionsLMDGroupBox;
			TLMDLabel *ModulesViewerVersionLabelLMDLabel;
			TLMDLabel *LusbapiVersionLabelLMDLabel;
			TLMDLabel *ModulesViewerVersionLMDLabel;
			TLMDLabel *LusbapiVersionLMDLabel;
			TLMDLabel *LdevusbuVersionLabelLMDLabel;
			TLMDLabel *LdevusbuVersionLMDLabel;
			TLMDButton *OkLMDButton;
			TPJVersionInfo *PJVersionInfo;
			TESColorMemo *AboutESColorMemo;
			TStaticText *StaticText1;
			TStaticText *StaticText2;
			TURLLink *LcardUrlLink;
			TURLLink *URLLink;
			TURLLink *URLLink1;
			TESColorMemo *ESColorMemo;
			void __fastcall OkLMDButtonClick(TObject *Sender);
			void __fastcall FormHide(TObject *Sender);
			void __fastcall FormShow(TObject *Sender);

		private:	// User declarations
			void __fastcall ApplicationShortCut(TWMKey &Msg, bool &Handled);
			AnsiString GetWinDir(void);

			TShortCutEvent OnShortCutAddress;

		public:		// User declarations
			__fastcall TAboutProgramPanel(TComponent* Owner);
	};
	//---------------------------------------------------------------------------
	extern PACKAGE TAboutProgramPanel *AboutProgramPanel;
	//---------------------------------------------------------------------------

#endif		// _About_H_
