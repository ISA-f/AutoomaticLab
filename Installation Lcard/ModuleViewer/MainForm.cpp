//-----------------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "ModulesMonitorThread.h"
#include "About.h"
//-----------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "LMDButton"
#pragma link "LMDComboBox"
#pragma link "LMDGroupBox"
#pragma link "JustOne"
#pragma link "LMDCustomBevelPanel"
#pragma link "LMDCustomControl"
#pragma link "LMDCustomGroupBox"
#pragma link "LMDCustomPanel"
#pragma link "LMDCustomPanelFill"
#pragma link "LMDCustomParentPanel"
#pragma link "LMDBaseControl"
#pragma link "LMDBaseGraphicControl"
#pragma link "LMDBaseLabel"
#pragma link "LMDControl"
#pragma link "LMDCustomLabel"
#pragma link "LMDLabel"
#pragma resource "*.dfm"

// �������� ����� ���������
TMainForm *MainForm;
// ��������� �� ����� ����������� USB �������
TModulesMonitorThread *ModulesMonitorThread;

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner) : TForm(Owner)
{
}

//-----------------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
	Application->HintHidePause = 500;
	Application->HintHidePause = 5000;
	Application->HintColor = (TColor)10813439;
	Application->OnShortCut = &ApplicationShortCut;

	DecimalSeparator = '.';

	// ����� �����������������...
	IsReenteringInProgress = false;
	// ������ ��� ������ ������ ����������� USB �������
	IsModulesMonitorThreadRunning = false;
	IsModulesMonitorThreadDone = true;

	// ������������ ��������� �����
	TRect r1,r2;
	r1 = Rect(Left+Width/2,Top+Height/2,Left+Width/2,Top+Height/2);
	r2 = BoundsRect;
	DrawAnimatedRects(Handle, ID_ANI_CAPTION, &r1, &r2);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::ApplicationShortCut(TWMKey &Msg, bool &Handled)
{
	if(Msg.CharCode == VK_ESCAPE)
	{
		// �����������������...
		if(IsReenteringInProgress) { Handled = true; return; }
		else IsReenteringInProgress = true;
		//
		if(Application->MessageBox("������������� ������ ��������� ������ ������� ����������?", "����������� ����������!", MB_YESNO + MB_ICONQUESTION) == IDYES)
		{
			Handled = true;
			// ����� ������ ����������
			Application->Terminate();
		}
		// ����� �����������������...
		IsReenteringInProgress = false;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::XJustOneAlreadyExist(TObject *Sender)
{
	Application->MessageBox("��������� ��������� ModulesViewer ��� ��������!", "��������!!!", MB_OK + MB_ICONINFORMATION);
	Application->Terminate();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject *Sender)
{
	StartModulesMonitorThread();
}
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::StartModulesMonitorThread(void)
{
	//
	if(!IsModulesMonitorThreadRunning && IsModulesMonitorThreadDone)
	{
		// ��������, ����� ���������� ��� ��������� �������������������
		IsModulesMonitorThreadDone = false;
		PostMessage(Handle, WM_MODULES_MONITOR_THREAD, 0x0, 0x0);
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::OnStartModulesMonitorThread(TMessage& Message)
{
	IsModulesMonitorThreadRunning = false;
	ModulesMonitorThread = new TModulesMonitorThread(false);
	if(!ModulesMonitorThread) { Application->MessageBox("�� ���� ������� ����� 'ModulesMonitorThread'!","������ OnStartModulesMonitorThreadThread()!", MB_OK); return; }
	ModulesMonitorThread->OnTerminate = ModulesMonitorThreadDone;
	IsModulesMonitorThreadDone = true;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::ModulesMonitorThreadDone(TObject * /*Sender*/)
{
	IsModulesMonitorThreadRunning = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::StopThread(void)
{
	if(ModulesMonitorThread && IsModulesMonitorThreadDone)
	{
		while(!IsModulesMonitorThreadRunning) { if(Application->Terminated) return; Application->ProcessMessages(); Sleep(50); };
		delete ModulesMonitorThread;
		while(IsModulesMonitorThreadRunning) { if(Application->Terminated) return; Application->ProcessMessages(); Sleep(50); };
		ModulesMonitorThread = NULL;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	// !!! ��� ������� �� ��������� ��� ���������� Application->Terminate() !!!

	// �����������������...
	if(IsReenteringInProgress) return;
	else IsReenteringInProgress = true;

	// ������� ������� �����
	StopThread();

	// ����� �����������������...
	IsReenteringInProgress = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
	// �����������������...
	if(IsReenteringInProgress) return;
	else IsReenteringInProgress = true;

	// ������� ������� �����
	StopThread();

	// ������������ ���������� �����
	TRect r1,r2;
	r1 = Rect(Left+Width/2,Top+Height/2,Left+Width/2,Top+Height/2);
	r2 = BoundsRect;
	DrawAnimatedRects(Handle, ID_ANI_CAPTION, &r2, &r1);

	// ����� �����������������...
	IsReenteringInProgress = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::ApplicationExitClick(TObject *Sender)
{
	// ������� �� ����������
	Application->Terminate();
	// ����� �����������������...
	IsReenteringInProgress = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::AboutProgramClick(TObject *Sender)
{
	// ������� ������ About
	AboutProgramPanel->ShowModal();
}

//-----------------------------------------------------------------------------------
// DateTimeToStr(Now()));


