#ifndef __WLUSBAPI__
#define __WLUSBAPI__
#include <windows.h>
#include <wtypes.h>

// -----------------------------------------------------------------------------
// wlusbapi - "обертка" основной  dll, для работы с языками не
//            поддерживающими указатели (MSVB, NI LabView,....)
// -----------------------------------------------------------------------------
// версия библиотеки
#define  WE140LC_VERMAJOR                       2       // только одна цифра
#define  WE140LC_VERMINOR                       1               // только одна цифра
#define  WE140LC_CURRENT_VERSION    ((WE140LC_VERMAJOR<<16)|WE140LC_VERMINOR)


// -----------------------------------------------------------------------------
// ---------------------------------- E140 -------------------------------------
// -----------------------------------------------------------------------------
#pragma pack(4)
#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif
// возвращает текущую версию библиотеки
__declspec(dllexport) DWORD WINAPI WE140GetDllVersion(VOID);
// создание экземпляра необходимого класса
__declspec(dllexport) LPVOID WINAPI WE140CreateInstance(VOID);
//  возвращается дескриптор устройства
__declspec(dllexport) HANDLE WINAPI WE140GetModuleHandle(LPVOID lptr);
//  Откроем виртуальный слот для доступа к USB модулю
__declspec(dllexport) BOOL WINAPI WE140OpenLDevice(LPVOID lptr, WORD VirtualSlot);
//  Освободим текущий виртуальный слот
__declspec(dllexport) BOOL WINAPI WE140CloseLDevice(LPVOID lptr);
//  Освободим указатель на устройство
__declspec(dllexport) BOOL WINAPI WE140ReleaseLDevice(LPVOID lptr);
//  Чтение название модуля
__declspec(dllexport) BOOL WINAPI WE140GetModuleName(LPVOID lptr, BYTE *ModuleName);
//  функция выдачи строки с последней ошибкой выполнения интерфейсной функции
__declspec(dllexport)  INT WINAPI WE140GetLastErrorString(LPVOID lptr, LPTSTR lpBuffer, DWORD nSize);
//  возвращает версию биоса
__declspec(dllexport) BOOL WINAPI WE140GET_LBIOS_VERSION(LPVOID lptr, DWORD *LbiosVersion);
// получение текущих параметров работы АЦП
__declspec(dllexport) BOOL WINAPI WE140GET_CUR_ADC_PARS(LPVOID lptr, struct ADC_PARS_E154 *am);
//  заполнение требуемых параметров работы АЦП
__declspec(dllexport) BOOL WINAPI WE140FILL_ADC_PARS(LPVOID lptr, struct ADC_PARS_E154 *am);
//  запуск работы АЦП
__declspec(dllexport) BOOL WINAPI WE140START_ADC(LPVOID lptr);
//  Останов АЦП
__declspec(dllexport) BOOL WINAPI WE140STOP_ADC(LPVOID lptr);
//  ввод кадра отсчетов с АЦП модуля
__declspec(dllexport) BOOL WINAPI WE140ADC_KADR(LPVOID lptr, SHORT *Data);
//  однократный ввод с заданного логического канала АЦП модуля
//__declspec(dllexport) BOOL WINAPI WE154ADC_SAMPLE(LPVOID lptr, SHORT *Data, WORD Channel)
//__declspec(dllexport) double WINAPI LV_SAMPLE_E154(DWORD Channel, DWORD *error);
__declspec(dllexport) double WINAPI LV_SAMPLE_E140(DWORD Channel, DWORD UseCalibr, DWORD *error);
//  потоковое чтение данных с АЦП модуля
//__declspec(dllexport) BOOL WINAPI WE154ReadData(LPVOID lptr, SHORT *lpBuffer, LPDWORD nNumberOfWordsToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
__declspec(dllexport) BOOL WINAPI WE140ReadData(LPVOID lptr, IO_REQUEST_LUSBAPI *ReadRequest);
//  однократный вывод на заданный канал ЦАП
//__declspec(dllexport) BOOL WINAPI WE154DAC_SAMPLE(LPVOID lptr, WORD *Data, WORD Channel);
__declspec(dllexport) BOOL WINAPI LV_OUTDA_E140(double value, int dac_number);
//  функция разрешения выходных линий внешнего цифрового разъёма
__declspec(dllexport) BOOL WINAPI WE140ENABLE_TTL_OUT(LPVOID lptr, BOOL flag);
//   функция чтения входных линии внешнего цифрового разъёма
__declspec(dllexport) BOOL WINAPI WE140TTL_IN(LPVOID lptr, WORD *TtlIn);
//  функция вывода на выходные линии внешнего цифрового разъёма
__declspec(dllexport) BOOL WINAPI WE140TTL_OUT(LPVOID lptr, WORD TtlOut);
//  разрешение/запрещение режима записи в ППЗУ модуля
__declspec(dllexport) BOOL WINAPI WE140ENABLE_FLASH_WRITE(LPVOID lptr, BOOL EnableFlashWrite);
//  получим служебную информацию о модуле из ППЗУ
__declspec(dllexport) BOOL WINAPI WE140GET_MODULE_DESCR(LPVOID lptr, struct MODULE_DESCRIPTION_E154 *md);
//  запишем служебную информацию о модуле в ППЗУ
__declspec(dllexport) BOOL WINAPI WE140SAVE_MODULE_DESCR(LPVOID lptr, struct MODULE_DESCRIPTION_E154 *md);
//  состояние аналогового питания узлов модуля при переходе шины usb в режим suspend
__declspec(dllexport) BOOL WINAPI WE140SetSuspendModeFlag(LPVOID lptr, BOOL SuspendModeFlag);
__declspec(dllexport) DWORD WINAPI LV_OpenE140(VOID);
__declspec(dllexport) DWORD WINAPI LV_CreateChannelE140(DWORD channel, DWORD gain, DWORD diff_mode);
__declspec(dllexport) DWORD WINAPI LV_CloseE140(VOID);
__declspec(dllexport) DWORD WINAPI LV_GETTTL_E140(DWORD *error);
__declspec(dllexport) BOOL WINAPI LV_SETTTL_E140(DWORD TtlOut);
__declspec(dllexport) BOOL WINAPI LV_SET_ADC_SYCHRO_READ_PARAMETERS(DWORD Nch, DWORD *Channels,
	float Rate, float AdcDelay, DWORD InputMode, DWORD SynchroAdType, DWORD SynchroAdMode, DWORD SynchroAdChannel,
   float SynchroAdPorog, float *SetRate, float *SetAdcDelay);
__declspec(dllexport) BOOL WINAPI LV_GET_ADC_SYNCHRO_ARRAY(DWORD NPoints, SHORT *Data, DWORD TimeOut);
__declspec(dllexport) BOOL WINAPI LV_START_REAL_TIME_SAMPLING(DWORD BufferSize, DWORD PacketSize);
__declspec(dllexport) BOOL WINAPI LV_GET_REAL_TIME_INDEX(DWORD *index, DWORD *overflow_flag, DWORD *user_index);
//__declspec(dllexport) BOOL WINAPI LV_GET_REAL_TIME_DATA(SHORT *Data, DWORD NPoints);
__declspec(dllexport) BOOL WINAPI LV_GET_REAL_TIME_DATA(SHORT *Data, DWORD NPoints, DWORD WaitFlag, DWORD *ReadPoints,
					DWORD *ReadyToReadPoints, DWORD *overflow, DWORD TimeOut);

__declspec(dllexport) BOOL WINAPI LV_STOP_REAL_TIME_SAMPLING(void);
__declspec(dllexport) BOOL WINAPI WE140D_FLASH_ARRAY(LPVOID lptr, unsigned char *ptr);
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
