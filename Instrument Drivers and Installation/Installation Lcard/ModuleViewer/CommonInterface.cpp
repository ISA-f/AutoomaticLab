#include "CommonInterface.h"

//******************************************************************************
// ���������� ������ ���������� ��� USB ������� �� ��� "� ����"
//******************************************************************************
//-----------------------------------------------------------------------------------
// �����������
//-----------------------------------------------------------------------------------
ILCOMMONINTERFACE::ILCOMMONINTERFACE(void)
{
	// ������������� ���� ��������� �������� ������
	ZeroMemory(&ModuleName, DEVICE_NAME_LENGTH);
	// ������� ��������� �����������   
	pLusbbase = NULL;
	pE140 = NULL;
	pE154 = NULL;
	pE440 = NULL;
	pE310 = NULL;
	pE2010 = NULL;
}

//-----------------------------------------------------------------------------------
// ����������
//-----------------------------------------------------------------------------------
ILCOMMONINTERFACE::~ILCOMMONINTERFACE() { ReleaseLInstance(); }



//-----------------------------------------------------------------------------------
// �������� ����������� ����������� ��������
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::CreateLInstance(void)
{
	// ��� ������ �������� ��������� �����������
	ReleaseLInstance();

	// ��������� �������� ��������� �� ������� ��������� ILUSBBASE
	pLusbbase = static_cast<ILUSBBASE *>(::CreateLInstance("Lusbbase"));
	if(!pLusbbase) { return FALSE; };

	// ��������� �������� ��������� �� ��������� ������ E14-140
	pE140 = static_cast<ILE140 *>(::CreateLInstance("e140"));
	if(!pE140) { return FALSE; };

	// ��������� �������� ��������� �� ��������� ������ E-154
	pE154 = static_cast<ILE154 *>(::CreateLInstance("e154"));
	if(!pE154) { return FALSE; };

	// ��������� �������� ��������� �� ��������� ������ E-310
	pE310 = static_cast<ILE310 *>(::CreateLInstance("e310"));
	if(!pE310) { return FALSE; };

	// ��������� �������� ��������� �� ��������� ������ E14-440
	pE440 = static_cast<ILE440 *>(::CreateLInstance("e440"));
	if(!pE440) { return FALSE; };

	// ��������� �������� ��������� �� ��������� ������ E20-10
	pE2010 = static_cast<ILE2010 *>(::CreateLInstance("e2010"));
	if(!pE2010) { return FALSE; };

	// ��� ������ :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// ��������� ��� ��������� �� ���������� ������
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::ReleaseLInstance(void)
{
	BOOL Status = TRUE;

	if(pLusbbase) 	{ Status &= pLusbbase->ReleaseLInstance(); pLusbbase = NULL; }
	if(pE140) 		{ Status &= pE140->ReleaseLInstance(); pE140 = NULL; }
	if(pE154) 		{ Status &= pE154->ReleaseLInstance(); pE154 = NULL; }
	if(pE310) 		{ Status &= pE310->ReleaseLInstance(); pE310 = NULL; }
	if(pE440) 		{ Status &= pE440->ReleaseLInstance(); pE440 = NULL; }
	if(pE2010) 		{ Status &= pE2010->ReleaseLInstance(); pE2010 = NULL; }
	// ���������� ������ ���������� �������
	return Status;
}

//-----------------------------------------------------------------------------------
// ������� ������� ����������� ����
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::OpenLDevice(WORD VirtualSlot)
{
	// �������� ��������� �� ����������
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;

	// ������� ��� �������� ����������
	if(!CloseLDevice()) { return FALSE; }
	// ������ ������� ������� ����������� ����
	else if(!pLusbbase->OpenLDevice(VirtualSlot)) { return FALSE; }
	// �������� ������ �������� ������
	else if(!pLusbbase->GetModuleName(ModuleName)) { return FALSE; }
	// ������ ������� ����������� ����
	else if(!pLusbbase->CloseLDevice()) { return FALSE; }

	// ������� ������� ����������� ���� � ������� ���������������� ����������
	if(!strcmpi(ModuleName, "E140")) { if(!pE140->OpenLDevice(VirtualSlot)) return FALSE; }
	else if(!strcmpi(ModuleName, "E154")) { if(!pE154->OpenLDevice(VirtualSlot)) return FALSE; }
	else if(!strcmpi(ModuleName, "E-310")) { if(!pE310->OpenLDevice(VirtualSlot)) return FALSE; }
	else if(!strcmpi(ModuleName, "E440")) { if(!pE440->OpenLDevice(VirtualSlot)) return FALSE; }
	else if(!strcmpi(ModuleName, "E20-10")) { if(!pE2010->OpenLDevice(VirtualSlot)) return FALSE; }

	// ��� ������ :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// ��������� ������� ����������� ����
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::CloseLDevice(void)
{
	BOOL Status = TRUE;

	if(pLusbbase)	Status &= pLusbbase->CloseLDevice();
	if(pE140)		Status &= pE140->CloseLDevice();
	if(pE154)		Status &= pE154->CloseLDevice();
	if(pE310)		Status &= pE310->CloseLDevice();
	if(pE440)		Status &= pE440->CloseLDevice();
	if(pE2010)		Status &= pE2010->CloseLDevice();

	// ���������� ������ ���������� �������
	return Status;
}

//-----------------------------------------------------------------------------------
// ��������� ����������� ���������� USB
//-----------------------------------------------------------------------------------
HANDLE WINAPI ILCOMMONINTERFACE::GetModuleHandle(void)
{
	// �������� ��������� �� ����������
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// ������ ����� ����������� �������� ����������� �������� ���������� USB
	if(!strcmpi(ModuleName, "E140")) return pE140->GetModuleHandle();
	else if(!strcmpi(ModuleName, "E154")) return pE154->GetModuleHandle();
	else if(!strcmpi(ModuleName, "E310")) return pE310->GetModuleHandle();
	else if(!strcmpi(ModuleName, "E440")) return pE440->GetModuleHandle();
	else if(!strcmpi(ModuleName, "E20-10")) return pE2010->GetModuleHandle();
	else return INVALID_HANDLE_VALUE;
}

//-----------------------------------------------------------------------------------
// ��������� �������� ������������� ������
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::GetModuleName(PCHAR const ModuleName)
{
	// �������� ��������� �� ����������
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// ������ ����� ����������� �������� �������� �������� ���������� USB
	if(!strcmpi(this->ModuleName, "E140")) { if(!pE140->GetModuleName(ModuleName)) return FALSE; }
	else if(!strcmpi(this->ModuleName, "E154")) { if(!pE154->GetModuleName(ModuleName)) return FALSE; }
	else if(!strcmpi(this->ModuleName, "E-310")) { if(!pE310->GetModuleName(ModuleName)) return FALSE; }
	else if(!strcmpi(this->ModuleName, "E440")) { if(!pE440->GetModuleName(ModuleName)) return FALSE; }
	else if(!strcmpi(this->ModuleName, "E20-10")) { if(!pE2010->GetModuleName(ModuleName)) return FALSE; }

	// ��� ������ :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// ��������� ������� �������� ������ ���� USB
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::GetUsbSpeed(BYTE * const UsbSpeed)
{
	// �������� ��������� �� ����������
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// ������ ����� ����������� �������� �������� �������� ���������� USB
	if(!strcmpi(ModuleName, "E140")) { if(!pE140->GetUsbSpeed(UsbSpeed)) return FALSE; }
	else if(!strcmpi(ModuleName, "E154")) { if(!pE154->GetUsbSpeed(UsbSpeed)) return FALSE; }
	else if(!strcmpi(ModuleName, "E-310")) { if(!pE310->GetUsbSpeed(UsbSpeed)) return FALSE; }
	else if(!strcmpi(ModuleName, "E440")) { if(!pE440->GetUsbSpeed(UsbSpeed)) return FALSE; }
	else if(!strcmpi(ModuleName, "E20-10")) { if(!pE2010->GetUsbSpeed(UsbSpeed)) return FALSE; }

	// ��� ������ :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// ������� ������ ������ � ��������� �������
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo)
{
	// �������� ��������� �� ����������
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// ������ ����� ����������� �������� ������ �������� ���������� USB
	if(!strcmpi(ModuleName, "E140")) { if(!pE140->GetLastErrorInfo(LastErrorInfo)) return FALSE; }
	else if(!strcmpi(ModuleName, "E154")) { if(!pE154->GetLastErrorInfo(LastErrorInfo)) return FALSE; }
	else if(!strcmpi(ModuleName, "E-310")) { if(!pE310->GetLastErrorInfo(LastErrorInfo)) return FALSE; }
	else if(!strcmpi(ModuleName, "E440")) { if(!pE440->GetLastErrorInfo(LastErrorInfo)) return FALSE; }
	else if(!strcmpi(ModuleName, "E20-10")) { if(!pE2010->GetLastErrorInfo(LastErrorInfo)) return FALSE; }

	// ��� ������ :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// ������� �������� ������
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::LOAD_MODULE(PCHAR const FileName)
{
	// �������� ��������� �� ����������
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// ������ ����� ����������� ��������� ������� ���������� USB
	if(!strcmpi(ModuleName, "E140")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E154")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E-310")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E440")) { if(!pE440->LOAD_MODULE(FileName)) return FALSE; }
	else if(!strcmpi(ModuleName, "E20-10")) { if(!pE2010->LOAD_MODULE(FileName)) return FALSE; }

	// ��� ������ :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// �������� �������� ������
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::TEST_MODULE(void)
{
	// �������� ��������� �� ����������
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// ������ ����� ����������� ��������� �������� �������� ���������� USB
	if(!strcmpi(ModuleName, "E140")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E154")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E-310")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E440")) { if(!pE440->TEST_MODULE()) return FALSE; }
	else if(!strcmpi(ModuleName, "E20-10")) { if(!pE2010->TEST_MODULE()) return FALSE; }

	// ��� ������ :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// ������� ��� ������ �� ��������� ����������� �� ����
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::GET_MODULE_DESCRIPTORS(MODULE_DESCRIPTORS * const ModuleDescriptors)
{
	// �������� ��������� �� ����������
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// ������ ����� ����������� �������� ����������� �������� ���������� USB
	if(!strcmpi(ModuleName, "E140"))
	{
		MODULE_DESCRIPTION_E140 ModuleDescription;

		if(!pE140->GET_MODULE_DESCRIPTION(&ModuleDescription)) { return FALSE; }

		// ���������� ������
		ModuleDescriptors->ModuleDescriptor = ModuleDescription.Module;

		// ���������� ����������������
		ModuleDescriptors->McuDescriptor.Active = TRUE;
		strcpy(ModuleDescriptors->McuDescriptor.Name, ModuleDescription.Mcu.Name);
		ModuleDescriptors->McuDescriptor.ClockRate = ModuleDescription.Mcu.ClockRate;
		strcpy(ModuleDescriptors->McuDescriptor.Comment, ModuleDescription.Mcu.Comment);
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Version = ModuleDescription.Mcu.Version;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Active = FALSE;

		// ���������� �������� DSP
		ModuleDescriptors->DspDescriptor.Active = FALSE;
		// ���������� �������� ����
		ModuleDescriptors->PldDescriptor.Active = FALSE;

		// ���������� ���
		ModuleDescriptors->AdcDescriptor = ModuleDescription.Adc;
		// ���������� ���
		ModuleDescriptors->DacDescriptor = ModuleDescription.Dac;
	}
	else if(!strcmpi(ModuleName, "E154"))
	{
		MODULE_DESCRIPTION_E154 ModuleDescription;

		if(!pE154->GET_MODULE_DESCRIPTION(&ModuleDescription)) { return FALSE; }

		// ���������� ������
		ModuleDescriptors->ModuleDescriptor = ModuleDescription.Module;

		// ���������� ����������������
		ModuleDescriptors->McuDescriptor.Active = TRUE;
		strcpy(ModuleDescriptors->McuDescriptor.Name, ModuleDescription.Mcu.Name);
		ModuleDescriptors->McuDescriptor.ClockRate = ModuleDescription.Mcu.ClockRate;
		strcpy(ModuleDescriptors->McuDescriptor.Comment, ModuleDescription.Mcu.Comment);
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Version = ModuleDescription.Mcu.Version;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Active = FALSE;

		// ���������� �������� DSP
		ModuleDescriptors->DspDescriptor.Active = FALSE;
		// ���������� �������� ����
		ModuleDescriptors->PldDescriptor.Active = FALSE;

		// ���������� ���
		ModuleDescriptors->AdcDescriptor = ModuleDescription.Adc;
		// ���������� ���
		ModuleDescriptors->DacDescriptor = ModuleDescription.Dac;
	}
	if(!strcmpi(ModuleName, "E-310"))
	{
		MODULE_DESCRIPTION_E310 ModuleDescription;

		if(!pE310->GET_MODULE_DESCRIPTION(&ModuleDescription)) { return FALSE; }

		// ���������� ������
		ModuleDescriptors->ModuleDescriptor = ModuleDescription.Module;

		// ���������� ����������������
		ModuleDescriptors->McuDescriptor.Active = TRUE;
		strcpy(ModuleDescriptors->McuDescriptor.Name, ModuleDescription.Mcu.Name);
		ModuleDescriptors->McuDescriptor.ClockRate = ModuleDescription.Mcu.ClockRate;
		strcpy(ModuleDescriptors->McuDescriptor.Comment, ModuleDescription.Mcu.Comment);
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Version = ModuleDescription.Mcu.Version.FwVersion;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Version = ModuleDescription.Mcu.Version.BlVersion;

		// ���������� �������� DSP
		ModuleDescriptors->DspDescriptor.Active = FALSE;
		// ���������� �������� ����
		ModuleDescriptors->PldDescriptor.Active = FALSE;

		// ���������� ���
		ModuleDescriptors->AdcDescriptor = ModuleDescription.Adc;
		// ���������� ���
		ModuleDescriptors->DacDescriptor = ModuleDescription.Dac;
	}
	else if(!strcmpi(ModuleName, "E440"))
	{
		MODULE_DESCRIPTION_E440 ModuleDescription;

		if(!pE440->GET_MODULE_DESCRIPTION(&ModuleDescription)) { return FALSE; }

		// ���������� ������
		ModuleDescriptors->ModuleDescriptor = ModuleDescription.Module;

		// ���������� ����������������
		ModuleDescriptors->McuDescriptor.Active = TRUE;
		strcpy(ModuleDescriptors->McuDescriptor.Name, ModuleDescription.Mcu.Name);
		ModuleDescriptors->McuDescriptor.ClockRate = ModuleDescription.Mcu.ClockRate;
		strcpy(ModuleDescriptors->McuDescriptor.Comment, ModuleDescription.Mcu.Comment);
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Version = ModuleDescription.Mcu.Version;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Active = FALSE;

		// ���������� �������� DSP
		ModuleDescriptors->DspDescriptor.Active = TRUE;
		ModuleDescriptors->DspDescriptor.Dsp = ModuleDescription.Dsp;

		// ���������� �������� ����
		ModuleDescriptors->PldDescriptor.Active = FALSE;

		// ���������� ���
		ModuleDescriptors->AdcDescriptor = ModuleDescription.Adc;
		// ���������� ���
		ModuleDescriptors->DacDescriptor = ModuleDescription.Dac;
	}
	else if(!strcmpi(ModuleName, "E20-10"))
	{
		MODULE_DESCRIPTION_E2010 ModuleDescription;

		if(!pE2010->GET_MODULE_DESCRIPTION(&ModuleDescription)) { return FALSE; }

		// ���������� ������
		ModuleDescriptors->ModuleDescriptor = ModuleDescription.Module;

		// ���������� ����������������
		ModuleDescriptors->McuDescriptor.Active = TRUE;
		strcpy(ModuleDescriptors->McuDescriptor.Name, ModuleDescription.Mcu.Name);
		ModuleDescriptors->McuDescriptor.ClockRate = ModuleDescription.Mcu.ClockRate;
		strcpy(ModuleDescriptors->McuDescriptor.Comment, ModuleDescription.Mcu.Comment);
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Version = ModuleDescription.Mcu.Version.FwVersion;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Version = ModuleDescription.Mcu.Version.BlVersion;

		// ���������� �������� DSP
		ModuleDescriptors->DspDescriptor.Active = FALSE;

		// ���������� �������� ����
		ModuleDescriptors->PldDescriptor.Active = TRUE;
		ModuleDescriptors->PldDescriptor.Pld = ModuleDescription.Pld;

		// ���������� ���
		ModuleDescriptors->AdcDescriptor = ModuleDescription.Adc;
		// ���������� ���
		ModuleDescriptors->DacDescriptor = ModuleDescription.Dac;
	}

	// ��� ������ :)))))
	return TRUE;
}

