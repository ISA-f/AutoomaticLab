#include "CommonInterface.h"

//******************************************************************************
// Реализация общего интерфейса для USB модулей от ООО "Л Кард"
//******************************************************************************
//-----------------------------------------------------------------------------------
// конструктор
//-----------------------------------------------------------------------------------
ILCOMMONINTERFACE::ILCOMMONINTERFACE(void)
{
	// инициализация всех локальных структур класса
	ZeroMemory(&ModuleName, DEVICE_NAME_LENGTH);
	// сбросим указатели интерфейсов   
	pLusbbase = NULL;
	pE140 = NULL;
	pE154 = NULL;
	pE440 = NULL;
	pE310 = NULL;
	pE2010 = NULL;
}

//-----------------------------------------------------------------------------------
// деструктор
//-----------------------------------------------------------------------------------
ILCOMMONINTERFACE::~ILCOMMONINTERFACE() { ReleaseLInstance(); }



//-----------------------------------------------------------------------------------
// Создание экземпляров инткрфейсов модуллей
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::CreateLInstance(void)
{
	// для начала зачистим указатели интерфейсов
	ReleaseLInstance();

	// попробуем получить указатель на базовый интерфейс ILUSBBASE
	pLusbbase = static_cast<ILUSBBASE *>(::CreateLInstance("Lusbbase"));
	if(!pLusbbase) { return FALSE; };

	// попробуем получить указатель на интерфейс модуля E14-140
	pE140 = static_cast<ILE140 *>(::CreateLInstance("e140"));
	if(!pE140) { return FALSE; };

	// попробуем получить указатель на интерфейс модуля E-154
	pE154 = static_cast<ILE154 *>(::CreateLInstance("e154"));
	if(!pE154) { return FALSE; };

	// попробуем получить указатель на интерфейс модуля E-310
	pE310 = static_cast<ILE310 *>(::CreateLInstance("e310"));
	if(!pE310) { return FALSE; };

	// попробуем получить указатель на интерфейс модуля E14-440
	pE440 = static_cast<ILE440 *>(::CreateLInstance("e440"));
	if(!pE440) { return FALSE; };

	// попробуем получить указатель на интерфейс модуля E20-10
	pE2010 = static_cast<ILE2010 *>(::CreateLInstance("e2010"));
	if(!pE2010) { return FALSE; };

	// все хорошо :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// Освободим все указатели на интерфейсы модуля
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
	// возвращаем статус выполнения функции
	return Status;
}

//-----------------------------------------------------------------------------------
// Пробуем открыть виртуальный слот
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::OpenLDevice(WORD VirtualSlot)
{
	// проверим указатели на интерфейсы
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;

	// закроем все открытые устройства
	if(!CloseLDevice()) { return FALSE; }
	// теперь пробуем открыть виртуальный слот
	else if(!pLusbbase->OpenLDevice(VirtualSlot)) { return FALSE; }
	// пытаемся узнать название модуля
	else if(!pLusbbase->GetModuleName(ModuleName)) { return FALSE; }
	// теперь закроем виртуальный слот
	else if(!pLusbbase->CloseLDevice()) { return FALSE; }

	// пробуем открыть виртуальный слот с помощью соответствующего интерфейса
	if(!strcmpi(ModuleName, "E140")) { if(!pE140->OpenLDevice(VirtualSlot)) return FALSE; }
	else if(!strcmpi(ModuleName, "E154")) { if(!pE154->OpenLDevice(VirtualSlot)) return FALSE; }
	else if(!strcmpi(ModuleName, "E-310")) { if(!pE310->OpenLDevice(VirtualSlot)) return FALSE; }
	else if(!strcmpi(ModuleName, "E440")) { if(!pE440->OpenLDevice(VirtualSlot)) return FALSE; }
	else if(!strcmpi(ModuleName, "E20-10")) { if(!pE2010->OpenLDevice(VirtualSlot)) return FALSE; }

	// все хорошо :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// Освободим текущий виртуальный слот
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

	// возвращаем статус выполнения функции
	return Status;
}

//-----------------------------------------------------------------------------------
// Получение дескриптора устройства USB
//-----------------------------------------------------------------------------------
HANDLE WINAPI ILCOMMONINTERFACE::GetModuleHandle(void)
{
	// проверим указатели на интерфейсы
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// теперь можно попробовать получить дескриптора текущего устройства USB
	if(!strcmpi(ModuleName, "E140")) return pE140->GetModuleHandle();
	else if(!strcmpi(ModuleName, "E154")) return pE154->GetModuleHandle();
	else if(!strcmpi(ModuleName, "E310")) return pE310->GetModuleHandle();
	else if(!strcmpi(ModuleName, "E440")) return pE440->GetModuleHandle();
	else if(!strcmpi(ModuleName, "E20-10")) return pE2010->GetModuleHandle();
	else return INVALID_HANDLE_VALUE;
}

//-----------------------------------------------------------------------------------
// получение названия используемого модуля
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::GetModuleName(PCHAR const ModuleName)
{
	// проверим указатели на интерфейсы
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// теперь можно попробовать получить название текущего устройства USB
	if(!strcmpi(this->ModuleName, "E140")) { if(!pE140->GetModuleName(ModuleName)) return FALSE; }
	else if(!strcmpi(this->ModuleName, "E154")) { if(!pE154->GetModuleName(ModuleName)) return FALSE; }
	else if(!strcmpi(this->ModuleName, "E-310")) { if(!pE310->GetModuleName(ModuleName)) return FALSE; }
	else if(!strcmpi(this->ModuleName, "E440")) { if(!pE440->GetModuleName(ModuleName)) return FALSE; }
	else if(!strcmpi(this->ModuleName, "E20-10")) { if(!pE2010->GetModuleName(ModuleName)) return FALSE; }

	// все хорошо :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// получение текущей скорости работы шины USB
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::GetUsbSpeed(BYTE * const UsbSpeed)
{
	// проверим указатели на интерфейсы
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// теперь можно попробовать получить скорость текущего устройства USB
	if(!strcmpi(ModuleName, "E140")) { if(!pE140->GetUsbSpeed(UsbSpeed)) return FALSE; }
	else if(!strcmpi(ModuleName, "E154")) { if(!pE154->GetUsbSpeed(UsbSpeed)) return FALSE; }
	else if(!strcmpi(ModuleName, "E-310")) { if(!pE310->GetUsbSpeed(UsbSpeed)) return FALSE; }
	else if(!strcmpi(ModuleName, "E440")) { if(!pE440->GetUsbSpeed(UsbSpeed)) return FALSE; }
	else if(!strcmpi(ModuleName, "E20-10")) { if(!pE2010->GetUsbSpeed(UsbSpeed)) return FALSE; }

	// все хорошо :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// функция выдачи строки с последней ошибкой
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo)
{
	// проверим указатели на интерфейсы
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// теперь можно попробовать получить ошибки текущего устройства USB
	if(!strcmpi(ModuleName, "E140")) { if(!pE140->GetLastErrorInfo(LastErrorInfo)) return FALSE; }
	else if(!strcmpi(ModuleName, "E154")) { if(!pE154->GetLastErrorInfo(LastErrorInfo)) return FALSE; }
	else if(!strcmpi(ModuleName, "E-310")) { if(!pE310->GetLastErrorInfo(LastErrorInfo)) return FALSE; }
	else if(!strcmpi(ModuleName, "E440")) { if(!pE440->GetLastErrorInfo(LastErrorInfo)) return FALSE; }
	else if(!strcmpi(ModuleName, "E20-10")) { if(!pE2010->GetLastErrorInfo(LastErrorInfo)) return FALSE; }

	// все хорошо :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// функции загрузки модуля
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::LOAD_MODULE(PCHAR const FileName)
{
	// проверим указатели на интерфейсы
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// теперь можно попробовать загрузить текущее устройство USB
	if(!strcmpi(ModuleName, "E140")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E154")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E-310")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E440")) { if(!pE440->LOAD_MODULE(FileName)) return FALSE; }
	else if(!strcmpi(ModuleName, "E20-10")) { if(!pE2010->LOAD_MODULE(FileName)) return FALSE; }

	// все хорошо :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// проверка загрузки модуля
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::TEST_MODULE(void)
{
	// проверим указатели на интерфейсы
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// теперь можно попробовать проверить загрузку текущего устройства USB
	if(!strcmpi(ModuleName, "E140")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E154")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E-310")) { return TRUE; }
	else if(!strcmpi(ModuleName, "E440")) { if(!pE440->TEST_MODULE()) return FALSE; }
	else if(!strcmpi(ModuleName, "E20-10")) { if(!pE2010->TEST_MODULE()) return FALSE; }

	// все хорошо :)))))
	return TRUE;
}

//-----------------------------------------------------------------------------------
// функции для работы со служебной информацией из ППЗУ
//-----------------------------------------------------------------------------------
BOOL WINAPI ILCOMMONINTERFACE::GET_MODULE_DESCRIPTORS(MODULE_DESCRIPTORS * const ModuleDescriptors)
{
	// проверим указатели на интерфейсы
	if(!pLusbbase || !pE140 || !pE154 || !pE310 || !pE440 || !pE2010) return FALSE;
	// теперь можно попробовать получить дескрипторы текущего устройства USB
	if(!strcmpi(ModuleName, "E140"))
	{
		MODULE_DESCRIPTION_E140 ModuleDescription;

		if(!pE140->GET_MODULE_DESCRIPTION(&ModuleDescription)) { return FALSE; }

		// дескриптор модуля
		ModuleDescriptors->ModuleDescriptor = ModuleDescription.Module;

		// дескриптор микроконтроллера
		ModuleDescriptors->McuDescriptor.Active = TRUE;
		strcpy(ModuleDescriptors->McuDescriptor.Name, ModuleDescription.Mcu.Name);
		ModuleDescriptors->McuDescriptor.ClockRate = ModuleDescription.Mcu.ClockRate;
		strcpy(ModuleDescriptors->McuDescriptor.Comment, ModuleDescription.Mcu.Comment);
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Version = ModuleDescription.Mcu.Version;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Active = FALSE;

		// дескриптор драйвера DSP
		ModuleDescriptors->DspDescriptor.Active = FALSE;
		// дескриптор драйвера ПЛИС
		ModuleDescriptors->PldDescriptor.Active = FALSE;

		// дескриптор АЦП
		ModuleDescriptors->AdcDescriptor = ModuleDescription.Adc;
		// дескриптор ЦАП
		ModuleDescriptors->DacDescriptor = ModuleDescription.Dac;
	}
	else if(!strcmpi(ModuleName, "E154"))
	{
		MODULE_DESCRIPTION_E154 ModuleDescription;

		if(!pE154->GET_MODULE_DESCRIPTION(&ModuleDescription)) { return FALSE; }

		// дескриптор модуля
		ModuleDescriptors->ModuleDescriptor = ModuleDescription.Module;

		// дескриптор микроконтроллера
		ModuleDescriptors->McuDescriptor.Active = TRUE;
		strcpy(ModuleDescriptors->McuDescriptor.Name, ModuleDescription.Mcu.Name);
		ModuleDescriptors->McuDescriptor.ClockRate = ModuleDescription.Mcu.ClockRate;
		strcpy(ModuleDescriptors->McuDescriptor.Comment, ModuleDescription.Mcu.Comment);
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Version = ModuleDescription.Mcu.Version;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Active = FALSE;

		// дескриптор драйвера DSP
		ModuleDescriptors->DspDescriptor.Active = FALSE;
		// дескриптор драйвера ПЛИС
		ModuleDescriptors->PldDescriptor.Active = FALSE;

		// дескриптор АЦП
		ModuleDescriptors->AdcDescriptor = ModuleDescription.Adc;
		// дескриптор ЦАП
		ModuleDescriptors->DacDescriptor = ModuleDescription.Dac;
	}
	if(!strcmpi(ModuleName, "E-310"))
	{
		MODULE_DESCRIPTION_E310 ModuleDescription;

		if(!pE310->GET_MODULE_DESCRIPTION(&ModuleDescription)) { return FALSE; }

		// дескриптор модуля
		ModuleDescriptors->ModuleDescriptor = ModuleDescription.Module;

		// дескриптор микроконтроллера
		ModuleDescriptors->McuDescriptor.Active = TRUE;
		strcpy(ModuleDescriptors->McuDescriptor.Name, ModuleDescription.Mcu.Name);
		ModuleDescriptors->McuDescriptor.ClockRate = ModuleDescription.Mcu.ClockRate;
		strcpy(ModuleDescriptors->McuDescriptor.Comment, ModuleDescription.Mcu.Comment);
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Version = ModuleDescription.Mcu.Version.FwVersion;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Version = ModuleDescription.Mcu.Version.BlVersion;

		// дескриптор драйвера DSP
		ModuleDescriptors->DspDescriptor.Active = FALSE;
		// дескриптор драйвера ПЛИС
		ModuleDescriptors->PldDescriptor.Active = FALSE;

		// дескриптор АЦП
		ModuleDescriptors->AdcDescriptor = ModuleDescription.Adc;
		// дескриптор ЦАП
		ModuleDescriptors->DacDescriptor = ModuleDescription.Dac;
	}
	else if(!strcmpi(ModuleName, "E440"))
	{
		MODULE_DESCRIPTION_E440 ModuleDescription;

		if(!pE440->GET_MODULE_DESCRIPTION(&ModuleDescription)) { return FALSE; }

		// дескриптор модуля
		ModuleDescriptors->ModuleDescriptor = ModuleDescription.Module;

		// дескриптор микроконтроллера
		ModuleDescriptors->McuDescriptor.Active = TRUE;
		strcpy(ModuleDescriptors->McuDescriptor.Name, ModuleDescription.Mcu.Name);
		ModuleDescriptors->McuDescriptor.ClockRate = ModuleDescription.Mcu.ClockRate;
		strcpy(ModuleDescriptors->McuDescriptor.Comment, ModuleDescription.Mcu.Comment);
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Version = ModuleDescription.Mcu.Version;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Active = FALSE;

		// дескриптор драйвера DSP
		ModuleDescriptors->DspDescriptor.Active = TRUE;
		ModuleDescriptors->DspDescriptor.Dsp = ModuleDescription.Dsp;

		// дескриптор драйвера ПЛИС
		ModuleDescriptors->PldDescriptor.Active = FALSE;

		// дескриптор АЦП
		ModuleDescriptors->AdcDescriptor = ModuleDescription.Adc;
		// дескриптор ЦАП
		ModuleDescriptors->DacDescriptor = ModuleDescription.Dac;
	}
	else if(!strcmpi(ModuleName, "E20-10"))
	{
		MODULE_DESCRIPTION_E2010 ModuleDescription;

		if(!pE2010->GET_MODULE_DESCRIPTION(&ModuleDescription)) { return FALSE; }

		// дескриптор модуля
		ModuleDescriptors->ModuleDescriptor = ModuleDescription.Module;

		// дескриптор микроконтроллера
		ModuleDescriptors->McuDescriptor.Active = TRUE;
		strcpy(ModuleDescriptors->McuDescriptor.Name, ModuleDescription.Mcu.Name);
		ModuleDescriptors->McuDescriptor.ClockRate = ModuleDescription.Mcu.ClockRate;
		strcpy(ModuleDescriptors->McuDescriptor.Comment, ModuleDescription.Mcu.Comment);
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.FirmwareDescriptor.Version = ModuleDescription.Mcu.Version.FwVersion;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Active = TRUE;
		ModuleDescriptors->McuDescriptor.BootLoaderDescriptor.Version = ModuleDescription.Mcu.Version.BlVersion;

		// дескриптор драйвера DSP
		ModuleDescriptors->DspDescriptor.Active = FALSE;

		// дескриптор драйвера ПЛИС
		ModuleDescriptors->PldDescriptor.Active = TRUE;
		ModuleDescriptors->PldDescriptor.Pld = ModuleDescription.Pld;

		// дескриптор АЦП
		ModuleDescriptors->AdcDescriptor = ModuleDescription.Adc;
		// дескриптор ЦАП
		ModuleDescriptors->DacDescriptor = ModuleDescription.Dac;
	}

	// все хорошо :)))))
	return TRUE;
}

