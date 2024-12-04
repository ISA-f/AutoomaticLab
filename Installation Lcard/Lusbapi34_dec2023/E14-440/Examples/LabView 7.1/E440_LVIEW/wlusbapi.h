#ifndef __WLUSBAPI__
#define __WLUSBAPI__
#include <windows.h>
#include <wtypes.h>

// -----------------------------------------------------------------------------
// wlusbapi - "�������" ��������  dll, ��� ������ � ������� ��
//            ��������������� ��������� (MSVB, NI LabView,....)
// -----------------------------------------------------------------------------
// ������ ����������
#define  WE140LC_VERMAJOR                       2       // ������ ���� �����
#define  WE140LC_VERMINOR                       1               // ������ ���� �����
#define  WE140LC_CURRENT_VERSION    ((WE140LC_VERMAJOR<<16)|WE140LC_VERMINOR)


// -----------------------------------------------------------------------------
// ---------------------------------- E140 -------------------------------------
// -----------------------------------------------------------------------------
#pragma pack(4)
#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif
// ���������� ������� ������ ����������
__declspec(dllexport) DWORD WINAPI WE440GetDllVersion(VOID);
// �������� ���������� ������������ ������
__declspec(dllexport) LPVOID WINAPI WE440CreateInstance(VOID);
//  ������������ ���������� ����������
__declspec(dllexport) HANDLE WINAPI WE440GetModuleHandle(LPVOID lptr);
//  ������� ����������� ���� ��� ������� � USB ������
__declspec(dllexport) BOOL WINAPI WE440OpenLDevice(LPVOID lptr, WORD VirtualSlot);
//  ��������� ������� ����������� ����
__declspec(dllexport) BOOL WINAPI WE440CloseLDevice(LPVOID lptr);
//  ��������� ��������� �� ����������
__declspec(dllexport) BOOL WINAPI WE440ReleaseLDevice(LPVOID lptr);
//  ������ �������� ������
__declspec(dllexport) BOOL WINAPI WE440GetModuleName(LPVOID lptr, BYTE *ModuleName);
//  ������� ������ ������ � ��������� ������� ���������� ������������ �������
__declspec(dllexport)  INT WINAPI WE440GetLastErrorString(LPVOID lptr, LPTSTR lpBuffer, DWORD nSize);
//  ���������� ������ �����
__declspec(dllexport) BOOL WINAPI WE440GET_LBIOS_VERSION(LPVOID lptr, DWORD *LbiosVersion);
// ��������� ������� ���������� ������ ���
__declspec(dllexport) BOOL WINAPI WE440GET_CUR_ADC_PARS(LPVOID lptr, struct ADC_PARS_E154 *am);
//  ���������� ��������� ���������� ������ ���
__declspec(dllexport) BOOL WINAPI WE440FILL_ADC_PARS(LPVOID lptr, struct ADC_PARS_E154 *am);
//  ������ ������ ���
__declspec(dllexport) BOOL WINAPI WE440START_ADC(LPVOID lptr);
//  ������� ���
__declspec(dllexport) BOOL WINAPI WE440STOP_ADC(LPVOID lptr);
//  ���� ����� �������� � ��� ������
__declspec(dllexport) BOOL WINAPI WE440ADC_KADR(LPVOID lptr, SHORT *Data);
//  ����������� ���� � ��������� ����������� ������ ��� ������
//__declspec(dllexport) BOOL WINAPI WE154ADC_SAMPLE(LPVOID lptr, SHORT *Data, WORD Channel)
//__declspec(dllexport) double WINAPI LV_SAMPLE_E154(DWORD Channel, DWORD *error);
__declspec(dllexport) double WINAPI LV_SAMPLE_E440(DWORD Channel, DWORD UseCalibr, DWORD *error);
//  ��������� ������ ������ � ��� ������
//__declspec(dllexport) BOOL WINAPI WE154ReadData(LPVOID lptr, SHORT *lpBuffer, LPDWORD nNumberOfWordsToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
__declspec(dllexport) BOOL WINAPI WE440ReadData(LPVOID lptr, IO_REQUEST_LUSBAPI *ReadRequest);
//  ����������� ����� �� �������� ����� ���
//__declspec(dllexport) BOOL WINAPI WE154DAC_SAMPLE(LPVOID lptr, WORD *Data, WORD Channel);
__declspec(dllexport) BOOL WINAPI LV_OUTDA_E440(double value, int dac_number);
//  ������� ���������� �������� ����� �������� ��������� �������
__declspec(dllexport) BOOL WINAPI WE440ENABLE_TTL_OUT(LPVOID lptr, BOOL flag);
//   ������� ������ ������� ����� �������� ��������� �������
__declspec(dllexport) BOOL WINAPI WE440TTL_IN(LPVOID lptr, WORD *TtlIn);
//  ������� ������ �� �������� ����� �������� ��������� �������
__declspec(dllexport) BOOL WINAPI WE440TTL_OUT(LPVOID lptr, WORD TtlOut);
//  ����������/���������� ������ ������ � ���� ������
__declspec(dllexport) BOOL WINAPI WE440ENABLE_FLASH_WRITE(LPVOID lptr, BOOL EnableFlashWrite);
//  ������� ��������� ���������� � ������ �� ����
__declspec(dllexport) BOOL WINAPI WE440GET_MODULE_DESCR(LPVOID lptr, struct MODULE_DESCRIPTION_E154 *md);
//  ������� ��������� ���������� � ������ � ����
__declspec(dllexport) BOOL WINAPI WE440SAVE_MODULE_DESCR(LPVOID lptr, struct MODULE_DESCRIPTION_E154 *md);
//  ��������� ����������� ������� ����� ������ ��� �������� ���� usb � ����� suspend
__declspec(dllexport) BOOL WINAPI WE440SetSuspendModeFlag(LPVOID lptr, BOOL SuspendModeFlag);
__declspec(dllexport) DWORD WINAPI LV_OpenE440(VOID);
__declspec(dllexport) DWORD WINAPI LV_CreateChannelE440(DWORD channel, DWORD gain, DWORD diff_mode);
__declspec(dllexport) DWORD WINAPI LV_CloseE440(VOID);
__declspec(dllexport) DWORD WINAPI LV_GETTTL_E440(DWORD *error);
__declspec(dllexport) BOOL WINAPI LV_SETTTL_E440(DWORD TtlOut);
__declspec(dllexport) BOOL WINAPI LV_SET_ADC_SYCHRO_READ_PARAMETERS(DWORD Nch, DWORD *Channels,
	float Rate, float AdcDelay, DWORD InputMode, DWORD SynchroAdType, DWORD SynchroAdMode, DWORD SynchroAdChannel,
   float SynchroAdPorog, float *SetRate, float *SetAdcDelay);
__declspec(dllexport) BOOL WINAPI LV_GET_ADC_SYNCHRO_ARRAY(DWORD NPoints, SHORT *Data, DWORD TimeOut);
__declspec(dllexport) BOOL WINAPI LV_START_REAL_TIME_SAMPLING(DWORD BufferSize, DWORD PacketSize);
__declspec(dllexport) BOOL WINAPI LV_GET_REAL_TIME_INDEX(DWORD *index, DWORD *overflow_flag, DWORD *user_index);
__declspec(dllexport) BOOL WINAPI LV_GET_REAL_TIME_DATA(SHORT *Data, DWORD NPoints);
__declspec(dllexport) BOOL WINAPI LV_STOP_REAL_TIME_SAMPLING(void);
__declspec(dllexport) BOOL WINAPI WE440D_FLASH_ARRAY(LPVOID lptr, unsigned char *ptr);
__declspec(dllexport) BOOL WINAPI  WEREAD_FLASH_ARRAY(LPVOID lptr, unsigned char *ptr);
__declspec(dllexport) BOOL WINAPI WEProcessArray(SHORT *src, double *dest, DWORD size, DWORD calibr, DWORD volt);
__declspec(dllexport) BOOL WINAPI WEProcessOnePoint(SHORT src, double *dest, DWORD channel, DWORD calibr, DWORD volt);
__declspec(dllexport) BOOL WINAPI WEFIFO_STATUS(DWORD *FifoOverflowFlag, double *FifoMaxPercentLoad, DWORD *FifoSize, DWORD *MaxFifoBytesUsed);
__declspec(dllexport) BOOL WINAPI WE_GET_ARRAY(BYTE *Buffer, WORD Size, WORD Address);
__declspec(dllexport) BOOL WINAPI WE_PUT_ARRAY(BYTE *Buffer, WORD Size, WORD Address);
__declspec(dllexport) BOOL WINAPI WEDAC_SAMPLE_VOLT(double DacData, DWORD calibr, DWORD dac_number);

#ifdef __cplusplus
 }
#endif

#endif
