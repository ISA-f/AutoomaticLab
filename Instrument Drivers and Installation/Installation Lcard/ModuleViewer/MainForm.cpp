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

// основная форма программы
TMainForm *MainForm;
// указатель на поток обнаружения USB модулей
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

	// конец реентерабельности...
	IsReenteringInProgress = false;
	// флажки для работы потока обнаружения USB модулей
	IsModulesMonitorThreadRunning = false;
	IsModulesMonitorThreadDone = true;

	// анимационное появление формы
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
		// реентерабельность...
		if(IsReenteringInProgress) { Handled = true; return; }
		else IsReenteringInProgress = true;
		//
		if(Application->MessageBox("Действительно хотите завершить работу данного приложения?", "Подтвердите завершение!", MB_YESNO + MB_ICONQUESTION) == IDYES)
		{
			Handled = true;
			// нагло прервём приложение
			Application->Terminate();
		}
		// конец реентерабельности...
		IsReenteringInProgress = false;
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::XJustOneAlreadyExist(TObject *Sender)
{
	Application->MessageBox("Экземпляр программы ModulesViewer уже работает!", "Внимание!!!", MB_OK + MB_ICONINFORMATION);
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
		// запомним, какие устройства нам требуется проинициализировать
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
	if(!ModulesMonitorThread) { Application->MessageBox("Не могу открыть поток 'ModulesMonitorThread'!","Ошибка OnStartModulesMonitorThreadThread()!", MB_OK); return; }
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
	// !!! Это событие не генерится при выполнении Application->Terminate() !!!

	// реентерабельность...
	if(IsReenteringInProgress) return;
	else IsReenteringInProgress = true;

	// прервем текущий поток
	StopThread();

	// конец реентерабельности...
	IsReenteringInProgress = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
	// реентерабельность...
	if(IsReenteringInProgress) return;
	else IsReenteringInProgress = true;

	// прервем текущий поток
	StopThread();

	// анимационное свёртывание формы
	TRect r1,r2;
	r1 = Rect(Left+Width/2,Top+Height/2,Left+Width/2,Top+Height/2);
	r2 = BoundsRect;
	DrawAnimatedRects(Handle, ID_ANI_CAPTION, &r2, &r1);

	// конец реентерабельности...
	IsReenteringInProgress = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::ApplicationExitClick(TObject *Sender)
{
	// выходим из приложения
	Application->Terminate();
	// конец реентерабельности...
	IsReenteringInProgress = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void __fastcall TMainForm::AboutProgramClick(TObject *Sender)
{
	// покажем панель About
	AboutProgramPanel->ShowModal();
}

//-----------------------------------------------------------------------------------
// DateTimeToStr(Now()));


