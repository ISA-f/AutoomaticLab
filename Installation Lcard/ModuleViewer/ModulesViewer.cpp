//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
USERES("ModulesViewer.res");
USEFORM("MainForm.cpp", MainForm);
USEFORM("About.cpp", AboutProgramPanel);
USEUNIT("ModulesMonitorThread.cpp");
USEUNIT("CommonInterface.cpp");
USELIB("..\DLL\Lib\Borland\Lusbapi.lib");
USEFORM("Waiting.cpp", WaitingPanel);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "Мониторинг USB модулей";
		Application->CreateForm(__classid(TMainForm), &MainForm);
		Application->CreateForm(__classid(TAboutProgramPanel), &AboutProgramPanel);
		Application->CreateForm(__classid(TWaitingPanel), &WaitingPanel);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
