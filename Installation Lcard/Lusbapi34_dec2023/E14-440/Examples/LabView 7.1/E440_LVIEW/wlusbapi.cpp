#include <windows.h>
#include "lusbapi.h"
#include "wlusbapi.h"
#pragma argsused

static int TtlOutEnabled;

static ILE440 *pModule;
// ���������� ����������
static HANDLE ModuleHandle;
// �������� ������
static char ModuleName[7];
// �������� ������ ���� USB
static BYTE UsbSpeed;
// ��������� � ������ ����������� � ������
static MODULE_DESCRIPTION_E440 E440DescriptionStruct;
// ��������� ���������� ������ ��� ������
static ADC_PARS_E440 ap;

DWORD WINAPI ServiceRealTImeReadThread(PVOID /*Context*/);

//BOOL DescriptorReadFlag;
//MODULE_DESCRIPTION_E440 E440DescriptionStruct;

BOOL ProcessOnePoint(SHORT source, double *dest, DWORD channel, BOOL calibr, BOOL volt)
{
	double K_offset, K_scale;
	int gain_index;
	MODULE_DESCRIPTION_E440 md;
	double ScaleGain[4]={10.0/8000., 2.5/8000., 0.625/8000., 0.15625/8000.};

   // ���������� ������������
	gain_index=(channel >> 6) & 3;
	if(calibr == FALSE)
	{
		// ��� ����������
		if(volt == FALSE)
         	{
            // ���� ���, ��� ����������
            K_offset=0;
            K_scale=1.0;
            }
           else
           	{
            // ������, ��� ����������
            K_offset=0;
            K_scale=ScaleGain[gain_index];
            }
         }
        else
      	{
         // � �����������
         if(volt == FALSE)
         	{
            // ���� ���, � �����������
            K_offset=E440DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale=E440DescriptionStruct.Adc.ScaleCalibration[gain_index];
            }
           else
           	{
            // ������, � �����������
            K_offset=E440DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale=E440DescriptionStruct.Adc.ScaleCalibration[gain_index]*ScaleGain[gain_index];
            }
         }
    //  }

   *dest=(source+K_offset)*K_scale;

	// ��� ������ :)))))
	return TRUE;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
    return 1;
}
//------------------------------------------------------------------------
// ���������� ������� ������ ����������
//------------------------------------------------------------------------
__declspec(dllexport) DWORD WINAPI WE440GetDllVersion(VOID)
{
        return GetDllVersion();
}
//------------------------------------------------------------------------
// ���������� ������� ������ ����������
//------------------------------------------------------------------------
void BadExit(void);

__declspec(dllexport) DWORD WINAPI LV_OpenE440(VOID)
{
int i;
   TtlOutEnabled=0;

	BadExit();

	// ��������� �������� ��������� �� ���������
	pModule = static_cast<ILE440 *>(CreateLInstance("e440"));
	if(!pModule) { BadExit(); return 0; }

	// ��������� ���������� ������ E14-1440 � ������ WORD MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI ����������� ������
	for(i = 0x0; i < MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI; i++) if(pModule->OpenLDevice(i)) break;
	// ���-������ ����������?
	if(i == MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI) { BadExit(); return 0; }

	// ��������� ��������� ���������� ����������
	ModuleHandle = pModule->GetModuleHandle();
	if(ModuleHandle == INVALID_HANDLE_VALUE) { BadExit(); return 0; }

	// ��������� �������� ������ � ������������ ����������� �����
	if(!pModule->GetModuleName(ModuleName)) { BadExit(); return 0; }

	// ��������, ��� ��� 'E14-440'
	if(strcmp(ModuleName, "E440")) { BadExit(); return 0; }

	// ��������� �������� �������� ������ ���� USB
	if(!pModule->GetUsbSpeed(&UsbSpeed)) { BadExit(); return 0; }

	// ��� LBIOS'� ������ �� ���������������� ������� ������� DLL ����������
	if(!pModule->LOAD_MODULE()) { BadExit(); return 0; }

	// �������� �������� ������
 	if(!pModule->TEST_MODULE()) { BadExit(); return 0; }

	// ������� ���������� �� ���� ������
	if(!pModule->GET_MODULE_DESCRIPTION(&E440DescriptionStruct)) { BadExit(); return 0; }

   return 1;
}

__declspec(dllexport) DWORD WINAPI LV_CloseE440(VOID)
{
BadExit();
return 0;
}

void BadExit(void)
{
	// ��������� ��������� ������
	if(pModule)
	{
		// ��������� ��������� ������
		pModule->ReleaseLInstance();

		// ������� ��������� �� ��������� ������
		pModule = NULL;
	}
}
// -----------------------------------------------------------------------------
// ---------------------------------- E440 -------------------------------------
// -----------------------------------------------------------------------------
//------------------------------------------------------------------------
// �������� ���������� ������������ ������
//------------------------------------------------------------------------
__declspec(dllexport) LPVOID WINAPI WE440CreateInstance(VOID)
{
    return CreateLInstance("E440");
}
//------------------------------------------------------------------------
//  ������������ ���������� ����������
//------------------------------------------------------------------------
__declspec(dllexport) HANDLE WINAPI WE440GetModuleHandle(LPVOID lptr)
{
    return ((ILE440*)lptr)->GetModuleHandle();
}
//------------------------------------------------------------------------
//  ������� ����������� ���� ��� ������� � USB ������
//------------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440OpenLDevice(LPVOID lptr, WORD VirtualSlot)
{
    return (BOOL)((ILE440*)lptr)->OpenLDevice(VirtualSlot);
}
//------------------------------------------------------------------------
//  ��������� ������� ����������� ����
//------------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440CloseLDevice(LPVOID lptr)
{
    return (BOOL)((ILE440*)lptr)->CloseLDevice();
}
// -----------------------------------------------------------------------
//  ��������� ��������� �� ����������
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440ReleaseLDevice(LPVOID lptr)
{
    return (BOOL)((ILE440*)lptr)->ReleaseLInstance();
}
// -----------------------------------------------------------------------
//  ������ �������� ������
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440GetModuleName(LPVOID lptr, BYTE *ModuleName)
{
    return (BOOL)((ILE440*)lptr)->GetModuleName(ModuleName);
}
// -----------------------------------------------------------------------
// ��������� ������� ���������� ������ ���
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440GET_CUR_ADC_PARS(LPVOID lptr, ADC_PARS_E440 *wam)
{
 if(((ILE440*)lptr)->GET_ADC_PARS(wam)) {
    return TRUE;
  } else return FALSE;
}
// -----------------------------------------------------------------------
//  ���������� ��������� ���������� ������ ���
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440FILL_ADC_PARS(LPVOID lptr, ADC_PARS_E440 *wam)
{
 if(((ILE440*)lptr)->SET_ADC_PARS(wam)) {
    return TRUE;
  } else return FALSE;
}
// -----------------------------------------------------------------------
//  ������ ������ ���
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440START_ADC(LPVOID lptr)
{
    return (BOOL)((ILE440*)lptr)->START_ADC();
}
// -----------------------------------------------------------------------
//  ������� ���
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440STOP_ADC(LPVOID lptr)
{
    return (BOOL)((ILE440*)lptr)->STOP_ADC();
}
// -----------------------------------------------------------------------
//  ���� ����� �������� � ��� ������
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440ADC_KADR(LPVOID lptr, SHORT *Data)
{
    return (BOOL)((ILE440*)lptr)->ADC_KADR(Data);
}
// -----------------------------------------------------------------------
//  ����������� ���� � ��������� ����������� ������ ��� ������
// -----------------------------------------------------------------------
static int test;
__declspec(dllexport) double WINAPI LV_SAMPLE_E440(DWORD Channel, DWORD UseCalibr, DWORD *error)
{
double f=0.0;
SHORT adc;
BOOL result;

result=pModule->ADC_SAMPLE(&adc, Channel);
*error=(result == TRUE) ? 1 : 0;
if(result == FALSE) return f;

result=ProcessOnePoint(adc, &f, Channel, UseCalibr, TRUE);
*error=(result == TRUE) ? 1 : 0;

return f;
}
// -----------------------------------------------------------------------
//  ��������� ������ ������ � ��� ������
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440ReadData(LPVOID lptr, IO_REQUEST_LUSBAPI *ReadRequest)
{

    return (BOOL)((ILE440*)lptr)->ReadData(ReadRequest);
}
// -----------------------------------------------------------------------
//  ����������� ����� �� �������� ����� ���
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI LV_OUTDA_E440(double value, int dac_number)
{
short val;

    val=value*2048./5.00;
    return (BOOL)pModule->DAC_SAMPLE(&val, dac_number);
}
// -----------------------------------------------------------------------
//  ������� ���������� �������� ����� �������� ��������� �������
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440ENABLE_TTL_OUT(LPVOID lptr, BOOL flag)
{
    return (BOOL)((ILE440*)lptr)->ENABLE_TTL_OUT(flag ? true : false);
}
// -----------------------------------------------------------------------
//   ������� ������ ������� ����� �������� ��������� �������
// -----------------------------------------------------------------------
__declspec(dllexport) DWORD WINAPI LV_GETTTL_E440(DWORD *error)
{
BOOL result;
unsigned short ttl;

result=pModule->TTL_IN(&ttl);
if(result == FALSE)
	{
   *error=0;
   return 0;
   }

*error=1;
return (DWORD) ttl;
}
// -----------------------------------------------------------------------
//  ������� ������ �� �������� ����� �������� ��������� �������
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI LV_SETTTL_E440(DWORD TtlOut)
{
BOOL result;

if(!TtlOutEnabled)
	{
   if(!pModule->ENABLE_TTL_OUT(true)) { return 0; }
   TtlOutEnabled=1;
   }

return (BOOL) pModule->TTL_OUT(TtlOut);
}
// -----------------------------------------------------------------------
//  ����������/���������� ������ ������ � ���� ������
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440ENABLE_FLASH_WRITE(LPVOID lptr, BOOL EnableFlashWrite)
{
    return (BOOL)((ILE440*)lptr)->ENABLE_FLASH_WRITE(EnableFlashWrite ? true : false);
}

// -----------------------------------------------------------------------
//  ������� ��������� ���������� � ������ �� ����
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440GET_MODULE_DESCR(LPVOID lptr, MODULE_DESCRIPTION_E440 *wmd)
{
 if(((ILE440*)lptr)->GET_MODULE_DESCRIPTION(wmd)) {
    return TRUE;
 } else return FALSE;
}
// -----------------------------------------------------------------------
//  ������� ��������� ���������� � ������ � ����
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE440SAVE_MODULE_DESCR(LPVOID lptr, MODULE_DESCRIPTION_E440 *wmd)
{
 return (BOOL)((ILE440*)lptr)->SAVE_MODULE_DESCRIPTION(wmd);
}

//------------------------------------------------------------------------
// ���������� ������� ������ ����������
//------------------------------------------------------------------------
__declspec(dllexport) DWORD WINAPI LV_CreateChannelE440(DWORD channel, DWORD gain, DWORD diff_mode)
{
return (channel & 0x1F) | ((gain & 3) << 6) | ((diff_mode & 1) << 5);
}


// -----------------------------------------------------------------------
//  ��������� ���������� ����������� ����� � ����
// -----------------------------------------------------------------------
// Nch - ����� �������� ������� (�� 1 �� 128)
// Channels - ������ � �������� �������
// Rate ������� ������ ������� ��� � ��
// AdcDelay - ����� �������������� ��� (������������ ��������), � ��
// InputMode, SynchroAdType, SynchroAdMode, SynchroAdChanne ��������� ������������� (�������� ��������)
// SynchroAdPorog ����� ������������ � �������
// SetRate ������� �������������� ������� ������ ������� ��� � ��
// SetAdcDelay ������� ������������� ������������ �������� (��)
__declspec(dllexport) BOOL WINAPI LV_SET_ADC_SYCHRO_READ_PARAMETERS(DWORD Nch, DWORD *Channels,
	float Rate, float AdcDelay, DWORD InputMode, DWORD SynchroAdType, DWORD SynchroAdMode, DWORD SynchroAdChannel,
   float SynchroAdPorog, float *SetRate, float *SetAdcDelay)
{
BOOL result;
double f;
int i;
double adc_ms, rate_ms, inter_kadr_ms;

if(!Nch) return false;

// ������� ������� ��������� ������ ���
if(!pModule->GET_ADC_PARS(&ap)) return false;

// ��������� �������� ��������� ���
ap.InputMode = InputMode;
ap.SynchroAdType=SynchroAdType;
ap.SynchroAdMode=SynchroAdMode;
ap.SynchroAdChannel=SynchroAdChannel;
ap.SynchroAdPorog=SynchroAdPorog*8000./ADC_INPUT_RANGES_E440[(SynchroAdChannel >> 6) & 3];

ap.ChannelsQuantity = Nch; 		// ���-�� �������� ������
for(i = 0x0; i < ap.ChannelsQuantity; i++) ap.ControlTable[i] = (WORD)Channels[i];

if(AdcDelay < 0.0025) ap.AdcRate=400.;
  else ap.AdcRate = 1./AdcDelay;					// ������� ������ ��� � ���

// ��������� ��� � ��
if(Rate > 400000) Rate=400000;
adc_ms=1./ap.AdcRate;
rate_ms=1000./Rate;
inter_kadr_ms=rate_ms-(Nch-1)*adc_ms;
if(inter_kadr_ms < 0) inter_kadr_ms=0;
ap.InterKadrDelay = inter_kadr_ms;			// ����������� �������� � ��

// ��������� ��������� ��������� ������ ��� � ������
if(!pModule->SET_ADC_PARS(&ap)) return false;

if(!pModule->GET_ADC_PARS(&ap)) return false;

*SetAdcDelay=1./ap.AdcRate;
*SetRate=ap.KadrRate*1000.;

return true;
}

DWORD WINAPI ServiceReadThread(PVOID /*Context*/);
// ������� ������ ��������� � ��������
void ShowThreadErrorMessage(void);

// ������������� ������ ����� ������
HANDLE hReadThread;
DWORD ReadTid;


// ���-�� ���������� �������� (������� 32) ��� �. ReadData()
DWORD DataStep = 32*1024;	// ����� �������� ������ �� 64 ��

// ����� ������
SHORT *AdcBuffer, *GlobalPtr;

// ������ ���������� ������ ������ ����� ������
bool IsReadThreadComplete;
// ����� ������ ��� ���������� ����� ������
WORD ReadThreadErrorNumber;

DWORD DataReadN, DataReadAll;;

// �������� �������-���������
DWORD Counter = 0x0, OldCounter = 0xFFFFFFFF;

// NPoints - ����� ��������
// Data - ������������� ������ � ������ ���
// TimeOut	������� ��, �� ���������� �������� ������� �������� ���������� ���� ���� ������ �� ���� ������� (��� ���� ���������� ������� ������)
// result 1 ��� ����������� ������, 0 �����
__declspec(dllexport) BOOL WINAPI LV_GET_ADC_SYNCHRO_ARRAY(DWORD NPoints, SHORT *Data, DWORD TimeOut)
{
double f;
int i;
int n_wait, n;
   // ���� ������ ���� �� ����� �� ��������� ����� ��������
	WORD RequestNumber;
	// ������������� ������� �� ���� �������
	HANDLE ReadEvent[2];
	// ������ OVERLAPPED �������� �� ���� ���������
	OVERLAPPED ReadOv[2];
	IO_REQUEST_LUSBAPI IoReq[2];

if(!ap.ChannelsQuantity ) return false;
if(!NPoints) return false;

if(!TimeOut) n_wait=250;	// 5 sec
  else n_wait=TimeOut/20;

// ������� ������ ���������� ������ ����� ������
IsReadThreadComplete = false;

// ���� ������ �� �������� ��� ����� ������
AdcBuffer = NULL;

// ���� �� ������ ����� ����� ������
hReadThread = NULL;

// ������� ���� ������ ������ ����� ������
ReadThreadErrorNumber = 0x0;

DataReadN=0;
DataReadAll=NPoints*ap.ChannelsQuantity;
GlobalPtr=Data;

if(DataReadAll < 128*1024)
	{
   // ������� ����� ����� ������� 64 ������ (32 �����)
   n=DataReadAll/32;
   if(DataReadAll % 32) n++;
   n *= 32;

	AdcBuffer = new SHORT[n];
	if(!AdcBuffer) return false;

	// ��������� ������ ��� � ������������ ������� USB-����� ������ ������
	if(!pModule->STOP_ADC()) { free(AdcBuffer); return false; }

	// ��������� ����������� ��� ����� ������ ���������
	// ������ �������
	ReadEvent[0] = CreateEvent(NULL, FALSE , FALSE, NULL);
	// ������������� ��������� ���� OVERLAPPED
	ZeroMemory(&ReadOv[0], sizeof(OVERLAPPED)); ReadOv[0].hEvent = ReadEvent[0];
	// ��������� ��������� IoReq
	IoReq[0].Buffer = AdcBuffer;
	IoReq[0].NumberOfWordsToPass = n;
	IoReq[0].NumberOfWordsPassed = 0x0;
	IoReq[0].Overlapped = &ReadOv[0];
	IoReq[0].TimeOut = n/ap.AdcRate + 10000;

	// ������ ��������������� ������ �� ���� ������
	RequestNumber = 0x0;
	if(!pModule->ReadData(&IoReq[RequestNumber])) { CloseHandle(ReadEvent[0]); CloseHandle(ReadEvent[1]);  return 0x0; }

	// �������� ���
	if(pModule->START_ADC())
	{
	// ��� ���������� �������� ����� ���������� ������ ������
	if(WaitForSingleObject(ReadEvent[0], IoReq[0].TimeOut) == WAIT_TIMEOUT) { ReadThreadErrorNumber = 0x3; }
     else
       memcpy(Data, AdcBuffer, DataReadAll*2);
	}
	else { ReadThreadErrorNumber = 0x6; }

	// ��������� ������ ���
	if(!pModule->STOP_ADC()) ReadThreadErrorNumber = 0x1;
	// ������ �������� ������������� ����������� ������ �� ���� ������
	if(!CancelIo(pModule->GetModuleHandle())) { ReadThreadErrorNumber = 0x7; }
	// ��������� ��� �������������� �������
	for(i = 0x0; i < 0x2; i++) CloseHandle(ReadEvent[i]);
	// ��������� ��������
  //	Sleep(100);
	// ��������� ������ ���������� ������ ������ ����� ������
	// ������ ����� �������� �������� �� ������
   free(AdcBuffer);

   return (ReadThreadErrorNumber) ? false :  true;
   }

// ������� ������ ��� �����
AdcBuffer = new SHORT[2*DataStep];
if(!AdcBuffer) return false;

// ������ � ��������� ����� ����� ������
hReadThread = CreateThread(0, 0x2000, ServiceReadThread, 0, 0, &ReadTid);
if(!hReadThread) { free(AdcBuffer); return false; }


// ���� ������ ���������� ������ � �������� ��������� ������ ����������
for(i=0; i < n_wait && !IsReadThreadComplete; i++)
	{
		//if(OldCounter != Counter) { printf(" Counter %3u from %3u\r", Counter, NDataBlock); OldCounter = Counter; }
		//else
      Sleep(20);
	}

// ��� ��������� ������ ������ ����� ������
WaitForSingleObject(hReadThread, INFINITE);

if(ReadThreadErrorNumber) { free(AdcBuffer); return false; }

free(AdcBuffer);

return true;
}



//------------------------------------------------------------------------
// �����, � ������� �������������� ���� ������
//------------------------------------------------------------------------
DWORD WINAPI ServiceReadThread(PVOID /*Context*/)
{
	WORD RequestNumber;
	DWORD i;
	// ������������� ������� �� ���� �������
	HANDLE ReadEvent[2];
	// ������ OVERLAPPED �������� �� ���� ���������
	OVERLAPPED ReadOv[2];
	IO_REQUEST_LUSBAPI IoReq[2];

	// ��������� ������ ��� � ������������ ������� USB-����� ������ ������
	if(!pModule->STOP_ADC()) { ReadThreadErrorNumber = 0x1; IsReadThreadComplete = true; return 0x0; }

	// ��������� ����������� ��� ����� ������ ���������
	for(i = 0x0; i < 0x2; i++)
	{
		// ������ �������
		ReadEvent[i] = CreateEvent(NULL, FALSE , FALSE, NULL);
		// ������������� ��������� ���� OVERLAPPED
		ZeroMemory(&ReadOv[i], sizeof(OVERLAPPED)); ReadOv[i].hEvent = ReadEvent[i];
		// ��������� ��������� IoReq
		IoReq[i].Buffer = AdcBuffer + i*DataStep;
		IoReq[i].NumberOfWordsToPass = DataStep;
		IoReq[i].NumberOfWordsPassed = 0x0;
		IoReq[i].Overlapped = &ReadOv[i];
		IoReq[i].TimeOut = DataStep/ap.AdcRate + 10000;
	}

	// ������ ��������������� ������ �� ���� ������
	RequestNumber = 0x0;
	if(!pModule->ReadData(&IoReq[RequestNumber])) { CloseHandle(ReadEvent[0]); CloseHandle(ReadEvent[1]); ReadThreadErrorNumber = 0x2; IsReadThreadComplete = true; return 0x0; }

	// �������� ���
	if(pModule->START_ADC())
	{
		// ���� ����� ������
		for(;;)
		{
			// ������� ������ �� ��������� ������ ������
			RequestNumber ^= 0x1;
			if(!pModule->ReadData(&IoReq[RequestNumber])) { ReadThreadErrorNumber = 0x2; break; }

			// ��� ���������� �������� ����� ���������� ������ ������
			if(WaitForSingleObject(ReadEvent[RequestNumber^0x1], IoReq[RequestNumber^0x1].TimeOut) == WAIT_TIMEOUT) { ReadThreadErrorNumber = 0x3; break; }

         if((DataReadAll-DataReadN) < DataStep)
         	{
            // ��������� ������ ������
            memcpy(&GlobalPtr[DataReadN], IoReq[RequestNumber^0x1].Buffer, 2*(DataReadAll-DataReadN));
            break;
            }
           else
           	{
            memcpy(&GlobalPtr[DataReadAll], IoReq[RequestNumber^0x1].Buffer, 2*DataStep);
            DataReadN += DataStep;
            }
			// ������� ���������� ������ ������ � ����
			if(ReadThreadErrorNumber) break;
//			else if(kbhit()) { ReadThreadErrorNumber = 0x5; break; }
			else Sleep(20);
			Counter++;
		}

	}
	else { ReadThreadErrorNumber = 0x6; }

	// ��������� ������ ���
	if(!pModule->STOP_ADC()) ReadThreadErrorNumber = 0x1;
	// ������ �������� ������������� ����������� ������ �� ���� ������
	if(!CancelIo(pModule->GetModuleHandle())) { ReadThreadErrorNumber = 0x7; }
	// ��������� ��� �������������� �������
	for(i = 0x0; i < 0x2; i++) CloseHandle(ReadEvent[i]);
	// ��������� ��������
	Sleep(100);
	// ��������� ������ ���������� ������ ������ ����� ������
	IsReadThreadComplete = true;
	// ������ ����� �������� �������� �� ������
	return 0x0;
}


int GlobalPacketSize;
SHORT *RealTimePtr;
int RealTimeReadIndex;
DWORD RealBufferSize;
static int StopEvent;
static int OverflowFlag;
static int UserIndex;

// BufferSize - ������ ������ � ���-������� ������, � ������� ����� ���������� ������ �� ������ (����� ���� ���� 1 ��, ���� �� ��� �������)
// PacketSize - ������ ������ ������� � �����, ���������� � ������� ����� ������ ����� ��������� �������� �� PacketSize ����
// ������ ������������ � FIFO �������� RealBufferSize ������� ��  GlobalPacketSize
// ������� ��������� ��������� ������ - RealTimeReadIndex
// ������� ��������� ��������� ����������  UserIndex
__declspec(dllexport) BOOL WINAPI LV_START_REAL_TIME_SAMPLING(DWORD BufferSize, DWORD PacketSize)
{
double f;
int i;
int n_wait, n;

// ���� ������ ���� �� ����� �� ��������� ����� ��������
WORD RequestNumber;

// ������������� ������� �� ���� �������
HANDLE ReadEvent[2];

// ������ OVERLAPPED �������� �� ���� ���������
OVERLAPPED ReadOv[2];
IO_REQUEST_LUSBAPI IoReq[2];

if(!ap.ChannelsQuantity ) return false;
if(BufferSize < 1024  || (BufferSize & 1)) return false;
if(PacketSize > BufferSize/2) PacketSize=BufferSize/2;
if(PacketSize < 32) PacketSize=32;
if(PacketSize % 32) { PacketSize=(PacketSize/32); PacketSize=(PacketSize+1)*32; }

// ������� ������ ���������� ������ ����� ������
IsReadThreadComplete = false;

// ���� ������ �� �������� ��� ����� ������
RealTimeReadIndex=0;
StopEvent=0;
GlobalPacketSize=PacketSize;
RealBufferSize=BufferSize;
AdcBuffer = NULL;
UserIndex=0;

// ���� �� ������ ����� ����� ������
hReadThread = NULL;

// ������� ���� ������ ������ ����� ������
ReadThreadErrorNumber = 0x0;

// ������� ������ ��� �����
AdcBuffer = new SHORT[2*PacketSize];
if(!AdcBuffer) return false;

RealTimePtr = new SHORT[RealBufferSize];
if(!RealTimePtr) { free(AdcBuffer); return false; }

// ������ � ��������� ����� ����� ������
hReadThread = CreateThread(0, 0x2000, ServiceRealTImeReadThread, 0, 0, &ReadTid);
//SetThreadPriority(hReadThread,  THREAD_PRIORITY_TIME_CRITICAL);
if(!hReadThread) { free(AdcBuffer); return false; }

if(ReadThreadErrorNumber) return false;

return true;
}

__declspec(dllexport) BOOL WINAPI LV_STOP_REAL_TIME_SAMPLING(void)
{
StopEvent=1;

for(;!IsReadThreadComplete;)
	{
      Sleep(20);
	}

// ��� ��������� ������ ������ ����� ������
WaitForSingleObject(hReadThread, INFINITE);

free(AdcBuffer); free(RealTimePtr);
AdcBuffer=NULL;
RealTimePtr=NULL;

if(ReadThreadErrorNumber) return false;

return true;
}


// index ���������� ������� ��������� ������������� ������
// overflow_flag ��������� �������� ��������, ��� ��������� ������������ ������
__declspec(dllexport) BOOL WINAPI LV_GET_REAL_TIME_INDEX(DWORD *index, DWORD *overflow_flag, DWORD *user_index)
{

*overflow_flag=OverflowFlag;
*index=RealTimeReadIndex;
*user_index=UserIndex;

if(ReadThreadErrorNumber) return false;

return true;
}

static int cnt=0;

// Data ������ � ������� ����� ���������� ������
// NPoints ������� �������� �������������
__declspec(dllexport) BOOL WINAPI LV_GET_REAL_TIME_DATA(SHORT *Data, DWORD NPoints)
{
DWORD n;
DWORD i;


// ������ ������������ � FIFO �������� RealBufferSize ������� ��  GlobalPacketSize
// ������� ��������� ��������� ������ - RealTimeReadIndex
// ������� ��������� ��������� ����������  UserIndex
if((UserIndex+NPoints) <= RealBufferSize)
         	{
            memcpy(Data, &RealTimePtr[UserIndex], 2*NPoints);
            UserIndex += NPoints;
            if(UserIndex >= RealBufferSize) UserIndex=0;
//            for(i=0, ++cnt; i < NPoints; i++) Data[i]=cnt;
            }
           else
           	{
            // 1. ������� ����� �� ����� ������
            n=RealBufferSize-UserIndex;
            memcpy(Data, &RealTimePtr[UserIndex], 2*n);

            UserIndex=0;

            // ��������� �����
            memcpy(&Data[n], &RealTimePtr[UserIndex], 2*(NPoints-n));
            UserIndex += (NPoints-n);
            }

            //cnt++;

return true;
}



//------------------------------------------------------------------------
// �����, � ������� �������������� ���� ������
//------------------------------------------------------------------------
DWORD WINAPI ServiceRealTImeReadThread(PVOID /*Context*/)
{
	WORD RequestNumber;
	DWORD i;
	// ������������� ������� �� ���� �������
	HANDLE ReadEvent[2];
	// ������ OVERLAPPED �������� �� ���� ���������
	OVERLAPPED ReadOv[2];
	IO_REQUEST_LUSBAPI IoReq[2];
   DWORD n;DWORD OldPointer;

	// ��������� ������ ��� � ������������ ������� USB-����� ������ ������
	if(!pModule->STOP_ADC()) { ReadThreadErrorNumber = 0x1; IsReadThreadComplete = true; return 0x0; }

	// ��������� ����������� ��� ����� ������ ���������
	for(i = 0x0; i < 0x2; i++)
	{
		// ������ �������
		ReadEvent[i] = CreateEvent(NULL, FALSE , FALSE, NULL);
		// ������������� ��������� ���� OVERLAPPED
		ZeroMemory(&ReadOv[i], sizeof(OVERLAPPED)); ReadOv[i].hEvent = ReadEvent[i];
		// ��������� ��������� IoReq
		IoReq[i].Buffer = AdcBuffer + i*GlobalPacketSize;
		IoReq[i].NumberOfWordsToPass = GlobalPacketSize;
		IoReq[i].NumberOfWordsPassed = 0x0;
		IoReq[i].Overlapped = &ReadOv[i];
		IoReq[i].TimeOut = 1000;
	}

	// ������ ��������������� ������ �� ���� ������
	RequestNumber = 0x0;
	if(!pModule->ReadData(&IoReq[RequestNumber])) { CloseHandle(ReadEvent[0]); CloseHandle(ReadEvent[1]); ReadThreadErrorNumber = 0x2; IsReadThreadComplete = true; return 0x0; }

	// �������� ���
	if(pModule->START_ADC())
	{
		// ���� ����� ������
		for(;;)
		{
			// ������� ������ �� ��������� ������ ������
			RequestNumber ^= 0x1;
			if(!pModule->ReadData(&IoReq[RequestNumber])) { ReadThreadErrorNumber = 0x2; break; }

			// ��� ���������� �������� ����� ���������� ������ ������
         for(;;)
         	{
            if(StopEvent) break;
				if(WaitForSingleObject(ReadEvent[RequestNumber^0x1], IoReq[RequestNumber^0x1].TimeOut) == WAIT_TIMEOUT) continue;
            	else break;
            }
         if(StopEvent) break;

			// ������ ������������ � FIFO �������� RealBufferSize ������� ��  GlobalPacketSize
			// ������� ��������� ��������� ������ - RealTimeReadIndex
			// ������� ��������� ��������� ����������  UserIndex
         OldPointer=RealTimeReadIndex;
         if((RealTimeReadIndex+GlobalPacketSize) <= RealBufferSize)
         	{
            memcpy(&RealTimePtr[RealTimeReadIndex], IoReq[RequestNumber^0x1].Buffer, 2*GlobalPacketSize);
            RealTimeReadIndex += GlobalPacketSize;

	         // �������� �� ������������
   	      if(UserIndex > OldPointer && UserIndex <= RealTimeReadIndex)   OverflowFlag=1;

            if(RealTimeReadIndex == RealBufferSize) RealTimeReadIndex=0;
            }
           else
           	{
            // 1. ������� ����� �� ����� ������
            n=RealBufferSize-RealTimeReadIndex;
            memcpy(&RealTimePtr[RealTimeReadIndex], IoReq[RequestNumber^0x1].Buffer, 2*n);

	         // �������� �� ������������
            RealTimeReadIndex += n;
   	      if(UserIndex > OldPointer && UserIndex <= RealTimeReadIndex)   OverflowFlag=1;

            RealTimeReadIndex=0;

            // ��������� �����
            memcpy(&RealTimePtr[RealTimeReadIndex], &IoReq[RequestNumber^0x1].Buffer[n], 2*(GlobalPacketSize-n));
            RealTimeReadIndex += (GlobalPacketSize-n);
            }


			if(ReadThreadErrorNumber) break;
				else Sleep(20);
		}

	}
	else { ReadThreadErrorNumber = 0x6; }

	// ��������� ������ ���
	if(!pModule->STOP_ADC()) ReadThreadErrorNumber = 0x1;
	// ������ �������� ������������� ����������� ������ �� ���� ������
	if(!CancelIo(pModule->GetModuleHandle())) { ReadThreadErrorNumber = 0x7; }
	// ��������� ��� �������������� �������
	for(i = 0x0; i < 0x2; i++) CloseHandle(ReadEvent[i]);
	// ��������� ��������
	Sleep(20);
	// ��������� ������ ���������� ������ ������ ����� ������
	IsReadThreadComplete = true;
	// ������ ����� �������� �������� �� ������
	return 0x0;
}



__declspec(dllexport) BOOL WINAPI WEProcessOnePoint(SHORT src, double *dest, DWORD channel, DWORD calibr, DWORD volt)
{
    return (BOOL)ProcessOnePoint(src, dest, channel, calibr, volt);
}


__declspec(dllexport) BOOL WINAPI WEDAC_SAMPLE_VOLT(double DacData, DWORD calibr, DWORD dac_number)
{
short val;

val=DacData*2048./5.00;
if(calibr == TRUE) val=(val+E440DescriptionStruct.Dac.OffsetCalibration[dac_number])*E440DescriptionStruct.Dac.ScaleCalibration[dac_number];

return (BOOL)pModule->DAC_SAMPLE(&val, dac_number);
}

// -----------------------------------------------------------------------------
// �������������� ����� ��� � ������ � ������ ����������
// -----------------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WEProcessArray(SHORT *src, double *dest, DWORD size, DWORD calibr, DWORD volt)
{
	double K_offset[MAX_CONTROL_TABLE_LENGTH_E440], K_scale[MAX_CONTROL_TABLE_LENGTH_E440];
	DWORD i, j, nch, gain_index;
	double ScaleGain[4]={10.0/8000., 2.5/8000., 0.625/8000., 0.15625/8000.};

	nch=ap.ChannelsQuantity;
   if(!nch) { return FALSE; }

   // ���������� ������������
   for(i=0; i < nch; i++)
   	{
      gain_index=(ap.ControlTable[i] >> 6) & 3;
      if(calibr == FALSE)
      	{
         // ��� ����������
         if(volt == FALSE)
         	{
            // ���� ���, ��� ����������
            K_offset[i]=0;
            K_scale[i]=1.0;
            }
           else
           	{
            // ������, ��� ����������
            K_offset[i]=0;
            K_scale[i]=ScaleGain[gain_index];
            }
         }
        else
      	{
         // � �����������
         if(volt == FALSE)
         	{
            // ���� ���, � �����������
            K_offset[i]=E440DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale[i]=E440DescriptionStruct.Adc.ScaleCalibration[gain_index];
            }
           else
           	{
            // ������, � �����������
            K_offset[i]=E440DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale[i]=E440DescriptionStruct.Adc.ScaleCalibration[gain_index]*ScaleGain[gain_index];
            }
         }
      }

   for(i=j=0; i < size; i++)
   	{
      *dest++=((*src++)+K_offset[j])*K_scale[j];
      j++;
      if(j >= nch) j=0;
      }

	// ��� ������ :)))))
	return TRUE;
}

