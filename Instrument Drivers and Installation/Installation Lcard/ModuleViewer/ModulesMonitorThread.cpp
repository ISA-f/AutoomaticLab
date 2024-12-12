//-----------------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ModulesMonitorThread.h"
#include "Waiting.h"
#pragma package(smart_init)
//-----------------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall Unit1::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//-----------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
__fastcall TModulesMonitorThread::TModulesMonitorThread(bool CreateSuspended) : TThread(CreateSuspended)
{
	FreeOnTerminate = false;		// ��������� ����� ����������� ������
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::Execute()
{
	//---- Place thread code here ----
	// �� � ������
	Synchronize(InitThread);
	try
	{
		// ������ ����� ����������� USB �������
		while(!Terminated && !ThreadError)
		{
			// ��������� ����� USB ������
			AccessToModule();
			// ��� - ���� ������?
			if(Terminated || ThreadError) break;
			// �������� ����� �� ��������� ���������� �� ������������ USB �������
			Synchronize(ShowModulesInfo);
			// ����-�
			for(BYTE i = 0x0; i < 0x2; i++) { if(Terminated) break; Sleep(100); }
		}
	}
	catch(...)
	{
		// handler for any C++ exception
		Mes = "�������!!! ������� ������������ ����������!!!\n������ �������� ������������!"; ShowErrorMessageBox();
	}

	// ��������� ������������ �������
	Synchronize(FreeResource);
	// ������ ���� �������� ���������� ������
	while(!Terminated) { Sleep(50); }
}

//-----------------------------------------------------------------------------------
// ������������� �������� ������
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::InitThread(void)
{
	WORD i;

	// ������� ������ ������� ������
	ThreadError = false;
	// �� � ������
	MainForm->IsModulesMonitorThreadRunning = true;

	// ��������� ������ ������� ������
	IsThreadLaunching = TRUE;
	// ������� ������� ��������� ������ ��������
	IsWaitingPanelActivated = false;
	// ����� ������������ ����� ��������� ������
	CurVirtualSlot = EMPTY_VIRTUAL_SLOT;
	// ������� ������� ������������� ���������� ���������� � USB �������
	IsRefreshMustBeDone = FALSE;
	// ������������� ��������� ������� ��������� USB �������
	InitModulesState();
	// ��������� ��������� USB �������
	OldModulesState = ModulesState;
	// ������� ����������� ��� ��������� ������������ ����������� �������� USB �������
	ModuleControlElements(false);
	// ������� ����������� ������ ���������
	MainForm->ModulesListBox->Enabled = false;

	// ����� ��������� �������� ��������� ������������ USB �������
	DllVersion = GetDllVersion();
	if(DllVersion != CURRENT_VERSION_LUSBAPI)
	{
		Mes = "������������ ������ ���������� Lusbapi.dll!\n";
		Mes += "�������: " + IntToStr(DllVersion >> 0x10) + "." + IntToStr(DllVersion & 0xFFFF);
		Mes += " ���������: " + IntToStr(CURRENT_VERSION_LUSBAPI >> 0x10) + "." + IntToStr(CURRENT_VERSION_LUSBAPI & 0xFFFF);
		ShowErrorMessageBox(); return;
	}
	// ��������� �������� ��������� �� ��������� ������������ USB �������
	pModule = new ILCOMMONINTERFACE;
	if(!pModule) { Mes = "�� ���� �������� ����� ��������� USB �������!"; ShowErrorMessageBox(); return; }
	// �������� ������ ���������� USB �������
	else if(!pModule->CreateLInstance()) { Mes = "�� ���� ������� ����� ��������� USB �������!"; ShowErrorMessageBox(); return; }
}

//-----------------------------------------------------------------------------------
// ������� ���������� USB ������
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::AccessToModule(void)
{
	WORD i;
	BOOL Error;
	AnsiString Str;

	// ������� ������ ������� ������ ���������� ������
	ThreadError = false;

	// ������ ����������������� ��������� ������� ��������� USB �������
//	InitModulesState();
	// ������� ���-�� ������������ USB �������
	ModulesState.ModulesQuantity = 0x0;
	// ������� ��������� ���� ��������� ������� � ���������
	CopyMemory(ModulesState.ModuleState, ModuleStatePattern, VIRTUAL_SLOTS_QUANTITY*sizeof(MODULE_STATE));

	// ��������� ���������� ��� USB ������ � ������ VIRTUAL_SLOTS_QUANTITY ����������� ������
	for(i = 0x0, Error = FALSE; i < VIRTUAL_SLOTS_QUANTITY; i++)
	{
		// �-�-�-�-�!!!!!! ��������� ��������!!!
		if(Terminated) break;
		// ��������� ������� ���������������� ����������� ����
		if(!pModule->OpenLDevice(i))
		{
			Str = "���� " + Format("%2u", ARRAYOFCONST((i))) + ": -----";
			strcpy(ModulesState.ModuleState[i].ModuleListBoxString, Str.c_str());
		}
		else
		{
			// �������������� ���-�� ������������ USB �������
			ModulesState.ModulesQuantity++;
			// ����������� ���� USB ������
			ModulesState.ModuleState[i].VirtualSlot = i;
			// ��������� �������� ������ � ������������ ����������� �����
			if(!pModule->GetModuleName(ModulesState.ModuleState[i].ModuleName)) { Error = TRUE; break; }
			// ������ ������� �������� ������ ���� USB20
			else if(!pModule->GetUsbSpeed(&ModulesState.ModuleState[i].UsbSpeed)) { Error = TRUE; break; }
			// ������ ��� ����������?
			else if(OldModulesState.ModuleState[i].IsModuleLoadingMustBeDone)
			{
				// ��� ��� ������ E14-440 ���������� �������� �����������, �� �������
				// ������ ��������, ����� ������������ ����, ��� ��������� ����
				if(!stricmp(ModulesState.ModuleState[i].ModuleName, "E440")) Synchronize(ShowWaitingPanel);
				// �������� ������������� �������� ������
				if(!pModule->TEST_MODULE())
				{
					// ����������� ��� ������ �� ���������������� ������� ������� DLL ����������
					if(!pModule->LOAD_MODULE()) { Error = TRUE; break; }
					// �-�-�-�-�!!!!!! ��������� ��������!!!
					if(Terminated) break;
					// �������� �������� ������
					if(!pModule->TEST_MODULE()) { Error = TRUE; break; }
				}
				// ������� ������� ������������� �������� ������
				ModulesState.ModuleState[i].IsModuleLoadingMustBeDone = FALSE;
				// ������ �������� ������
				ModulesState.ModuleState[i].IsModuleLoaded = TRUE;
				// ������� ����������� USB ������
				if(pModule->GET_MODULE_DESCRIPTORS(&ModulesState.ModuleState[i].ModuleDescriptors))
				{
					// ��������� ������� ����������� ������������ ������
					ModulesState.ModuleState[i].IsModuleDescriptorsEnabled = TRUE;
					// �������������� �������
					Str = "���� " + Format("%2u", ARRAYOFCONST((i))) + ": ������ " + (AnsiString)((char *)ModulesState.ModuleState[i].ModuleDescriptors.ModuleDescriptor.DeviceName);
					strcpy(ModulesState.ModuleState[i].ModuleListBoxString, Str.c_str());
					// �������� ����� ������
					strncpy(ModulesState.ModuleState[i].ModuleSerialNumber, ModulesState.ModuleState[i].ModuleDescriptors.ModuleDescriptor.SerialNumber, sizeof(CurModuleSerialNumber));
					// ������� USB ������
					ModulesState.ModuleState[i].ModuleRevision = ModulesState.ModuleState[i].ModuleDescriptors.ModuleDescriptor.Revision;
					// ������� ������� ���
					ModulesState.ModuleState[i].IsDacPresented = ModulesState.ModuleState[i].ModuleDescriptors.DacDescriptor.Active;
				}
				else
				{
					// ������� ������� ����������� ������������ ������
					ModulesState.ModuleState[i].IsModuleDescriptorsEnabled = FALSE;
					// �������������� �������
					Str = "���� " + Format("%2u", ARRAYOFCONST((i))) + ": ������ " + (AnsiString)((char *)ModulesState.ModuleState[i].ModuleName);
					strcpy(ModulesState.ModuleState[i].ModuleListBoxString, Str.c_str());
					// ������� USB ������
					ModulesState.ModuleState[i].ModuleRevision = 'A';
				}
			}
			else
			{
				// ������� ���, ��� ����������� USB ������� �� ����������
				ModulesState.ModuleState[i] = OldModulesState.ModuleState[i];
			}
		}
	}
	// ���� ����� - ������� ������ ��������
	Synchronize(HideWaitingPanel);
	// �-�-�-�-�!!!!!! ��������� ��������!!!
	if(Terminated) return;

	if(!Error && ((ModuleListBoxItemIndex != MainForm->ModulesListBox->ItemIndex) ||
		(!CompareMem(ModulesState.ModuleState, OldModulesState.ModuleState, VIRTUAL_SLOTS_QUANTITY*sizeof(MODULE_STATE)))))
	{
/*		if(ModulesState.ModulesQuantity >= 0x1)
		{
			// ��������� ���������� ������ ���� � USB �������
			for(i = 0x0; i < VIRTUAL_SLOTS_QUANTITY; i++)
				if(ModulesState.ModuleState[i].VirtualSlot == i) break;
			ModulesState.ActiveModule = ModulesState.ModuleState[i];
		}*/
		// ������� �������� ������
		Synchronize(MarkActiveModule);
		// �������� ������� ���������� � USB �������
		OldModulesState = ModulesState;
	}
}

//-----------------------------------------------------------------------------------
// ������� �������� ������
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::MarkActiveModule(void)
{
	WORD i;

	// ��������� ������ ������� ������
	if(IsThreadLaunching)
	{
		if(ModulesState.ModulesQuantity >= 0x1)
		{
			// ��������� ���������� ������ ���� � USB �������
			for(i = 0x0; i < VIRTUAL_SLOTS_QUANTITY; i++)
				if(ModulesState.ModuleState[i].VirtualSlot == i) break;
			// � ������ ���������� ������ �� ������������ USB �������
			if(i == VIRTUAL_SLOTS_QUANTITY) MainForm->ModulesListBox->ItemIndex = ModuleListBoxItemIndex = -1;
			else MainForm->ModulesListBox->ItemIndex = ModuleListBoxItemIndex = i;
			// ����������� � �������� �������
			ModulesState.ActiveModule = ModulesState.ModuleState[i];
			// ������� ��������� ������ ���������
			MainForm->ModulesListBox->Enabled = true;
			// � ������� ��� ��������
			MainForm->ModulesListBox->SetFocus();
		}
		else { MainForm->ModulesListBox->ItemIndex = ModuleListBoxItemIndex = -1; }
		// ������ ����� �������� ������ ������� ������
		IsThreadLaunching = FALSE;
		// ��������� ������� ������������� ���������� ���������� � USB �������
		IsRefreshMustBeDone = TRUE;
	}
	else
	{
		//
		ModulesState.ActiveModule = ModulesState.ModuleState[MainForm->ModulesListBox->ItemIndex];

		// ������� ����������� ������ ���������
		if(ModulesState.ModulesQuantity >= 0x1)
		{
			if(!MainForm->ModulesListBox->Enabled) MainForm->ModulesListBox->Enabled = true;
			if(!MainForm->ModulesListBox->Focused()) MainForm->ModulesListBox->SetFocus();
		}
		else
		{
			// ������� ����� �������� ������������ �����
			MainForm->VirtualSlotStaticText->Caption = "";
			// ��� ����� ����������
			MainForm->ModulesListBox->ItemIndex = 0x0;
			MainForm->ModulesListBox->ItemIndex = -1;
			// ������� ����������� ������ ���������
			MainForm->ModulesListBox->Enabled = false;
			// ��������� ������ ������������� ���������� ����������� ���������� � USB �������
			IsRefreshMustBeDone = TRUE;
		}

		// ���� ����� - ��������� ������� ������������� ���������� ���������� � USB �������
		if(ModuleListBoxItemIndex != MainForm->ModulesListBox->ItemIndex)
		{
			AnsiString Str;

			ModuleListBoxItemIndex = MainForm->ModulesListBox->ItemIndex;
			if((OldCurVirtualSlot != EMPTY_VIRTUAL_SLOT) || (CurVirtualSlot != EMPTY_VIRTUAL_SLOT))
				IsRefreshMustBeDone = TRUE;
			else
			{
				Str = ModuleListBoxItemIndex;
				MainForm->VirtualSlotStaticText->Caption = (ModuleListBoxItemIndex != (-1)) ? Str.c_str() : "";
			}
		}
		else IsRefreshMustBeDone = TRUE;
	}
}

//-----------------------------------------------------------------------------------
// ��������� ��, ��� �������� ��� USB ������
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::ShowModulesInfo(void)
{
	if(IsRefreshMustBeDone)
	{
		// ������� ��������� ��� ��������� ������������ ����������� �������� USB ������
		ModuleControlElements(true);
		// ������� ������� ������������� ���������� ���������� � USB �������
		IsRefreshMustBeDone = FALSE;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void TModulesMonitorThread::ModuleControlElements(bool EnabledFlag)
{
	WORD i;
	AnsiString Str;

	// ���� � �����������������...
	MainForm->IsReenteringInProgress = true;

	for(i = 0x0; i < VIRTUAL_SLOTS_QUANTITY; i++)
		MainForm->ModulesListBox->Items->Strings[i] = (char *)ModulesState.ModuleState[i].ModuleListBoxString;
	if(!EnabledFlag) MainForm->VirtualSlotStaticText->Caption = "";
	else
	{
		Str = ModuleListBoxItemIndex;
		MainForm->VirtualSlotStaticText->Caption = (ModuleListBoxItemIndex != (-1)) ? Str.c_str() : "";
	}

	// �������� ����������
	if(EnabledFlag)
	{
		if(IsCurModuleLoaded)
		{
			if(CurMcuDescriptors.FirmwareDescriptor.Active) Str = "���������� USB �������: " + (AnsiString)((char *)CurDeviceDescriptors.ModuleDescriptor.DeviceName) + " --> S/N " + (AnsiString)((char *)CurDeviceDescriptors.ModuleDescriptor.SerialNumber);
			else Str = "���������� USB �������: " + (AnsiString)((char *)CurModuleName) + " --> S/N " + (AnsiString)((char *)CurModuleSerialNumber)	;
		}
		else Str = "���������� USB �������: No Module  --> S/N ????????";
	}
	MainForm->Caption = EnabledFlag ? Str.c_str() : "���������� USB �������: No Module  --> S/N ????????";

	//
	ControlElements((TWinControl *)MainForm->ModuleGroupBox, EnabledFlag);

	// �������� ������
	if(EnabledFlag)
	{
		if(CurMcuDescriptors.FirmwareDescriptor.Active) Str = "������ " + (AnsiString)((char *)CurDeviceDescriptors.ModuleDescriptor.DeviceName);
		else Str = "������ " + (AnsiString)((char *)CurModuleName);
	}
	Str = "������ " + (AnsiString)((char *)CurDeviceDescriptors.ModuleDescriptor.DeviceName);
	MainForm->ModuleGroupBox->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "������ ????") : "������ ????";

	// ��������� �������� ������
	MainForm->ModuleLoadingLed->Brush->Color = EnabledFlag ? ((CurVirtualSlot != (-1)) ? (IsCurModuleLoaded ? clLime : clRed): clBtnFace) : clBtnFace;
	// ��������� ������� �������� USB
	MainForm->UsbSpeedLed->Brush->Color = EnabledFlag & (IsCurModuleLoaded) ? (CurUsbSpeed ? clLime : (TColor)RGB(255, 150,0 )) : clBtnFace;
	MainForm->SpeedModeStaticText->Caption = EnabledFlag ? (IsCurModuleLoaded ? (CurUsbSpeed ? "HS" : "FS") : "??") : "??";

	// �������� ����� ������
		CurModuleSerialNumber[0x8] = '\0';
		MainForm->SerialNumberLMDLabel->Enabled = EnabledFlag ? (IsCurModuleDescriptorEnabled ? true : false) : false;
		Str = (char *)CurModuleSerialNumber;
		MainForm->SerialNumberLMDLabel->Caption = EnabledFlag ? (IsCurModuleDescriptorEnabled ? Str.c_str() : "--------") : "--------";

	// ������� ������
		MainForm->ModuleRevisionLMDLabel->Enabled = EnabledFlag ? (IsCurModuleDescriptorEnabled ? true : false) : false;
		Str = (char)CurModuleRevision;
		MainForm->ModuleRevisionLMDLabel->Caption = EnabledFlag ? (IsCurModuleDescriptorEnabled ? Str.c_str() : "-") : "-";

	// ������� ���
		MainForm->DacLMDLabel->Enabled = EnabledFlag ? (IsCurModuleDescriptorEnabled ? true : false) : false;
		MainForm->DacLMDLabel->Caption = EnabledFlag ? (IsCurModuleDescriptorEnabled ? (CurIsDacPresented ? "����" : "���") : "----") : "----";

	// ���������� � �������� MCU
	MainForm->McuLMDGroupBox->Enabled = IsCurFirmwareDescriptorEnabled ? true : false;
	if(IsCurFirmwareDescriptorEnabled)
	{
		MainForm->McuTypeLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->McuTypeLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->McuTypeLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? (char *)CurMcuDescriptors.Name : "------------") : "------------";
		MainForm->McuFreqLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->McuFreqLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		Str = FloatToStr(CurMcuDescriptors.ClockRate) + " ���";
		MainForm->McuFreqLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "------------") : "------------";
		MainForm->McuFirmwareVersionLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->McuFirmwareVersionLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		Str = (AnsiString)((char *)CurMcuDescriptors.FirmwareDescriptor.Version.Version) + " �� " + (AnsiString)((char *)CurMcuDescriptors.FirmwareDescriptor.Version.Date);
		MainForm->McuFirmwareVersionLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "------------") : "------------";
	}
	else
	{
		MainForm->McuTypeLabelLMDLabel->Enabled =  false;
		MainForm->McuTypeLMDLabel->Enabled = false;
		MainForm->McuTypeLMDLabel->Caption = "------------";
		MainForm->McuFreqLabelLMDLabel->Enabled = false;
		MainForm->McuFreqLMDLabel->Enabled = false;
		MainForm->McuFreqLMDLabel->Caption = "------------";
		MainForm->McuFirmwareVersionLabelLMDLabel->Enabled = false;
		MainForm->McuFirmwareVersionLMDLabel->Enabled = false;
		MainForm->McuFirmwareVersionLMDLabel->Caption = "------------";
	}

	// ���������� � ���������� MCU
	MainForm->BootLoaderLMDGroupBox->Enabled = IsCurBootLoaderDescriptorEnabled ? true : false;
	if(IsCurBootLoaderDescriptorEnabled)
	{
		MainForm->McuBootLoadeVersionLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? (IsCurBootLoaderDescriptorEnabled ? true : false) : false) : false;
		MainForm->McuBootLoaderVersionLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? (IsCurBootLoaderDescriptorEnabled ? true : false) : false) : false;
		Str = (AnsiString)((char *)CurMcuDescriptors.BootLoaderDescriptor.Version.Version) + " �� " + (AnsiString)((char *)CurMcuDescriptors.BootLoaderDescriptor.Version.Date);
		MainForm->McuBootLoaderVersionLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? (IsCurBootLoaderDescriptorEnabled ? Str.c_str() : "????????????") : "????????????") : "????????????";
	}
	else
	{
		MainForm->McuBootLoadeVersionLabelLMDLabel->Enabled = false;
		MainForm->McuBootLoaderVersionLMDLabel->Enabled = false;
		MainForm->McuBootLoaderVersionLMDLabel->Caption = "------------";
	}

	// ���������� � DSP
	MainForm->DspLMDGroupBox->Enabled = IsCurDspDescriptorEnabled ? true : false;
	if(IsCurDspDescriptorEnabled)
	{
		MainForm->DspTypeLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		MainForm->DspTypeLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->DspTypeLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? (char *)CurDspDescriptors.Dsp.Name : "????????????") : "????????????";
		MainForm->DspFreqLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->DspFreqLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		Str = FloatToStr(CurDspDescriptors.Dsp.ClockRate) + " ���";
		MainForm->DspFreqLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "????????????") : "????????????";
		MainForm->DspFirmwareVersionLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		MainForm->DspFirmwareVersionLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		Str = (AnsiString)((char *)CurDspDescriptors.Dsp.Version.Version) + " �� " + (AnsiString)((char *)CurDspDescriptors.Dsp.Version.Date);
		MainForm->DspFirmwareVersionLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "????????????") : "????????????";
	}
	else
	{
		MainForm->DspTypeLabelLMDLabel->Enabled = false;;
		MainForm->DspTypeLMDLabel->Enabled = false;
		MainForm->DspTypeLMDLabel->Caption = "------------";
		MainForm->DspFreqLabelLMDLabel->Enabled = false;
		MainForm->DspFreqLMDLabel->Enabled = false;
		MainForm->DspFreqLMDLabel->Caption = "------------";
		MainForm->DspFirmwareVersionLabelLMDLabel->Enabled = false;;
		MainForm->DspFirmwareVersionLMDLabel->Enabled = false;;
		MainForm->DspFirmwareVersionLMDLabel->Caption = "------------";
	}

	// ���������� � ����
	MainForm->PldLMDGroupBox->Enabled = IsCurPldDescriptorEnabled ? true : false;
	if(IsCurPldDescriptorEnabled)
	{
		MainForm->PldTypeLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		MainForm->PldTypeLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->PldTypeLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? (char *)CurPldDescriptors.Pld.Name : "????????????") : "????????????";
		MainForm->PldFreqLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->PldFreqLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		Str = FloatToStr(CurPldDescriptors.Pld.ClockRate) + " ���";
		MainForm->PldFreqLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "????????????") : "????????????";
		MainForm->PldFirmwareVersionLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		MainForm->PldFirmwareVersionLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		Str = (AnsiString)((char *)CurPldDescriptors.Pld.Version.Version) + " �� " + (AnsiString)((char *)CurPldDescriptors.Pld.Version.Date);
		MainForm->PldFirmwareVersionLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "????????????") : "????????????";
	}
	else
	{
		MainForm->PldTypeLabelLMDLabel->Enabled				= false;;
		MainForm->PldTypeLMDLabel->Enabled						= false;
		MainForm->PldTypeLMDLabel->Caption						= "------------";
		MainForm->PldFreqLabelLMDLabel->Enabled				= false;
		MainForm->PldFreqLMDLabel->Enabled						= false;
		MainForm->PldFreqLMDLabel->Caption						= "------------";
		MainForm->PldFirmwareVersionLabelLMDLabel->Enabled	= false;;
		MainForm->PldFirmwareVersionLMDLabel->Enabled		= false;;
		MainForm->PldFirmwareVersionLMDLabel->Caption		= "------------";
	}

	// ����� �����������������...
	MainForm->IsReenteringInProgress = false;
}

//-----------------------------------------------------------------------------------
// ������� ������ ��������
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::ShowWaitingPanel(void)
{
	// ���� ����� - ���������� ������ ��������
	if(!IsWaitingPanelActivated) WaitingPanel->Show();
	// ��������� ������� ��������� ������ ��������
	IsWaitingPanelActivated = true;
}

//-----------------------------------------------------------------------------------
// ������� ������ ��������
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::HideWaitingPanel(void)
{
	// ���� ����� - ������������ ������ ��������
	if(IsWaitingPanelActivated) WaitingPanel->Hide();
	// ������� ������� ��������� ������ ��������
	IsWaitingPanelActivated = false;
}



//-----------------------------------------------------------------------------------
// ������������� ��������� ������� ��������� USB �������
//-----------------------------------------------------------------------------------
void TModulesMonitorThread::InitModulesState(void)
{
	WORD i;
	MODULE_STATE LocModuleState;

	// ������� ���-�� ������������ USB �������
	ModulesState.ModulesQuantity = 0x0;
	// ====== �������� USB ������ ===============================
	// ����������� ���� ��������� USB ������
	LocModuleState.VirtualSlot = EMPTY_VIRTUAL_SLOT;
	// ��������� ������� ������������� �������� ������
	LocModuleState.IsModuleLoadingMustBeDone = TRUE;
	// ������� ������������ �������� ������
	LocModuleState.IsModuleLoadingMustBeDone = TRUE;
	// ������ �������� ��������� USB ������
	LocModuleState.IsModuleLoaded = FALSE;
	// ������� ����������� ������������ �� �������� USB ������
	LocModuleState.IsModuleDescriptorsEnabled = FALSE;
	// ������� ������� ���
	LocModuleState.IsDacPresented = FALSE;
	// ��������� � ������������� ��������� USB ������
	ZeroMemory(&LocModuleState.ModuleDescriptors, sizeof(ModulesState.ActiveModule.ModuleDescriptors));
	// ������� � ����� ��������� USB ������
	ZeroMemory(LocModuleState.ModuleListBoxString, sizeof(ModulesState.ActiveModule.ModuleListBoxString));
	// �������� ��������� USB ������
	ZeroMemory(LocModuleState.ModuleName, sizeof(ModulesState.ActiveModule.ModuleName));
	// �������� ����� ������
	ZeroMemory(LocModuleState.ModuleSerialNumber,sizeof(ModulesState.ActiveModule.ModuleSerialNumber));
	// ������� ��������� USB ������
	LocModuleState.ModuleRevision = '\0';
	// �������� ������ ���� USB ��� ��������� USB ������
	LocModuleState.UsbSpeed = INVALID_USB_SPEED_LUSBAPI;
	// =========================================================
	// ������ �������������� ������ �������� USB �������
	for(i = 0x0; i < VIRTUAL_SLOTS_QUANTITY; i++)
		ModulesState.ModuleState[i] = ModuleStatePattern[i] = LocModuleState;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void TModulesMonitorThread::ControlElements(TWinControl *WinControlElement, bool EnabledFlag)
{
	DWORD i;

	for(i = 0x0; i < (DWORD)WinControlElement->ControlCount; i++)
	{
		TWinControl *WinControl;
		WinControl = dynamic_cast<TWinControl *>(WinControlElement->Controls[i]);
		if(WinControl) ControlElements(dynamic_cast<TWinControl *>(WinControlElement->Controls[i]), EnabledFlag);

		TControl *Control;
		Control = dynamic_cast<TControl *>(WinControlElement->Controls[i]);
		if(Control) { Control->Enabled = EnabledFlag;  }
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::FreeResource(void)
{
	// ���� ����� - ������� ������ ��������
	if(!Application->Terminated)
		{ if(WaitingPanel->Visible) WaitingPanel->Hide(); }
	// ��������� ��������� ��������� �� ������
	if(pModule) { delete pModule; pModule = NULL; }
}

//-----------------------------------------------------------------------------------
// ����������� ��������������� ���������
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::ShowInfoMessageBox(void)
{
	Application->MessageBox(Mes.c_str(),"��������� TModulesMonitorThread::Execute()!", MB_OK + MB_ICONINFORMATION);
}

//-----------------------------------------------------------------------------------
// ����������� ��������� � �������
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::ShowErrorMessageBox(void)
{
	if(ThreadError) return;
	Application->MessageBox(Mes.c_str(),"��������� TModulesMonitorThread::Execute()!", MB_OK + MB_ICONINFORMATION);
	ThreadError = true;
}
//-----------------------------------------------------------------------------------

