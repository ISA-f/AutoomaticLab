//-----------------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "About.h"
#include "Lusbapi.h"
//-----------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDCustomBevelPanel"
#pragma link "LMDCustomControl"
#pragma link "LMDCustomGroupBox"
#pragma link "LMDCustomPanel"
#pragma link "LMDCustomPanelFill"
#pragma link "LMDCustomParentPanel"
#pragma link "LMDGroupBox"
#pragma link "LMDButton"
#pragma link "LMDCustomButton"
#pragma link "PJVersionInfo"
#pragma link "ESColorMemo"
#pragma link "URLLink"
#pragma link "LMDBaseControl"
#pragma link "LMDBaseGraphicControl"
#pragma link "LMDBaseLabel"
#pragma link "LMDControl"
#pragma link "LMDCustomSimpleLabel"
#pragma link "LMDSimpleLabel"
#pragma link "LMDBaseEdit"
#pragma link "LMDCustomMemo"
#pragma link "LMDMemo"
#pragma link "LMDBaseControl"
#pragma link "LMDBaseGraphicControl"
#pragma link "LMDBaseLabel"
#pragma link "LMDControl"
#pragma link "LMDCustomLabel"
#pragma link "LMDLabel"
#pragma resource "*.dfm"

TAboutProgramPanel *AboutProgramPanel;

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
__fastcall TAboutProgramPanel::TAboutProgramPanel(TComponent* Owner) : TForm(Owner)
{
	// ������ �������� ��������� ModulesViewer.exe
	if(PJVersionInfo->HaveInfo)
		ModulesViewerVersionLMDLabel->Caption = PJVersionInfo->FileVersion;

	// ������ ���������� Lusbapi.dll
	PJVersionInfo->FileName = "Lusbapi.dll";
	if(PJVersionInfo->HaveInfo)
		LusbapiVersionLMDLabel->Caption = PJVersionInfo->FileVersion;

	// ������ USB �������� Ldevusbu.sys
	PJVersionInfo->FileName = GetWinDir() + "\\system32\\drivers\\Ldevusbu.sys";
	if(PJVersionInfo->HaveInfo)
		LdevusbuVersionLMDLabel->Caption = PJVersionInfo->FileVersion;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TAboutProgramPanel::FormShow(TObject *Sender)
{
	// �������� ������ ���������� OnShortCut
	OnShortCutAddress = Application->OnShortCut;
	// ����� ���������� OnShortCut ������������� ��� ������ �����
	Application->OnShortCut = &ApplicationShortCut;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TAboutProgramPanel::FormHide(TObject *Sender)
{
	// ����������� ����������� ���������� OnShortCut
	Application->OnShortCut = OnShortCutAddress;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// ��������� ������� ������� ESCAPE �� ����������
//-----------------------------------------------------------------------------------
void __fastcall TAboutProgramPanel::ApplicationShortCut(TWMKey &Msg, bool &Handled)
{
	WORD i;

	if(Msg.CharCode == VK_ESCAPE)
	{
		Handled = true;
		ModalResult = mrOk;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TAboutProgramPanel::OkLMDButtonClick(TObject *Sender)
{
	ModalResult = mrOk;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
AnsiString TAboutProgramPanel::GetWinDir(void)
{
	char wd[MAX_PATH];
	GetWindowsDirectory(wd, sizeof(wd));
	return AnsiString(wd);
}
//-----------------------------------------------------------------------------------

