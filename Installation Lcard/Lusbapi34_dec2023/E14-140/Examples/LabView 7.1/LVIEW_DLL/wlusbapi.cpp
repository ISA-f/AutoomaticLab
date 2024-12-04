#include <windows.h>
#include "lusbapi.h"
#include "wlusbapi.h"
#pragma argsused

static ILE140 *pModule;
// дескриптор устройства
static HANDLE ModuleHandle;
// название модуля
static char ModuleName[7];
// скорость работы шины USB
static BYTE UsbSpeed;
// структура с полной информацией о модуле
static MODULE_DESCRIPTION_E140 E140DescriptionStruct;
// структура параметров работы АЦП модуля
static ADC_PARS_E140 ap;

DWORD WINAPI ServiceRealTImeReadThread(PVOID /*Context*/);

//BOOL DescriptorReadFlag;
//MODULE_DESCRIPTION_E140 E140DescriptionStruct;

BOOL ProcessOnePoint(SHORT source, double *dest, DWORD channel, BOOL calibr, BOOL volt)
{
	double K_offset, K_scale;
	int gain_index;
	MODULE_DESCRIPTION_E140 md;
	double ScaleGain[4]={10.0/8000., 2.5/8000., 0.625/8000., 0.15625/8000.};

   // подготовим коэффициенты
	gain_index=(channel >> 6) & 3;
	if(calibr == FALSE)
	{
		// без калибровки
		if(volt == FALSE)
         	{
            // коды АЦП, без калибровки
            K_offset=0;
            K_scale=1.0;
            }
           else
           	{
            // вольты, без калибровки
            K_offset=0;
            K_scale=ScaleGain[gain_index];
            }
         }
        else
      	{
         // с калибровкой
         if(volt == FALSE)
         	{
            // коды АЦП, с калибровкой
            K_offset=E140DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale=E140DescriptionStruct.Adc.ScaleCalibration[gain_index];
            }
           else
           	{
            // вольты, с калибровкой
            K_offset=E140DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale=E140DescriptionStruct.Adc.ScaleCalibration[gain_index]*ScaleGain[gain_index];
            }
         }
    //  }

   *dest=(source+K_offset)*K_scale;

	// все хорошо :)))))
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
// возвращает текущую версию библиотеки
//------------------------------------------------------------------------
__declspec(dllexport) DWORD WINAPI WE140GetDllVersion(VOID)
{
        return GetDllVersion();
}
//------------------------------------------------------------------------
// возвращает текущую версию библиотеки
//------------------------------------------------------------------------
void BadExit(void);

__declspec(dllexport) DWORD WINAPI LV_OpenE140(VOID)
{
int i;

	BadExit();

	// попробуем получить указатель на интерфейс
	pModule = static_cast<ILE140 *>(CreateLInstance("e140"));
	if(!pModule) { BadExit(); return 0; }

	// попробуем обнаружить модуль E14-1140 в первых WORD MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI виртуальных слотах
	for(i = 0x0; i < MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI; i++) if(pModule->OpenLDevice(i)) break;
	// что-нибудь обнаружили?
	if(i == MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI) { BadExit(); return 0; }

	// попробуем прочитать дескриптор устройства
	ModuleHandle = pModule->GetModuleHandle();
	if(ModuleHandle == INVALID_HANDLE_VALUE) { BadExit(); return 0; }

	// прочитаем название модуля в обнаруженном виртуальном слоте
	if(!pModule->GetModuleName(ModuleName)) { BadExit(); return 0; }

	// проверим, что это 'E14-140'
	if(strcmp(ModuleName, "E140")) { BadExit(); return 0; }

	// попробуем получить скорость работы шины USB
	if(!pModule->GetUsbSpeed(&UsbSpeed)) { BadExit(); return 0; }

	// получим информацию из ППЗУ модуля
	if(!pModule->GET_MODULE_DESCRIPTION(&E140DescriptionStruct)) { BadExit(); return 0; }

   return 1;
}

__declspec(dllexport) DWORD WINAPI LV_CloseE140(VOID)
{
BadExit();
return 0;
}

void BadExit(void)
{
	// подчищаем интерфейс модуля
	if(pModule)
	{
		// освободим интерфейс модуля
		pModule->ReleaseLInstance();

		// обнулим указатель на интерфейс модуля
		pModule = NULL;
	}
}
// -----------------------------------------------------------------------------
// ---------------------------------- E140 -------------------------------------
// -----------------------------------------------------------------------------
//------------------------------------------------------------------------
// создание экземпляра необходимого класса
//------------------------------------------------------------------------
__declspec(dllexport) LPVOID WINAPI WE140CreateInstance(VOID)
{
    return CreateLInstance("E140");
}
//------------------------------------------------------------------------
//  возвращается дескриптор устройства
//------------------------------------------------------------------------
__declspec(dllexport) HANDLE WINAPI WE140GetModuleHandle(LPVOID lptr)
{
    return ((ILE140*)lptr)->GetModuleHandle();
}
//------------------------------------------------------------------------
//  Откроем виртуальный слот для доступа к USB модулю
//------------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140OpenLDevice(LPVOID lptr, WORD VirtualSlot)
{
    return (BOOL)((ILE140*)lptr)->OpenLDevice(VirtualSlot);
}
//------------------------------------------------------------------------
//  Освободим текущий виртуальный слот
//------------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140CloseLDevice(LPVOID lptr)
{
    return (BOOL)((ILE140*)lptr)->CloseLDevice();
}
// -----------------------------------------------------------------------
//  Освободим указатель на устройство
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140ReleaseLDevice(LPVOID lptr)
{
    return (BOOL)((ILE140*)lptr)->ReleaseLInstance();
}
// -----------------------------------------------------------------------
//  Чтение название модуля
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140GetModuleName(LPVOID lptr, BYTE *ModuleName)
{
    return (BOOL)((ILE140*)lptr)->GetModuleName(ModuleName);
}
// -----------------------------------------------------------------------
// получение текущих параметров работы АЦП
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140GET_CUR_ADC_PARS(LPVOID lptr, ADC_PARS_E140 *wam)
{
 if(((ILE140*)lptr)->GET_ADC_PARS(wam)) {
    return TRUE;
  } else return FALSE;
}
// -----------------------------------------------------------------------
//  заполнение требуемых параметров работы АЦП
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140FILL_ADC_PARS(LPVOID lptr, ADC_PARS_E140 *wam)
{
 if(((ILE140*)lptr)->SET_ADC_PARS(wam)) {
    return TRUE;
  } else return FALSE;
}
// -----------------------------------------------------------------------
//  запуск работы АЦП
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140START_ADC(LPVOID lptr)
{
    return (BOOL)((ILE140*)lptr)->START_ADC();
}
// -----------------------------------------------------------------------
//  Останов АЦП
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140STOP_ADC(LPVOID lptr)
{
    return (BOOL)((ILE140*)lptr)->STOP_ADC();
}
// -----------------------------------------------------------------------
//  ввод кадра отсчетов с АЦП модуля
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140ADC_KADR(LPVOID lptr, SHORT *Data)
{
    return (BOOL)((ILE140*)lptr)->ADC_KADR(Data);
}
// -----------------------------------------------------------------------
//  однократный ввод с заданного логического канала АЦП модуля
// -----------------------------------------------------------------------
static int test;
__declspec(dllexport) double WINAPI LV_SAMPLE_E140(DWORD Channel, DWORD UseCalibr, DWORD *error)
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
//  потоковое чтение данных с АЦП модуля
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140ReadData(LPVOID lptr, IO_REQUEST_LUSBAPI *ReadRequest)
{

    return (BOOL)((ILE140*)lptr)->ReadData(ReadRequest);
}
// -----------------------------------------------------------------------
//  однократный вывод на заданный канал ЦАП
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI LV_OUTDA_E140(double value, int dac_number)
{
short val;

    val=value*2048./5.00;
    return (BOOL)pModule->DAC_SAMPLE(&val, dac_number);
}
// -----------------------------------------------------------------------
//  функция разрешения выходных линий внешнего цифрового разъёма
// -----------------------------------------------------------------------
static int TtlOutEnabled;
__declspec(dllexport) BOOL WINAPI WE140ENABLE_TTL_OUT(LPVOID lptr, BOOL flag)
{
    return (BOOL)((ILE140*)lptr)->ENABLE_TTL_OUT(flag ? true : false);
}
// -----------------------------------------------------------------------
//   функция чтения входных линии внешнего цифрового разъёма
// -----------------------------------------------------------------------
__declspec(dllexport) DWORD WINAPI LV_GETTTL_E140(DWORD *error)
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
//  функция вывода на выходные линии внешнего цифрового разъёма
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI LV_SETTTL_E140(DWORD TtlOut)
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
//  разрешение/запрещение режима записи в ППЗУ модуля
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140ENABLE_FLASH_WRITE(LPVOID lptr, BOOL EnableFlashWrite)
{
    return (BOOL)((ILE140*)lptr)->ENABLE_FLASH_WRITE(EnableFlashWrite ? true : false);
}

// -----------------------------------------------------------------------
//  получим служебную информацию о модуле из ППЗУ
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140GET_MODULE_DESCR(LPVOID lptr, MODULE_DESCRIPTION_E140 *wmd)
{
 if(((ILE140*)lptr)->GET_MODULE_DESCRIPTION(wmd)) {
    return TRUE;
 } else return FALSE;
}
// -----------------------------------------------------------------------
//  запишем служебную информацию о модуле в ППЗУ
// -----------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WE140SAVE_MODULE_DESCR(LPVOID lptr, MODULE_DESCRIPTION_E140 *wmd)
{
 return (BOOL)((ILE140*)lptr)->SAVE_MODULE_DESCRIPTION(wmd);
}

//------------------------------------------------------------------------
// возвращает текущую версию библиотеки
//------------------------------------------------------------------------
__declspec(dllexport) DWORD WINAPI LV_CreateChannelE140(DWORD channel, DWORD gain, DWORD diff_mode)
{
return (channel & 0x1F) | ((gain & 3) << 6) | ((diff_mode & 1) << 5);
}


// -----------------------------------------------------------------------
//  установка параметров синхронного ввода с АВЦП
// -----------------------------------------------------------------------
// Nch - число вводимых каналов (от 1 до 128)
// Channels - массив с номерами каналов
// Rate частота опроса каналов АЦП в Гц
// AdcDelay - время преобразования АЦП (межканальная задержка), в мс
// InputMode, SynchroAdType, SynchroAdMode, SynchroAdChanne параметры синхронизации (согласно описанию)
// SynchroAdPorog порог сихронизации в вольтах
// SetRate реально установленнрая частота опроса каналов АЦП в Гц
// SetAdcDelay реально установленная межканальная задержка (мс)
__declspec(dllexport) BOOL WINAPI LV_SET_ADC_SYCHRO_READ_PARAMETERS(DWORD Nch, DWORD *Channels,
	float Rate, float AdcDelay, DWORD InputMode, DWORD SynchroAdType, DWORD SynchroAdMode, DWORD SynchroAdChannel,
   float SynchroAdPorog, float *SetRate, float *SetAdcDelay)
{
BOOL result;
double f;
int i;
double adc_ms, rate_ms, inter_kadr_ms;

if(!Nch) return false;

// получим текущие параметры работы АЦП
if(!pModule->GET_ADC_PARS(&ap)) return false;

// установим желаемые параметры АЦП
ap.InputMode = InputMode;
ap.SynchroAdType=SynchroAdType;
ap.SynchroAdMode=SynchroAdMode;
ap.SynchroAdChannel=SynchroAdChannel;
ap.SynchroAdPorog=SynchroAdPorog*8000./ADC_INPUT_RANGES_E140[(SynchroAdChannel >> 6) & 3];

ap.ChannelsQuantity = Nch; 		// кол-во активных канала
for(i = 0x0; i < ap.ChannelsQuantity; i++) ap.ControlTable[i] = (WORD)Channels[i];

if(AdcDelay < 0.01) ap.AdcRate=100.;
  else ap.AdcRate = 1./AdcDelay;					// частота работы АЦП в кГц

// переведем все в мс
if(Rate > 100000) Rate=100000;
adc_ms=1./ap.AdcRate;
rate_ms=1000./Rate;
inter_kadr_ms=rate_ms-(Nch-1)*adc_ms;
if(inter_kadr_ms < 0) inter_kadr_ms=0;
ap.InterKadrDelay = inter_kadr_ms;			// межкадровая задержка в мс

// передадим требуемые параметры работы АЦП в модуль
if(!pModule->SET_ADC_PARS(&ap)) return false;

if(!pModule->GET_ADC_PARS(&ap)) return false;

*SetAdcDelay=1./ap.AdcRate;
*SetRate=ap.KadrRate*1000.;

return true;
}

DWORD WINAPI ServiceReadThread(PVOID /*Context*/);
// функция вывода сообщений с ошибками
void ShowThreadErrorMessage(void);

// идентификатор потока сбора данных
HANDLE hReadThread;
DWORD ReadTid;


// кол-во получаемых отсчетов (кратное 32) для Ф. ReadData()
DWORD DataStep = 32*1024;	// будем получать данные по 64 кБ

// буфер данных
SHORT *AdcBuffer, *GlobalPtr;

// флажок завершения работы потока сбора данных
bool IsReadThreadComplete;
// номер ошибки при выполнении сбора данных
WORD ReadThreadErrorNumber;

DWORD DataReadN, DataReadAll;;

// экранный счетчик-индикатор
DWORD Counter = 0x0, OldCounter = 0xFFFFFFFF;


int GlobalPacketSize;
SHORT *RealTimePtr;
int RealTimeReadIndex;
DWORD RealBufferSize;
static int StopEvent;
static int OverflowFlag;
static int UserIndex;

// BufferSize - размер буфера в дву-байтных словах, в который будут помещаться данные по кольцу (может быть хоть 1 Гб, лишь бы ОЗУ хватило)
// PacketSize - размер одного запроса к плате, фактически в главный буфер данные будут приходить порциями по PacketSize слов
// Данные записываются в FIFO размером RealBufferSize пачками по  GlobalPacketSize
// текущее положение указателя записи - RealTimeReadIndex
// текущее положения указателя считывания  UserIndex
__declspec(dllexport) BOOL WINAPI LV_START_REAL_TIME_SAMPLING(DWORD BufferSize, DWORD PacketSize)
{
if(!ap.ChannelsQuantity ) return false;
if(BufferSize < 1024  || (BufferSize & 1)) return false;
if(PacketSize > BufferSize/2) PacketSize=BufferSize/2;
if(PacketSize < 32) PacketSize=32;
if(PacketSize % 32) { PacketSize=(PacketSize/32); PacketSize=(PacketSize+1)*32; }

// сбросим флажок завершения потока ввода данных
IsReadThreadComplete = false;

// пока ничего не выделено под буфер данных
RealTimeReadIndex=0;
StopEvent=0;
GlobalPacketSize=PacketSize;
RealBufferSize=BufferSize;
AdcBuffer = NULL;
UserIndex=0;

// пока не создан поток ввода данных
hReadThread = NULL;

// сбросим флаг ошибок потока ввода данных
ReadThreadErrorNumber = 0x0;

// выделим память под буфер
AdcBuffer = new SHORT[2*PacketSize];
if(!AdcBuffer) return false;

RealTimePtr = new SHORT[RealBufferSize];
if(!RealTimePtr) { free(AdcBuffer); return false; }

// Создаём и запускаем поток сбора данных
hReadThread = CreateThread(0, 0x2000, ServiceRealTImeReadThread, 0, 0, &ReadTid);
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

// ждём окончания работы потока ввода данных
WaitForSingleObject(hReadThread, INFINITE);

free(AdcBuffer); free(RealTimePtr);
AdcBuffer=NULL;
RealTimePtr=NULL;

if(ReadThreadErrorNumber) return false;

return true;
}

static int cnt=0;

// Data массив в который будут переложены данные
// NPoints сколько отсчетов перекладывать
// WaitFlag=1 -> будем ждать пока не появятся NPoints данных иначе вернем сколь есть (но не больше NPoints)
// ReadPoints -> сколько данных реально записано в Data
// ReadyToReadPoints -> сколько данных есть готовых к вычитыванию
// overflow -> флаг переполнения буфера
// TimeOut	таймаут мс, по завершению которого функция закончит выполнения даже если данные не были введены (при этом усановится признак ошибки)
__declspec(dllexport) BOOL WINAPI LV_GET_REAL_TIME_DATA(SHORT *Data, DWORD NPoints, DWORD WaitFlag, DWORD *ReadPoints, DWORD *ReadyToReadPoints,
			DWORD *overflow, DWORD TimeOut)
{
DWORD n;
DWORD n_ready, n_to_send;
DWORD time_waited;

*ReadPoints=0;
time_waited=0;

// посмотрим сколько всего данных накопилось
for(;;)
	{
	if(UserIndex <= RealTimeReadIndex) n_ready=RealTimeReadIndex-UserIndex;
	  else n_ready=RealTimeReadIndex+(RealBufferSize-UserIndex);

   *ReadyToReadPoints=n_ready;

   if(n_ready >= NPoints) { n_to_send=NPoints; break; }

   if(!WaitFlag) { n_to_send=n_ready; break; }

   if(ReadThreadErrorNumber) { *overflow=OverflowFlag; return false; }

   Sleep(10);
   time_waited += 10;

   if(time_waited > TimeOut)
   	{
      *overflow=OverflowFlag;
      return false;
      }
   }


if(!n_to_send) { *ReadyToReadPoints=n_ready; *overflow=OverflowFlag; return true; }

// Данные записываются в FIFO размером RealBufferSize пачками по  GlobalPacketSize
// текущее положение указателя записи - RealTimeReadIndex
// текущее положения указателя считывания  UserIndex
if((UserIndex+n_to_send) <= RealBufferSize)
         	{
            memcpy(Data, &RealTimePtr[UserIndex], 2*n_to_send);
            UserIndex += n_to_send;
            if(UserIndex >= RealBufferSize) UserIndex=0;
            }
           else
           	{
            // 1. запишем кусок до конца буфера
            n=RealBufferSize-UserIndex;
            memcpy(Data, &RealTimePtr[UserIndex], 2*n);

            UserIndex=0;

            // дозапишем хвост
            memcpy(&Data[n], &RealTimePtr[UserIndex], 2*(n_to_send-n));
            UserIndex += (n_to_send-n);
            }

*ReadyToReadPoints=n_ready-n_to_send;
*ReadPoints=n_to_send;
*overflow=OverflowFlag;

return true;
}



//------------------------------------------------------------------------
// Поток, в котором осуществляется сбор данных
//------------------------------------------------------------------------
DWORD WINAPI ServiceRealTImeReadThread(PVOID)
{
	WORD RequestNumber;
	DWORD i;
	// идентификатор массива их двух событий
	HANDLE ReadEvent[2];
	// массив OVERLAPPED структур из двух элементов
	OVERLAPPED ReadOv[2];
	IO_REQUEST_LUSBAPI IoReq[2];
   DWORD n;DWORD OldPointer;

	// остановим работу АЦП и одновременно сбросим USB-канал чтения данных
	if(!pModule->STOP_ADC()) { ReadThreadErrorNumber = 0x1; IsReadThreadComplete = true; return 0x0; }

	// формируем необходимые для сбора данных структуры
	for(i = 0x0; i < 0x2; i++)
	{
		// создаём событие
		ReadEvent[i] = CreateEvent(NULL, FALSE , FALSE, NULL);
		// инициализация структуры типа OVERLAPPED
		ZeroMemory(&ReadOv[i], sizeof(OVERLAPPED)); ReadOv[i].hEvent = ReadEvent[i];
		// формируем структуру IoReq
		IoReq[i].Buffer = AdcBuffer + i*GlobalPacketSize;
		IoReq[i].NumberOfWordsToPass = GlobalPacketSize;
		IoReq[i].NumberOfWordsPassed = 0x0;
		IoReq[i].Overlapped = &ReadOv[i];
		IoReq[i].TimeOut = 1000;
	}

	// делаем предварительный запрос на ввод данных
	RequestNumber = 0x0;
	if(!pModule->ReadData(&IoReq[RequestNumber])) { CloseHandle(ReadEvent[0]); CloseHandle(ReadEvent[1]); ReadThreadErrorNumber = 0x2; IsReadThreadComplete = true; return 0x0; }

	// запустим АЦП
	if(pModule->START_ADC())
	{
		// цикл сбора данных
		for(;;)
		{
			// сделаем запрос на очередную порции данных
			RequestNumber ^= 0x1;
			if(!pModule->ReadData(&IoReq[RequestNumber])) { ReadThreadErrorNumber = 0x2; break; }

			// ждём завершения операции сбора предыдущей порции данных
         for(;;)
         	{
            if(StopEvent) break;
				if(WaitForSingleObject(ReadEvent[RequestNumber^0x1], IoReq[RequestNumber^0x1].TimeOut) == WAIT_TIMEOUT) continue;
            	else break;
            }
         if(StopEvent) break;

			// Данные записываются в FIFO размером RealBufferSize пачками по  GlobalPacketSize
			// текущее положение указателя записи - RealTimeReadIndex
			// текущее положения указателя считывания  UserIndex
         OldPointer=RealTimeReadIndex;
         if((RealTimeReadIndex+GlobalPacketSize) <= RealBufferSize)
         	{
            memcpy(&RealTimePtr[RealTimeReadIndex], IoReq[RequestNumber^0x1].Buffer, 2*GlobalPacketSize);
            RealTimeReadIndex += GlobalPacketSize;

	         // проверим на переполнение
   	      if(UserIndex > OldPointer && UserIndex <= RealTimeReadIndex)   OverflowFlag=1;

            if(RealTimeReadIndex == RealBufferSize) RealTimeReadIndex=0;
            }
           else
           	{
            // 1. запишем кусок до конца буфера
            n=RealBufferSize-RealTimeReadIndex;
            memcpy(&RealTimePtr[RealTimeReadIndex], IoReq[RequestNumber^0x1].Buffer, 2*n);

	         // проверим на переполнение
            RealTimeReadIndex += n;
   	      if(UserIndex > OldPointer && UserIndex <= RealTimeReadIndex)   OverflowFlag=1;

            RealTimeReadIndex=0;

            // дозапишем хвост
            memcpy(&RealTimePtr[RealTimeReadIndex], &IoReq[RequestNumber^0x1].Buffer[n], 2*(GlobalPacketSize-n));
            RealTimeReadIndex += (GlobalPacketSize-n);
            }


			if(ReadThreadErrorNumber) break;
				else Sleep(20);
		}

	}
	else { ReadThreadErrorNumber = 0x6; }

	// остановим работу АЦП
	if(!pModule->STOP_ADC()) ReadThreadErrorNumber = 0x1;
	// прервём возможно незавершённый асинхронный запрос на приём данных
	if(!CancelIo(pModule->GetModuleHandle())) { ReadThreadErrorNumber = 0x7; }
	// освободим все идентификаторы событий
	for(i = 0x0; i < 0x2; i++) CloseHandle(ReadEvent[i]);
	// небольшая задержка
	Sleep(20);
	// установим флажок завершения работы потока сбора данных
	IsReadThreadComplete = true;
	// теперь можно спокойно выходить из потока
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
if(calibr == TRUE) val=(val+E140DescriptionStruct.Dac.OffsetCalibration[dac_number])*E140DescriptionStruct.Dac.ScaleCalibration[dac_number];

return (BOOL)pModule->DAC_SAMPLE(&val, dac_number);
}

// -----------------------------------------------------------------------------
// Преобразование кодов АЦП в вольты с учетом калибровки
// -----------------------------------------------------------------------------
__declspec(dllexport) BOOL WINAPI WEProcessArray(SHORT *src, double *dest, DWORD size, DWORD calibr, DWORD volt)
{
	double K_offset[MAX_CONTROL_TABLE_LENGTH_E140], K_scale[MAX_CONTROL_TABLE_LENGTH_E140];
	DWORD i, j, nch, gain_index;
	double ScaleGain[4]={10.0/8000., 2.5/8000., 0.625/8000., 0.15625/8000.};

	nch=ap.ChannelsQuantity;
   if(!nch) { return FALSE; }

   // подготовим коэффициенты
   for(i=0; i < nch; i++)
   	{
      gain_index=(ap.ControlTable[i] >> 6) & 3;
      if(calibr == FALSE)
      	{
         // без калибровки
         if(volt == FALSE)
         	{
            // коды АЦП, без калибровки
            K_offset[i]=0;
            K_scale[i]=1.0;
            }
           else
           	{
            // вольты, без калибровки
            K_offset[i]=0;
            K_scale[i]=ScaleGain[gain_index];
            }
         }
        else
      	{
         // с калибровкой
         if(volt == FALSE)
         	{
            // коды АЦП, с калибровкой
            K_offset[i]=E140DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale[i]=E140DescriptionStruct.Adc.ScaleCalibration[gain_index];
            }
           else
           	{
            // вольты, с калибровкой
            K_offset[i]=E140DescriptionStruct.Adc.OffsetCalibration[gain_index];
            K_scale[i]=E140DescriptionStruct.Adc.ScaleCalibration[gain_index]*ScaleGain[gain_index];
            }
         }
      }

   for(i=j=0; i < size; i++)
   	{
      *dest++=((*src++)+K_offset[j])*K_scale[j];
      j++;
      if(j >= nch) j=0;
      }

	// все хорошо :)))))
	return TRUE;
}

