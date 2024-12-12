//---------------------------------------------------------------------------
#ifndef _ModulesMonitorThread_H_
#define _ModulesMonitorThread_H_
//---------------------------------------------------------------------------
	#include <Classes.hpp>
	#include "MainForm.h"
	#include "CommonInterface.h"

	// параметры обнаружения USB модулей
	#define	VIRTUAL_SLOTS_QUANTITY					(0x19) 	// кол-во опрашиваемых виртуальных слотов для USB устройств
	#define	EMPTY_VIRTUAL_SLOT 						(-1)	 	// пустой виртуальный слот

	// замена для краткости
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
	// состояния всех обнаруженных USB модулей
	struct MODULES_STATE
	{
		// кол-во обнаруженных USB модулей
		WORD ModulesQuantity;
		// полное состояние активного USB модуля
		MODULE_STATE ActiveModule;
		// полное состояние всех обнаруженных USB модулей
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

			// управляющие элементы панели USB модулей
			void ModuleControlElements(bool EnabledFlag);
			void ControlElements(TWinControl *WinControlElement, bool EnabledFlag);
			void __fastcall FreeResource(void);
			void __fastcall ShowInfoMessageBox(void);
			void __fastcall ShowErrorMessageBox(void);

		protected:
			void __fastcall Execute();

		public:
			__fastcall TModulesMonitorThread(bool CreateSuspended);

			// признак ошибки выполнения потока
			BOOL ThreadError;
			// флажок запуска потока
			BOOL IsThreadLaunching;
			// признак необходимости обновления информации о USB модулях
			BOOL IsRefreshMustBeDone;
			// признак активации панели ожидания
			BOOL IsWaitingPanelActivated;
			// текущий номер в списке USB модулей
			int ModuleListBoxItemIndex;
			// версия библиотеки
			DWORD DllVersion;
			// указатель на общий интерфейс USB устройств
			ILCOMMONINTERFACE *pModule;
			// текущее и предыдущее состояния USB модулей
			MODULES_STATE ModulesState, OldModulesState;
			// начальные состояния всех вожможных VIRTUAL_SLOTS_QUANTITY USB модулей
			MODULE_STATE ModuleStatePattern[VIRTUAL_SLOTS_QUANTITY];

			// строчка с ошибкой выполнения потока
			AnsiString Mes;
	};
	//---------------------------------------------------------------------------

#endif		// _ModulesMonitorThread_H_
