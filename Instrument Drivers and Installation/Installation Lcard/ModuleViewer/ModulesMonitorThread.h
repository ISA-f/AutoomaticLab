//---------------------------------------------------------------------------
#ifndef _ModulesMonitorThread_H_
#define _ModulesMonitorThread_H_
//---------------------------------------------------------------------------
	#include <Classes.hpp>
	#include "MainForm.h"
	#include "CommonInterface.h"

	// ��������� ����������� USB �������
	#define	VIRTUAL_SLOTS_QUANTITY					(0x19) 	// ���-�� ������������ ����������� ������ ��� USB ���������
	#define	EMPTY_VIRTUAL_SLOT 						(-1)	 	// ������ ����������� ����

	// ������ ��� ���������
	#define	CurVirtualSlot								ModulesState.ActiveModule.VirtualSlot
	#define	OldCurVirtualSlot							OldModulesState.ActiveModule.VirtualSlot
	#define	IsCurModuleLoaded							ModulesState.ActiveModule.IsModuleLoaded
	#define	IsCurModuleDescriptorEnabled			ModulesState.ActiveModule.IsModuleLoaded
	#define	IsCurFirmwareDescriptorEnabled		ModulesState.ActiveModule.ModuleDescriptors.McuDescriptor.FirmwareDescriptor.Active
	#define	IsCurBootLoaderDescriptorEnabled		ModulesState.ActiveModule.ModuleDescriptors.McuDescriptor.BootLoaderDescriptor.Active
	#define	IsCurDspDescriptorEnabled				ModulesState.ActiveModule.ModuleDescriptors.DspDescriptor.Active
	#define	IsCurPldDescriptorEnabled				ModulesState.ActiveModule.ModuleDescriptors.PldDescriptor.Active
	#define	CurDeviceDescriptors						ModulesState.ActiveModule.ModuleDescriptors
	#define	CurMcuDescriptors							ModulesState.ActiveModule.ModuleDescriptors.McuDescriptor
	#define	CurDspDescriptors							ModulesState.ActiveModule.ModuleDescriptors.DspDescriptor
	#define	CurPldDescriptors							ModulesState.ActiveModule.ModuleDescriptors.PldDescriptor
	#define	CurModuleListBoxStrings					ModulesState.ActiveModule.ModuleListBoxStrings
	#define	CurModuleName								ModulesState.ActiveModule.ModuleName
	#define	CurModuleSerialNumber 					ModulesState.ActiveModule.ModuleSerialNumber
	#define	CurModuleRevision							ModulesState.ActiveModule.ModuleRevision
	#define	CurUsbSpeed									ModulesState.ActiveModule.UsbSpeed
	#define	CurIsDacPresented							ModulesState.ActiveModule.IsDacPresented

	#pragma pack(1)
	// ��������� ���� ������������ USB �������
	struct MODULES_STATE
	{
		// ���-�� ������������ USB �������
		WORD ModulesQuantity;
		// ������ ��������� ��������� USB ������
		MODULE_STATE ActiveModule;
		// ������ ��������� ���� ������������ USB �������
		MODULE_STATE ModuleState[VIRTUAL_SLOTS_QUANTITY];
	};
	#pragma pack()


	//---------------------------------------------------------------------------
	//
	//---------------------------------------------------------------------------
	class TModulesMonitorThread : public TThread
	{
		private:
			void __fastcall InitThread(void);

			void InitModulesState(void);
			void __fastcall AccessToModule(void);
			void __fastcall MarkActiveModule(void);
			void __fastcall ShowModulesInfo(void);
			void __fastcall ShowWaitingPanel(void);
			void __fastcall HideWaitingPanel(void);

			// ����������� �������� ������ USB �������
			void ModuleControlElements(bool EnabledFlag);
			void ControlElements(TWinControl *WinControlElement, bool EnabledFlag);
			void __fastcall FreeResource(void);
			void __fastcall ShowInfoMessageBox(void);
			void __fastcall ShowErrorMessageBox(void);

		protected:
			void __fastcall Execute();

		public:
			__fastcall TModulesMonitorThread(bool CreateSuspended);

			// ������� ������ ���������� ������
			BOOL ThreadError;
			// ������ ������� ������
			BOOL IsThreadLaunching;
			// ������� ������������� ���������� ���������� � USB �������
			BOOL IsRefreshMustBeDone;
			// ������� ��������� ������ ��������
			BOOL IsWaitingPanelActivated;
			// ������� ����� � ������ USB �������
			int ModuleListBoxItemIndex;
			// ������ ����������
			DWORD DllVersion;
			// ��������� �� ����� ��������� USB ���������
			ILCOMMONINTERFACE *pModule;
			// ������� � ���������� ��������� USB �������
			MODULES_STATE ModulesState, OldModulesState;
			// ��������� ��������� ���� ��������� VIRTUAL_SLOTS_QUANTITY USB �������
			MODULE_STATE ModuleStatePattern[VIRTUAL_SLOTS_QUANTITY];

			// ������� � ������� ���������� ������
			AnsiString Mes;
	};
	//---------------------------------------------------------------------------

#endif		// _ModulesMonitorThread_H_
