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
	FreeOnTerminate = false;		// требуется явное уничтожения потока
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::Execute()
{
	//---- Place thread code here ----
	// мы в потоке
	Synchronize(InitThread);
	try
	{
		// вечный поток обнаружения USB модулей
		while(!Terminated && !ThreadError)
		{
			// попробуем найти USB модуля
			AccessToModule();
			// что - была ошибка?
			if(Terminated || ThreadError) break;
			// проверим нужно ли обновлять информацию об обнаруженных USB модулях
			Synchronize(ShowModulesInfo);
			// спим-с
			for(BYTE i = 0x0; i < 0x2; i++) { if(Terminated) break; Sleep(100); }
		}
	}
	catch(...)
	{
		// handler for any C++ exception
		Mes = "Опаньки!!! Поймали неустранимое исключение!!!\nСрочно сообщите разработчику!"; ShowErrorMessageBox();
	}

	// освободим используемые ресурсы
	Synchronize(FreeResource);
	// вечный цикл ожидания завершения потока
	while(!Terminated) { Sleep(50); }
}

//-----------------------------------------------------------------------------------
// инициализация текущего потока
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::InitThread(void)
{
	WORD i;

	// сбросим флажок наличия ошибки
	ThreadError = false;
	// мы в потоке
	MainForm->IsModulesMonitorThreadRunning = true;

	// установим флажок запуска потока
	IsThreadLaunching = TRUE;
	// сбросим признак активации панели ожидания
	IsWaitingPanelActivated = false;
	// номер вмртуального слота активного модуля
	CurVirtualSlot = EMPTY_VIRTUAL_SLOT;
	// сбросим признак необходимости обновления информации о USB модулях
	IsRefreshMustBeDone = FALSE;
	// инициализация структуры текущих состояний USB модулей
	InitModulesState();
	// скопируем состояния USB модулей
	OldModulesState = ModulesState;
	// сделаем недоступыми все возможные отображаемые управляющие элементы USB модулей
	ModuleControlElements(false);
	// сделаем недоступным список устройств
	MainForm->ModulesListBox->Enabled = false;

	// иначе попробуем получить интерфейс дескрипторов USB модулей
	DllVersion = GetDllVersion();
	if(DllVersion != CURRENT_VERSION_LUSBAPI)
	{
		Mes = "Неправильная версия библиотеки Lusbapi.dll!\n";
		Mes += "Текущая: " + IntToStr(DllVersion >> 0x10) + "." + IntToStr(DllVersion & 0xFFFF);
		Mes += " Требуется: " + IntToStr(CURRENT_VERSION_LUSBAPI >> 0x10) + "." + IntToStr(CURRENT_VERSION_LUSBAPI & 0xFFFF);
		ShowErrorMessageBox(); return;
	}
	// попробуем получить указатель на интерфейс дескрипторов USB модулей
	pModule = new ILCOMMONINTERFACE;
	if(!pModule) { Mes = "Не могу получить общий интерфейс USB модулей!"; ShowErrorMessageBox(); return; }
	// создание общего интерфейса USB модулей
	else if(!pModule->CreateLInstance()) { Mes = "Не могу создать общий интерфейс USB модулей!"; ShowErrorMessageBox(); return; }
}

//-----------------------------------------------------------------------------------
// попытка обнаружить USB модуля
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::AccessToModule(void)
{
	WORD i;
	BOOL Error;
	AnsiString Str;

	// сбросим флажок наличия ошибки выполнения потока
	ThreadError = false;

	// заново проинициализируем структуру текущих состояний USB модулей
//	InitModulesState();
	// сбросим кол-во обнаруженных USB модулей
	ModulesState.ModulesQuantity = 0x0;
	// сбросим состояния всех возможных модулей в начальные
	CopyMemory(ModulesState.ModuleState, ModuleStatePattern, VIRTUAL_SLOTS_QUANTITY*sizeof(MODULE_STATE));

	// попробуем обнаружить все USB модули в первых VIRTUAL_SLOTS_QUANTITY виртуальных слотах
	for(i = 0x0, Error = FALSE; i < VIRTUAL_SLOTS_QUANTITY; i++)
	{
		// А-А-А-А-А!!!!!! программу прервали!!!
		if(Terminated) break;
		// попробуем открыть соответмствующий виртуальный слот
		if(!pModule->OpenLDevice(i))
		{
			Str = "Слот " + Format("%2u", ARRAYOFCONST((i))) + ": -----";
			strcpy(ModulesState.ModuleState[i].ModuleListBoxString, Str.c_str());
		}
		else
		{
			// инкрементируем кол-во обнаруженных USB модулей
			ModulesState.ModulesQuantity++;
			// виртуальный слот USB модуля
			ModulesState.ModuleState[i].VirtualSlot = i;
			// прочитаем название модуля в обнаруженном виртуальном слоте
			if(!pModule->GetModuleName(ModulesState.ModuleState[i].ModuleName)) { Error = TRUE; break; }
			// узнаем текущую скорость работы шины USB20
			else if(!pModule->GetUsbSpeed(&ModulesState.ModuleState[i].UsbSpeed)) { Error = TRUE; break; }
			// модуль уже загружался?
			else if(OldModulesState.ModuleState[i].IsModuleLoadingMustBeDone)
			{
				// так как модуль E14-440 достаточно медленно загружается, то покажем
				// панель ожидания, чтобы пользователь знал, что программа живёт
				if(!stricmp(ModulesState.ModuleState[i].ModuleName, "E440")) Synchronize(ShowWaitingPanel);
				// проверим загруженность текущего модуля
				if(!pModule->TEST_MODULE())
				{
					// загрузочный код возьмём из соответствующего ресурса штатной DLL библиотеки
					if(!pModule->LOAD_MODULE()) { Error = TRUE; break; }
					// А-А-А-А-А!!!!!! программу прервали!!!
					if(Terminated) break;
					// проверим загрузку модуля
					if(!pModule->TEST_MODULE()) { Error = TRUE; break; }
				}
				// сбросим признак необходимости загрузки модуля
				ModulesState.ModuleState[i].IsModuleLoadingMustBeDone = FALSE;
				// статус загрузки модуля
				ModulesState.ModuleState[i].IsModuleLoaded = TRUE;
				// получим дескрипторы USB модуля
				if(pModule->GET_MODULE_DESCRIPTORS(&ModulesState.ModuleState[i].ModuleDescriptors))
				{
					// установим признак присутствия дескрипторов модуля
					ModulesState.ModuleState[i].IsModuleDescriptorsEnabled = TRUE;
					// информационная строчка
					Str = "Слот " + Format("%2u", ARRAYOFCONST((i))) + ": Модуль " + (AnsiString)((char *)ModulesState.ModuleState[i].ModuleDescriptors.ModuleDescriptor.DeviceName);
					strcpy(ModulesState.ModuleState[i].ModuleListBoxString, Str.c_str());
					// серийный номер модуля
					strncpy(ModulesState.ModuleState[i].ModuleSerialNumber, ModulesState.ModuleState[i].ModuleDescriptors.ModuleDescriptor.SerialNumber, sizeof(CurModuleSerialNumber));
					// ревизия USB модуля
					ModulesState.ModuleState[i].ModuleRevision = ModulesState.ModuleState[i].ModuleDescriptors.ModuleDescriptor.Revision;
					// признак наличия ЦАП
					ModulesState.ModuleState[i].IsDacPresented = ModulesState.ModuleState[i].ModuleDescriptors.DacDescriptor.Active;
				}
				else
				{
					// сбросим признак присутствия дескрипторов модуля
					ModulesState.ModuleState[i].IsModuleDescriptorsEnabled = FALSE;
					// информационная строчка
					Str = "Слот " + Format("%2u", ARRAYOFCONST((i))) + ": Модуль " + (AnsiString)((char *)ModulesState.ModuleState[i].ModuleName);
					strcpy(ModulesState.ModuleState[i].ModuleListBoxString, Str.c_str());
					// ревизия USB модуля
					ModulesState.ModuleState[i].ModuleRevision = 'A';
				}
			}
			else
			{
				// сделаем вид, что кофигурация USB модулей не изменилась
				ModulesState.ModuleState[i] = OldModulesState.ModuleState[i];
			}
		}
	}
	// если нужно - закроем панель ожидания
	Synchronize(HideWaitingPanel);
	// А-А-А-А-А!!!!!! программу прервали!!!
	if(Terminated) return;

	if(!Error && ((ModuleListBoxItemIndex != MainForm->ModulesListBox->ItemIndex) ||
		(!CompareMem(ModulesState.ModuleState, OldModulesState.ModuleState, VIRTUAL_SLOTS_QUANTITY*sizeof(MODULE_STATE)))))
	{
/*		if(ModulesState.ModulesQuantity >= 0x1)
		{
			// попробуем обнаружить первых слот с USB модулем
			for(i = 0x0; i < VIRTUAL_SLOTS_QUANTITY; i++)
				if(ModulesState.ModuleState[i].VirtualSlot == i) break;
			ModulesState.ActiveModule = ModulesState.ModuleState[i];
		}*/
		// отметим активный модуль
		Synchronize(MarkActiveModule);
		// сохраним текущую информацию о USB модулях
		OldModulesState = ModulesState;
	}
}

//-----------------------------------------------------------------------------------
// отметим активный модуль
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::MarkActiveModule(void)
{
	WORD i;

	// установим флажок запуска потока
	if(IsThreadLaunching)
	{
		if(ModulesState.ModulesQuantity >= 0x1)
		{
			// попробуем обнаружить первых слот с USB модулем
			for(i = 0x0; i < VIRTUAL_SLOTS_QUANTITY; i++)
				if(ModulesState.ModuleState[i].VirtualSlot == i) break;
			// в списке активируем первый из обнаруженных USB модулей
			if(i == VIRTUAL_SLOTS_QUANTITY) MainForm->ModulesListBox->ItemIndex = ModuleListBoxItemIndex = -1;
			else MainForm->ModulesListBox->ItemIndex = ModuleListBoxItemIndex = i;
			// определимся с активным модулем
			ModulesState.ActiveModule = ModulesState.ModuleState[i];
			// сделаем доступным список устройств
			MainForm->ModulesListBox->Enabled = true;
			// и сделаем его активным
			MainForm->ModulesListBox->SetFocus();
		}
		else { MainForm->ModulesListBox->ItemIndex = ModuleListBoxItemIndex = -1; }
		// теперь можно сбросить флажок запуска потока
		IsThreadLaunching = FALSE;
		// установим признак необходимости обновления информации о USB модулях
		IsRefreshMustBeDone = TRUE;
	}
	else
	{
		//
		ModulesState.ActiveModule = ModulesState.ModuleState[MainForm->ModulesListBox->ItemIndex];

		// сделаем недоступным список устройств
		if(ModulesState.ModulesQuantity >= 0x1)
		{
			if(!MainForm->ModulesListBox->Enabled) MainForm->ModulesListBox->Enabled = true;
			if(!MainForm->ModulesListBox->Focused()) MainForm->ModulesListBox->SetFocus();
		}
		else
		{
			// сбросим номер текущего виртуального слота
			MainForm->VirtualSlotStaticText->Caption = "";
			// вот такая процедурка
			MainForm->ModulesListBox->ItemIndex = 0x0;
			MainForm->ModulesListBox->ItemIndex = -1;
			// сделаем недоступным список устройств
			MainForm->ModulesListBox->Enabled = false;
			// установим флажок необходимости обновления отображения информации о USB модулях
			IsRefreshMustBeDone = TRUE;
		}

		// если нужно - установим признак необходимости обновления информации о USB модулях
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
// отобразим всё, что надыбали про USB модуля
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::ShowModulesInfo(void)
{
	if(IsRefreshMustBeDone)
	{
		// сделаем доступыми все возможные отображаемые управляющие элементы USB модуля
		ModuleControlElements(true);
		// сбросим признак необходимости обновления информации о USB модулях
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

	// вход в реентерабельность...
	MainForm->IsReenteringInProgress = true;

	for(i = 0x0; i < VIRTUAL_SLOTS_QUANTITY; i++)
		MainForm->ModulesListBox->Items->Strings[i] = (char *)ModulesState.ModuleState[i].ModuleListBoxString;
	if(!EnabledFlag) MainForm->VirtualSlotStaticText->Caption = "";
	else
	{
		Str = ModuleListBoxItemIndex;
		MainForm->VirtualSlotStaticText->Caption = (ModuleListBoxItemIndex != (-1)) ? Str.c_str() : "";
	}

	// заглавие приложения
	if(EnabledFlag)
	{
		if(IsCurModuleLoaded)
		{
			if(CurMcuDescriptors.FirmwareDescriptor.Active) Str = "Мониторинг USB модулей: " + (AnsiString)((char *)CurDeviceDescriptors.ModuleDescriptor.DeviceName) + " --> S/N " + (AnsiString)((char *)CurDeviceDescriptors.ModuleDescriptor.SerialNumber);
			else Str = "Мониторинг USB модулей: " + (AnsiString)((char *)CurModuleName) + " --> S/N " + (AnsiString)((char *)CurModuleSerialNumber)	;
		}
		else Str = "Мониторинг USB модулей: No Module  --> S/N ????????";
	}
	MainForm->Caption = EnabledFlag ? Str.c_str() : "Мониторинг USB модулей: No Module  --> S/N ????????";

	//
	ControlElements((TWinControl *)MainForm->ModuleGroupBox, EnabledFlag);

	// название модуля
	if(EnabledFlag)
	{
		if(CurMcuDescriptors.FirmwareDescriptor.Active) Str = "Модуль " + (AnsiString)((char *)CurDeviceDescriptors.ModuleDescriptor.DeviceName);
		else Str = "Модуль " + (AnsiString)((char *)CurModuleName);
	}
	Str = "Модуль " + (AnsiString)((char *)CurDeviceDescriptors.ModuleDescriptor.DeviceName);
	MainForm->ModuleGroupBox->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "Модуль ????") : "Модуль ????";

	// светодиод загрузки модуля
	MainForm->ModuleLoadingLed->Brush->Color = EnabledFlag ? ((CurVirtualSlot != (-1)) ? (IsCurModuleLoaded ? clLime : clRed): clBtnFace) : clBtnFace;
	// светодиод текущей скорости USB
	MainForm->UsbSpeedLed->Brush->Color = EnabledFlag & (IsCurModuleLoaded) ? (CurUsbSpeed ? clLime : (TColor)RGB(255, 150,0 )) : clBtnFace;
	MainForm->SpeedModeStaticText->Caption = EnabledFlag ? (IsCurModuleLoaded ? (CurUsbSpeed ? "HS" : "FS") : "??") : "??";

	// серийный номер модуля
		CurModuleSerialNumber[0x8] = '\0';
		MainForm->SerialNumberLMDLabel->Enabled = EnabledFlag ? (IsCurModuleDescriptorEnabled ? true : false) : false;
		Str = (char *)CurModuleSerialNumber;
		MainForm->SerialNumberLMDLabel->Caption = EnabledFlag ? (IsCurModuleDescriptorEnabled ? Str.c_str() : "--------") : "--------";

	// ревизия модуля
		MainForm->ModuleRevisionLMDLabel->Enabled = EnabledFlag ? (IsCurModuleDescriptorEnabled ? true : false) : false;
		Str = (char)CurModuleRevision;
		MainForm->ModuleRevisionLMDLabel->Caption = EnabledFlag ? (IsCurModuleDescriptorEnabled ? Str.c_str() : "-") : "-";

	// наличие ЦАП
		MainForm->DacLMDLabel->Enabled = EnabledFlag ? (IsCurModuleDescriptorEnabled ? true : false) : false;
		MainForm->DacLMDLabel->Caption = EnabledFlag ? (IsCurModuleDescriptorEnabled ? (CurIsDacPresented ? "ЕСТЬ" : "НЕТ") : "----") : "----";

	// информация о драйвере MCU
	MainForm->McuLMDGroupBox->Enabled = IsCurFirmwareDescriptorEnabled ? true : false;
	if(IsCurFirmwareDescriptorEnabled)
	{
		MainForm->McuTypeLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->McuTypeLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->McuTypeLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? (char *)CurMcuDescriptors.Name : "------------") : "------------";
		MainForm->McuFreqLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->McuFreqLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		Str = FloatToStr(CurMcuDescriptors.ClockRate) + " кГц";
		MainForm->McuFreqLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "------------") : "------------";
		MainForm->McuFirmwareVersionLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->McuFirmwareVersionLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		Str = (AnsiString)((char *)CurMcuDescriptors.FirmwareDescriptor.Version.Version) + " от " + (AnsiString)((char *)CurMcuDescriptors.FirmwareDescriptor.Version.Date);
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

	// информация о загрузчике MCU
	MainForm->BootLoaderLMDGroupBox->Enabled = IsCurBootLoaderDescriptorEnabled ? true : false;
	if(IsCurBootLoaderDescriptorEnabled)
	{
		MainForm->McuBootLoadeVersionLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? (IsCurBootLoaderDescriptorEnabled ? true : false) : false) : false;
		MainForm->McuBootLoaderVersionLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? (IsCurBootLoaderDescriptorEnabled ? true : false) : false) : false;
		Str = (AnsiString)((char *)CurMcuDescriptors.BootLoaderDescriptor.Version.Version) + " от " + (AnsiString)((char *)CurMcuDescriptors.BootLoaderDescriptor.Version.Date);
		MainForm->McuBootLoaderVersionLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? (IsCurBootLoaderDescriptorEnabled ? Str.c_str() : "????????????") : "????????????") : "????????????";
	}
	else
	{
		MainForm->McuBootLoadeVersionLabelLMDLabel->Enabled = false;
		MainForm->McuBootLoaderVersionLMDLabel->Enabled = false;
		MainForm->McuBootLoaderVersionLMDLabel->Caption = "------------";
	}

	// информация о DSP
	MainForm->DspLMDGroupBox->Enabled = IsCurDspDescriptorEnabled ? true : false;
	if(IsCurDspDescriptorEnabled)
	{
		MainForm->DspTypeLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		MainForm->DspTypeLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->DspTypeLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? (char *)CurDspDescriptors.Dsp.Name : "????????????") : "????????????";
		MainForm->DspFreqLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->DspFreqLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		Str = FloatToStr(CurDspDescriptors.Dsp.ClockRate) + " кГц";
		MainForm->DspFreqLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "????????????") : "????????????";
		MainForm->DspFirmwareVersionLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		MainForm->DspFirmwareVersionLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		Str = (AnsiString)((char *)CurDspDescriptors.Dsp.Version.Version) + " от " + (AnsiString)((char *)CurDspDescriptors.Dsp.Version.Date);
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

	// информация о ПЛИС
	MainForm->PldLMDGroupBox->Enabled = IsCurPldDescriptorEnabled ? true : false;
	if(IsCurPldDescriptorEnabled)
	{
		MainForm->PldTypeLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		MainForm->PldTypeLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->PldTypeLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? (char *)CurPldDescriptors.Pld.Name : "????????????") : "????????????";
		MainForm->PldFreqLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		MainForm->PldFreqLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;
		Str = FloatToStr(CurPldDescriptors.Pld.ClockRate) + " кГц";
		MainForm->PldFreqLMDLabel->Caption = EnabledFlag ? (IsCurModuleLoaded ? Str.c_str() : "????????????") : "????????????";
		MainForm->PldFirmwareVersionLabelLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		MainForm->PldFirmwareVersionLMDLabel->Enabled = EnabledFlag ? (IsCurModuleLoaded ? true : false) : false;;
		Str = (AnsiString)((char *)CurPldDescriptors.Pld.Version.Version) + " от " + (AnsiString)((char *)CurPldDescriptors.Pld.Version.Date);
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

	// конец реентерабельности...
	MainForm->IsReenteringInProgress = false;
}

//-----------------------------------------------------------------------------------
// покажем панель ожидания
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::ShowWaitingPanel(void)
{
	// если нужно - активируем панель ожидания
	if(!IsWaitingPanelActivated) WaitingPanel->Show();
	// установим признак активации панели ожидания
	IsWaitingPanelActivated = true;
}

//-----------------------------------------------------------------------------------
// спрячем панель ожидания
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::HideWaitingPanel(void)
{
	// если нужно - деактивируем панель ожидания
	if(IsWaitingPanelActivated) WaitingPanel->Hide();
	// сбросим признак активации панели ожидания
	IsWaitingPanelActivated = false;
}



//-----------------------------------------------------------------------------------
// инициализация структуры текущих состояний USB модулей
//-----------------------------------------------------------------------------------
void TModulesMonitorThread::InitModulesState(void)
{
	WORD i;
	MODULE_STATE LocModuleState;

	// сбросим кол-во обнаруженных USB модулей
	ModulesState.ModulesQuantity = 0x0;
	// ====== активный USB модуль ===============================
	// виртуальный слот активного USB модуля
	LocModuleState.VirtualSlot = EMPTY_VIRTUAL_SLOT;
	// установим признак необходимости загрузки модуля
	LocModuleState.IsModuleLoadingMustBeDone = TRUE;
	// признак необходимост загрузки модуля
	LocModuleState.IsModuleLoadingMustBeDone = TRUE;
	// статус загрузки активного USB модуля
	LocModuleState.IsModuleLoaded = FALSE;
	// признак присутствия дескрипторов на активном USB модуле
	LocModuleState.IsModuleDescriptorsEnabled = FALSE;
	// признак наличия ЦАП
	LocModuleState.IsDacPresented = FALSE;
	// структура с дескрипторами активного USB модуля
	ZeroMemory(&LocModuleState.ModuleDescriptors, sizeof(ModulesState.ActiveModule.ModuleDescriptors));
	// строчка с типом активного USB модуля
	ZeroMemory(LocModuleState.ModuleListBoxString, sizeof(ModulesState.ActiveModule.ModuleListBoxString));
	// название активного USB модуля
	ZeroMemory(LocModuleState.ModuleName, sizeof(ModulesState.ActiveModule.ModuleName));
	// серийный номер модуля
	ZeroMemory(LocModuleState.ModuleSerialNumber,sizeof(ModulesState.ActiveModule.ModuleSerialNumber));
	// ревизия активного USB модуля
	LocModuleState.ModuleRevision = '\0';
	// скорость работы шины USB для активного USB модуля
	LocModuleState.UsbSpeed = INVALID_USB_SPEED_LUSBAPI;
	// =========================================================
	// теперь инициализируем массив структур USB модулей
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
	// если нужно - спрячем панель ожидания
	if(!Application->Terminated)
		{ if(WaitingPanel->Visible) WaitingPanel->Hide(); }
	// полностью освободим указатель на модуль
	if(pModule) { delete pModule; pModule = NULL; }
}

//-----------------------------------------------------------------------------------
// отображение информационного сообщения
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::ShowInfoMessageBox(void)
{
	Application->MessageBox(Mes.c_str(),"Сообщение TModulesMonitorThread::Execute()!", MB_OK + MB_ICONINFORMATION);
}

//-----------------------------------------------------------------------------------
// отображение сообщение с ошибкой
//-----------------------------------------------------------------------------------
void __fastcall TModulesMonitorThread::ShowErrorMessageBox(void)
{
	if(ThreadError) return;
	Application->MessageBox(Mes.c_str(),"Сообщение TModulesMonitorThread::Execute()!", MB_OK + MB_ICONINFORMATION);
	ThreadError = true;
}
//-----------------------------------------------------------------------------------

