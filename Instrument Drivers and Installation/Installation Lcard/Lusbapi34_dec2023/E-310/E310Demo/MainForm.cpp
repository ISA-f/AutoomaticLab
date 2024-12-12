//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "GeneratorFrame.h"
#include "MainForm.h"
#include "InitDevicesThread.h"
#include "About.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDCustomBevelPanel"
#pragma link "LMDCustomControl"
#pragma link "LMDCustomGroupBox"
#pragma link "LMDCustomPanel"
#pragma link "LMDCustomParentPanel"
#pragma link "LMDGroupBox"
#pragma link "LMDBaseGraphicControl"
#pragma link "LMDButton"
#pragma link "LMDButtonControl"
#pragma link "LMDCheckBox"
#pragma link "LMDControl"
#pragma link "LMDCustomButton"
#pragma link "LMDCustomCheckBox"
#pragma link "LMDCustomPanelFill"
#pragma link "LMDLabel"
#pragma link "LMDCustomButtonGroup"
#pragma link "LMDCustomRadioGroup"
#pragma link "LMDRadioGroup"
#pragma link "Pages"
#pragma link "BMPage"
#pragma link "LMDBaseLabel"
#pragma link "LMDCustomLabel"
#pragma link "LMDLabel"
#pragma resource "*.dfm"

// �������� ����� ���������
TMainForm *MainForm;
// ��������� �� ����� ����������� ������ E-310
TInitDevicesThread *InitDevicesThread;
// ��������� �� ��������� ������ E-310
ILE310 *pModule;

DWORD test;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner) : TForm(Owner)
{
	E310GeneratorFrame = new TE310GeneratorFrame(GeneratorParsLMDGroupBox);
	// ����� ������?
	if(!E310GeneratorFrame) return;
	// ��������� ���������� ������ ���������� ��������
	E310GeneratorFrame->Parent = GeneratorParsLMDGroupBox;
	E310GeneratorFrame->Top		= 25;
	E310GeneratorFrame->Left	= 2;
	GeneratorParsLMDGroupBox->Width = E310GeneratorFrame->Left + E310GeneratorFrame->Width + 0x2;
	GeneratorParsLMDGroupBox->Height = E310GeneratorFrame->Top + E310GeneratorFrame->Height + 0x2;
	Width = GeneratorParsLMDGroupBox->Left + GeneratorParsLMDGroupBox->Width + 0x7;
	Height = GeneratorParsLMDGroupBox->Top + GeneratorParsLMDGroupBox->Height + 31;
	// ������ ����� �������
	E310GeneratorFrame->Visible = true;

	// ����������� � ������� ������������ Windows
	WindowsVersion = GetWindowsVersion();
	if((WindowsVersion >= WINDOWS_7))
	{
		// ��� windows 7 � ���� �������� ������ ������ ��������� LMD
		LmdControlElementsTuning(MainForm);
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
	Application->OnShortCut = &ApplicationShortCut;

	// �����������
	DecimalSeparator = '.';

	// ������� ������� ���������� ����������
	IsAppTerminated = false;
	// ��������� ������ ���������
	IsReenteringInProgress = false;
	// ��������� ������� ������� ����������
	IsProgramLaunching = true;
	// ������������� ������� ��� ������ ����������� ������������
	IsInitDevicesThreadRunning = false;
	IsInitDevicesThreadDone = true;

	// �����������������...
//	IsReenteringInProgress = true;

}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormActivate(TObject *Sender)
{
	DecimalSeparator = '.';
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void __fastcall TMainForm::ApplicationShortCut(TWMKey &Msg, bool &Handled)
{
	if(Msg.CharCode == VK_ESCAPE)
	{
		// �����������������...
		if(IsReenteringInProgress) { Handled = true; return; }
		else IsReenteringInProgress = true;

		if(Application->MessageBox("������������� ������ ��������� ������?", "����������� ����������!", MB_YESNO + MB_ICONQUESTION) == IDYES)
		{
			Handled = true;
			Application->Terminate();
		}

		// ����� �����������������...
		IsReenteringInProgress = false;
	}
	else if(Msg.CharCode == VK_F1)
	{
//		SetModuleParsLMDButtonClick(SetModuleParsLMDButton);
	}
}

//---------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *Sender)
{
	// �������� ����� �� ����������� ������ E-310
//	StartInitModuleThread();
}

//---------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject *Sender)
{
	// �������� ����� �� ��������� ����� ����������� ������
	if(IsProgramLaunching)
	{
		// �������� ����� �� ����������� ������ E-310
		StartInitModuleThread();
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void TMainForm::ControlElements(TWinControl *WinControlElement, bool EnabledFlag)
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

//---------------------------------------------------------------------------
// ������ ������ ������ � ������������� ������
//---------------------------------------------------------------------------
void TMainForm::StartInitModuleThread(void)
{
	if(!IsInitDevicesThreadRunning && IsInitDevicesThreadDone)
	{
		IsInitDevicesThreadDone = false;
		PostMessage(Handle, WM_START_INIT_MODULE_THREAD, 0x0, 0x0);
	}
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnStartInitModuleThread(TMessage& Message)
{
	// ������� ��������� ����� ����������� ������
	IsInitDevicesThreadRunning = false;
	InitDevicesThread = new TInitDevicesThread(false);
	if(!InitDevicesThread) { Application->MessageBox("�� ���� ������� ����� 'InitDevicesThread'!","������ OnStartInitDevicesThreadThread()!", MB_OK); return; }
	InitDevicesThread->OnTerminate = InitDevicesThreadDone;
	IsInitDevicesThreadDone = true;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::InitDevicesThreadDone(TObject * /*Sender*/)
{
	IsInitDevicesThreadRunning = false;
}

//---------------------------------------------------------------------------
// ���� ����� - ��� ���������� ������ ����������� ������
//---------------------------------------------------------------------------
BOOL TMainForm::WaitFortInitDevicesThreadDone(void)
{
	while(IsInitDevicesThreadRunning)
	{
		if(Application->Terminated) return FALSE;
		Application->ProcessMessages();
		Sleep(50);
	};
	return TRUE;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	// ��� ������� �� ��������� ��� ���������� Application->Terminate();
	// ������ ������ ���������� ����������
	IsAppTerminated = true;
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
	// ������ ������ ���������� ����������
	IsAppTerminated = true;

	// �����������������...
	if(IsReenteringInProgress) return;
	else IsReenteringInProgress = true;

	// �������������� ���� ����� - �������� ������� ���������
	if(AutoSaveSettingsLMDCheckBox->Checked) SaveSettingsButtonClick(Sender);
	// �������� ���� ��������
	if(IniFile) { delete IniFile; IniFile = NULL; }
	// ��������� ��������� ��������� �� ������
	if(pModule)
	{
		pModule->STOP_GENERATOR();
		pModule->ReleaseLInstance();
		pModule = NULL;
	}

	// ����� �����������������...
	IsReenteringInProgress = false;
}

//---------------------------------------------------------------------------
// ������������� ��������� � ������
//---------------------------------------------------------------------------
bool TMainForm::InitApplication(TIniFile *IniFile)
{
	E310GeneratorFrame->InitGeneratorFramePars(pModule, &GeneratorPars);

	// �����������������...
	IsReenteringInProgress = true;

	// ��� ��� � ��� � ��������������� ��������
	AutoSaveSettingsLMDCheckBox->Checked = IniFile ? IniFile->ReadInteger("����������. �����", "���������� ��������", false) : false;

	// �� ������
	return true;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::AutoSaveSettingsLMDCheckBoxChange(TObject *Sender)
{
	if(AutoSaveSettingsLMDCheckBox->Checked) SaveSettingsButton->Enabled = false;
	else SaveSettingsButton->Enabled = true;
}

//---------------------------------------------------------------------------
// �������� ����� ������� ��������
//---------------------------------------------------------------------------
bool TMainForm::CreateDefaultIniFile(AnsiString IniFileName)
{
	AnsiString Str;

	// �� ������ ������ �������� ���� ��������
	if(!IniFile) { delete IniFile; IniFile = NULL; };
	// �������� �������� �����
	if(IniFileName == "") { Application->MessageBox("������������ �������� ����� ������� ��������!", "������ CreateDefaultIniFile()!", MB_OK + MB_ICONINFORMATION); return false; }
	// ������ �������� ��������� ������� ���� ��������
	IniFile = new TIniFile(IniFileName);
	// ��������
	if(!IniFile) { Str = "�� ���� ������� ���� ������� �������� '" + IniFileName + "'!"; Application->MessageBox(Str.c_str(), "������ CreateDefaultIniFile()!", MB_OK + MB_ICONINFORMATION); return false; }

	// �������� ����������
	IniFile->WriteString("�����", "�������� ���������", ParamStr(0x0));

	// �������������� ��������
	IniFile->WriteBool("����������. �����", "���������� ��������", false);
	// ���������� �������� ������ ���������
	IniFile->WriteInteger("����������. �����", "X ���������� ��������� ������", 20);
	IniFile->WriteInteger("����������. �����", "Y ���������� ��������� ������", 20);

	// ��������� ����������
	IniFile->WriteFloat("������ E-310",		"���������. ��������� ������� � ���", 10.0);
	IniFile->WriteFloat("������ E-310",		"���������. ������� ���������� � ���", 50.0);
	IniFile->WriteInteger("������ E-310",	"���������. ���-�� ����������", 10);
	IniFile->WriteInteger("������ E-310",	"���������. ��� �������� ��������� ����������", CLOCK_PERIOD_INCREMENT_INTERVAL_E310);
	IniFile->WriteInteger("������ E-310",	"���������. ������ ���������� ��� �������� ��������� ����������", INCREMENT_INTERVAL_MULTIPLIERS_001_E310);
	IniFile->WriteInteger("������ E-310",	"���������. ���-�� ������� ���������� � ��������� ����������", 100);
	IniFile->WriteFloat("������ E-310",		"���������. ������� ������������ �������", 50000.0);
	IniFile->WriteInteger("������ E-310",	"���������. �������� ������������ �������", INTERNAL_MASTER_CLOCK_E310);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ������������ ����������������", NO_CYCLIC_AUTOSCAN_E310);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ������������� �������", CTRL_LINE_INCREMENT_E310);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ����� 'CTRL'", INTERNAL_CTRL_LINE_E310);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ����� 'INTERRUPT'", INTERNAL_INTERRUPT_LINE_E310);
	IniFile->WriteBool("������ E-310",		"���������. ���������� ��������� ������ ����������", false);
	IniFile->WriteBool("������ E-310",		"���������. ���������� �������������", false);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ������������ �������������", SYNCOUT_AT_END_OF_SCAN_E310);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ����������� �������", SINUSOIDAL_ANALOG_OUTPUT_E310);
	IniFile->WriteInteger("������ E-310",	"���������. ������ �������� ��������� ������", ANALOG_OUTPUT_GAIN_PLUS_10_DB_E310);
	IniFile->WriteFloat("������ E-310",		"���������. �������� ����������� �������� �� ������ 10 ��", 0.0);
	IniFile->WriteInteger("������ E-310",	"���������. ��� �������� �� ������ 10 ��", INTERNAL_OUTPUT_10_OHM_OFFSET_E310);

	// ��������� ������������ ���� ��������
	if(IniFile) { delete IniFile; IniFile = NULL; }
	// �� ������
	return true;
}


//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::SaveSettingsButtonClick(TObject *Sender)
{
	AnsiString IniFileName = "E310Config.ini";

	// ������ ������� ��������� ������ ����������
	E310GeneratorFrame->GetGeneratorFramePars(&MainForm->GeneratorPars);

	// ��������� ������ �������� �����
	IniFileName = ExtractFileDir(ParamStr(0)) + "\\" + IniFileName;
	// ��������� ������������ ���� ��������
	if(IniFile) { delete IniFile; IniFile = NULL; }

	// ������� ���� ��������
	IniFile = new TIniFile(IniFileName);
	IniFile->WriteString("�����", "�������� ���������", ParamStr(0));

	// ��� ��� � ��� � ��������������� ��������
	IniFile->WriteBool("����������. �����", "���������� ��������", AutoSaveSettingsLMDCheckBox->Checked);
	IniFile->WriteInteger("����������. �����", "X ���������� ��������� ������", MainForm->Left);
	IniFile->WriteInteger("����������. �����", "Y ���������� ��������� ������", MainForm->Top);

	// ��������� ����������
	IniFile->WriteFloat("������ E-310",		"���������. ��������� ������� � ���", GeneratorPars.StartFrequency);
	IniFile->WriteFloat("������ E-310",		"���������. ������� ���������� � ���", GeneratorPars.FrequencyIncrements);
	IniFile->WriteInteger("������ E-310",	"���������. ���-�� ����������", GeneratorPars.NumberOfIncrements);
	IniFile->WriteInteger("������ E-310",	"���������. ��� �������� ��������� ����������", GeneratorPars.IncrementIntervalPars.BaseIntervalType);
	IniFile->WriteInteger("������ E-310",	"���������. ������ ���������� ��� �������� ��������� ����������", GeneratorPars.IncrementIntervalPars.MultiplierIndex);
	IniFile->WriteInteger("������ E-310",	"���������. ���-�� ������� ���������� � ��������� ����������", GeneratorPars.IncrementIntervalPars.BaseIntervalsNumber);
	IniFile->WriteFloat("������ E-310",		"���������. ������� ������������ �������", GeneratorPars.MasterClock);
	IniFile->WriteInteger("������ E-310",	"���������. �������� ������������ �������", GeneratorPars.MasterClockSource);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ������������ ����������������", GeneratorPars.CyclicAutoScanType);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ������������� �������", GeneratorPars.IncrementType);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ����� 'CTRL'", GeneratorPars.CtrlLineType);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ����� 'INTERRUPT'", GeneratorPars.InterrupLineType);
	IniFile->WriteBool("������ E-310",		"���������. ���������� ��������� ������ ����������", GeneratorPars.SquareWaveOutputEna);
	IniFile->WriteBool("������ E-310",		"���������. ���������� �������������", GeneratorPars.SynchroOutEna);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ������������ �������������", GeneratorPars.SynchroOutType);
	IniFile->WriteInteger("������ E-310",	"���������. ��� ����������� �������", GeneratorPars.AnalogOutputsPars.SignalType);
	IniFile->WriteInteger("������ E-310",	"���������. ������ �������� ��������� ������", GeneratorPars.AnalogOutputsPars.GainIndex);
	IniFile->WriteFloat("������ E-310",		"���������. �������� ����������� �������� �� ������ 10 ��", GeneratorPars.AnalogOutputsPars.Output10OhmOffset);
	IniFile->WriteInteger("������ E-310",	"���������. ��� �������� �� ������ 10 ��", GeneratorPars.AnalogOutputsPars.Output10OhmOffsetSource);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::FindModuleLMDButtonClick(TObject *Sender)
{
	// �������� ����� �� ����������� ������ E-310
	StartInitModuleThread();
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::ApplicationExitClick(TObject *Sender)
{
	// ������� �� ����������
	Application->Terminate();
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::SaveDefaultSettingsClick(TObject *Sender)
{
	AnsiString IniFileName = "E310Config.ini";

	// ��������� ������ �������� �����
	IniFileName = ExtractFileDir(ParamStr(0)) + "\\" + IniFileName;
	// �������� ��������� ������ ��������� �� ���������
	CreateDefaultIniFile(IniFileName);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::SaveCurrentSettingsClick(TObject *Sender)
{
	SaveSettingsButtonClick(SaveSettingsButton);
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void __fastcall TMainForm::AboutProgramClick(TObject *Sender)
{
	// ������� ������ About
	AboutProgramPanel->ShowModal();
}

//------------------------------------------------------------------------------
// ��������� �������� ������ ������������ Windows
//------------------------------------------------------------------------------
DWORD TMainForm::GetWindowsVersion(void)
{
	OSVERSIONINFO osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if(!GetVersionEx(&osvi))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx((OSVERSIONINFO *)&osvi)) return UNKNOWN_WINDOWS_VERSION;
	}

	switch (osvi.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_NT:
			if(osvi.dwMajorVersion <= 4 ) return WINDOWS_NT; 			// WinNT
			else if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 0)) return WINDOWS_2000; 			// Win2000
			else if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 1)) return WINDOWS_XP; 				// WinXP
			else if((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion == 2)) return WINDOWS_2003_SERVER; 	// Win2003 Server
			else if((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 0)) return WINDOWS_VISTA; 	// WinVista ���  WinServer 2008
			else if((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 1)) return WINDOWS_7; 		// Win7     ���  WinServer 2008 R2
			else if(osvi.dwMajorVersion == 6) return WINDOWS_FUTURE;			// �������� ��� ������� Windows
			else return UNKNOWN_WINDOWS_VERSION;

		case VER_PLATFORM_WIN32_WINDOWS:
			if((osvi.dwMajorVersion > 0x4) || ((osvi.dwMajorVersion == 0x4) &&
				(osvi.dwMinorVersion > 0x0))) return WINDOWS_98_OR_LATER; // Win98 or later
			else return WINDOWS_95; 											// Win95

		case VER_PLATFORM_WIN32s: return WINDOWS_32S; 					// Win32s
	}

	return UNKNOWN_WINDOWS_VERSION;
}

//---------------------------------------------------------------------------
// �������� LMD ���������
//---------------------------------------------------------------------------
void TMainForm::LmdControlElementsTuning(TWinControl *WinControlElement)
{
	DWORD i;

	for(i = 0x0; i < (DWORD)WinControlElement->ControlCount; i++)
	{
		TWinControl *WinControl;
		WinControl = dynamic_cast<TWinControl *>(WinControlElement->Controls[i]);
		if(WinControl) LmdControlElementsTuning(dynamic_cast<TWinControl *>(WinControlElement->Controls[i]));

		// ������� �������� ������
		TLMDGroupBox *LMDGroupBox;
		LMDGroupBox = dynamic_cast<TLMDGroupBox *>(WinControlElement->Controls[i]);
//		if(LMDGroupBox) LMDGroupBox->CaptionFont3D->Tracing = 0x0;
		if(LMDGroupBox)
		{
			LMDGroupBox->Bevel->Mode			= bmEdge;
			LMDGroupBox->Bevel->EdgeStyle		= etEtched;
			LMDGroupBox->CtlXPCaptionColor	= false;
		}

		// ������� �������� ������ �����������
		TLMDRadioGroup *LMDRadioGroup;
		LMDRadioGroup = dynamic_cast<TLMDRadioGroup *>(WinControlElement->Controls[i]);
//		if(LMDRadioGroup) LMDRadioGroup->CaptionFont3D->Tracing = 0x0;
		if(LMDRadioGroup)
		{
			LMDRadioGroup->Bevel->Mode			= bmEdge;
			LMDRadioGroup->Bevel->EdgeStyle	= etEtched;
			LMDRadioGroup->CtlXPCaptionColor	= false;
		}

		// ������� �������� ������
/*		TLMDButton *LMDButton;
		LMDButton = dynamic_cast<TLMDButton *>(WinControlElement->Controls[i]);
		if(LMDButton) LMDButton->FontFX->Tracing = 0x0;*/

		// ������� �������� ������ ������
/*		TLMDCheckBox *LMDCheckBox;
		LMDCheckBox = dynamic_cast<TLMDCheckBox *>(WinControlElement->Controls[i]);
		if(LMDCheckBox) LMDCheckBox->Font3D->Tracing = 0x0;*/
	}
}
//---------------------------------------------------------------------------


