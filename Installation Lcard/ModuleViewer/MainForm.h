//---------------------------------------------------------------------------
#ifndef _MainForm_H_
#define _MainForm_H_
//---------------------------------------------------------------------------
	#include <Classes.hpp>
	#include <Controls.hpp>
	#include <StdCtrls.hpp>
	#include <Forms.hpp>
	#include <ExtCtrls.hpp>
	#include <Menus.hpp>
	#include "LMDButton.hpp"
	#include "LMDComboBox.hpp"
	#include "LMDGroupBox.hpp"
	#include "CommonInterface.h"
	#include "LMDCustomBevelPanel.hpp"
	#include "LMDCustomControl.hpp"
	#include "LMDCustomGroupBox.hpp"
	#include "LMDCustomPanel.hpp"
	#include "LMDCustomPanelFill.hpp"
	#include "LMDCustomParentPanel.hpp"
	#include "JustOne.hpp"
#include "LMDBaseControl.hpp"
#include "LMDBaseGraphicControl.hpp"
#include "LMDBaseLabel.hpp"
#include "LMDControl.hpp"
#include "LMDCustomLabel.hpp"
#include "LMDLabel.hpp"

	#define	WM_MODULES_MONITOR_THREAD			(WM_USER + 100)

	// ������-�� ���� ��������� ��� � Builder, � ��� � Delphi �������
	#define	ID_ANI_CAPTION							(0x3)

	#pragma pack(1)

	// ��������� ���������� USB ������
	struct MODULE_STATE
	{
		// ������� ����������� ���� USB ������
		int VirtualSlot;
		// ������� ������������� �������� ������
		BOOL IsModuleLoadingMustBeDone;
		// ������ �������� ������
		BOOL IsModuleLoaded;
		// ������� ����������� ������������ ������
		BOOL IsModuleDescriptorsEnabled;
		// ��������� � ������������� ������
		MODULE_DESCRIPTORS ModuleDescriptors;
		// ������� � ����� USB ������
		BYTE ModuleListBoxString[77];
		// �������� ������ ���� USB ��� USB ������
		BYTE UsbSpeed;
		// �������� USB ������
		BYTE ModuleName[16], ModuleSerialNumber[16];
		// ������� USB ������
		BYTE ModuleRevision;
		// ������� ������� ���
		BOOL IsDacPresented;
	};
	#pragma pack()

	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	class TMainForm : public TForm
	{
		__published:	// IDE-managed Components
			TXJustOne *XJustOne;
			TPopupMenu *PopupMenu;
			TMenuItem *AboutProgram;
			TMenuItem *BreakLine;
			TMenuItem *ApplicationExit;
	TStaticText *ModulesListStaticText;
			TStaticText *VirtualSlotStaticText;
			TGroupBox *ModuleGroupBox;
			TShape *ModuleLoadingLed;
			TShape *UsbSpeedLed;
			TLMDGroupBox *McuLMDGroupBox;
			TLMDLabel *McuTypeLabelLMDLabel;
			TLMDLabel *McuFreqLabelLMDLabel;
			TLMDLabel *McuFirmwareVersionLabelLMDLabel;
			TLMDLabel *McuTypeLMDLabel;
			TLMDLabel *McuFreqLMDLabel;
			TLMDLabel *McuFirmwareVersionLMDLabel;
			TLMDGroupBox *DspLMDGroupBox;
			TLMDLabel *DspTypeLabelLMDLabel;
			TLMDLabel *DspFreqLabelLMDLabel;
			TLMDLabel *DspFirmwareVersionLabelLMDLabel;
			TLMDLabel *DspTypeLMDLabel;
			TLMDLabel *DspFreqLMDLabel;
			TLMDLabel *DspFirmwareVersionLMDLabel;
			TLMDLabel *ModuleStatusLMDLabel;
			TLMDLabel *RevisionLabelLMDLabel;
			TLMDLabel *SerialNumberLabelLMDLabel;
			TStaticText *SpeedModeStaticText;
			TLMDGroupBox *BootLoaderLMDGroupBox;
			TLMDLabel *McuBootLoadeVersionLabelLMDLabel;
			TLMDLabel *McuBootLoaderVersionLMDLabel;
			TLMDLabel *SerialNumberLMDLabel;
			TLMDLabel *ModuleRevisionLMDLabel;
			TListBox *ModulesListBox;
			TLMDGroupBox *PldLMDGroupBox;
			TLMDLabel *PldTypeLabelLMDLabel;
			TLMDLabel *PldFreqLabelLMDLabel;
			TLMDLabel *PldFirmwareVersionLabelLMDLabel;
			TLMDLabel *PldTypeLMDLabel;
			TLMDLabel *PldFreqLMDLabel;
			TLMDLabel *PldFirmwareVersionLMDLabel;
			TLMDLabel *DacLabelLMDLabel;
			TLMDLabel *DacLMDLabel;

			void __fastcall FormCreate(TObject *Sender);

			void __fastcall XJustOneAlreadyExist(TObject *Sender);
			void __fastcall FormShow(TObject *Sender);
			void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
			void __fastcall FormDestroy(TObject *Sender);
			void __fastcall ApplicationExitClick(TObject *Sender);
			void __fastcall AboutProgramClick(TObject *Sender);

		private:	// User declarations
			void __fastcall ApplicationShortCut(TWMKey &Msg, bool &Handled);

			// ������ � ������� ����������� USB �������
			void __fastcall StartModulesMonitorThread(void);
			void __fastcall OnStartModulesMonitorThread(TMessage& Message);
			void __fastcall ModulesMonitorThreadDone(TObject * /*Sender*/);
			void __fastcall StopThread(void);

		public:		// User declarations
			__fastcall TMainForm(TComponent* Owner);

			// ������ �����������������
			bool IsReenteringInProgress;
			// ������ ��� ������ � ������� ����������� USB �������
			bool IsModulesMonitorThreadRunning, IsModulesMonitorThreadDone;

			BEGIN_MESSAGE_MAP
				VCL_MESSAGE_HANDLER(WM_MODULES_MONITOR_THREAD, TMessage, OnStartModulesMonitorThread)
			END_MESSAGE_MAP(TForm)
	};

	//---------------------------------------------------------------------------
	extern PACKAGE TMainForm *MainForm;
	//---------------------------------------------------------------------------
#endif		// _MainForm_H_
