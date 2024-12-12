/* --- wlusbapi.cpp --------------------------------------------------------------------------------------- **
 *
 * 	wlusbapi - "обертка" основной  dll, для работы с языками не поддерживающими
 *	указатели(MSVB, NI LabView, ect)
 *
 * 	Поддерживаемые модули:
 * 		* E-154
 * 		* E14-140
 * 		* E140-140M
 * 		* E14-440
 * 		* E20-10
 *
 * 	(C) Lcard, 2009.
 * -------------------------------------------------------------------------------------------------------- */

#include <windows.h>
#include <conio.h>                                     
#include "lusbapi.h"
#include "lusbbase.h"
#include "wlusbapi.h"
#include "circularBuf.h"
#pragma argsused

/************************************************************************************************
 *																                                *								
 *                                CONSTANT DEFINITIONS       			    				    *
 *																								*																							
 ***********************************************************************************************/
#define E140_MAX_DAC_FREQ      	 	200.0
#define E440_MAX_DAC_FREQ      	 	125.0
#define E140_M_MAX_DAC_CODE			32767
#define RD_REQ_QNT					0x08

/* Максимальные значения амплитуд сигналов модуля DAC */
static const BYTE dacMaxAmplitudes[ ] =
{
	5, 		/* E154 */
	5, 		/* E140 */
	5, 		/* E440 */
	5		/* E2010 */
};

/* Максимальные значения кода DAC */
static const WORD dacMaxCodes[ ] =
{
	127, 	/* E154 */
	2047, 	/* E140 */
	2047, 	/* E440 */
	2047  	/* E2010 */
};

/* Идентификаторы поддерживаемых библиотекой устройств */
enum
{
	E154_ID,
	E14_140_ID,
	E14_440_ID,
	E2010_ID,
	SUPPORTED_DEVICES_QUANTITY
};

/* Имена модулей так, как они записаны в модулях */
static const CHAR moduleNames[][7] =
{
	"E154",
	"E140",
	"E440",
	"E2010"
};

/* Ревизия модуля */
static CHAR moduleRev;

/************************************************************************************************
 *																                                *								
 *                                 FUNCTION DECLARATIONS       		    	    	    	    *
 *																								*																							
 ***********************************************************************************************/

/* Поток чтения данных из модуля */
DWORD WINAPI ServiceRealTimeReadThread(PVOID /*Context*/);

/* Поток записи данных в модуль */
DWORD WINAPI ServiceRealTimeWriteThread(PVOID /*Context*/);

/* Вход в критическую секцию для потока чтения */
void f_RdEnterCritSect(void);

/* Выход из критической секции для потока чтения */
void f_RdLeaveCritSect(void);

/************************************************************************************************
 *																                                *								
 *                                VARIABLE DEFINITIONS       			    				    *
 *																								*																							
 ***********************************************************************************************/

/***************************************
 *       	 COMMON variables          *
 **************************************/

/* Дескриптор потока чтения */
static HANDLE hReadThread;

/* Дескриптор потока записи */
static HANDLE hWriteThread;

/* Идентификатор потока чтения данных */
static DWORD readThreadId;

/* Идентификатор потока записи данных */
static DWORD writeThreadId;

/* Критические секции */
static CRITICAL_SECTION rdCritSection;
static CRITICAL_SECTION wrCritSection;

/* Идентификатор модуля */
static WORD moduleId;

/* Указатель на интерфейс модуля */
static LPVOID pModule = NULL;

/* Дескриптор устройства */
static HANDLE moduleHandle;

/* Cкорость работы шины USB */
static BYTE usbSpeed;

/* Размер пакета записи в ЦАП */
static DWORD dacPacketSize;

/* Указатель на буфер ЦАП */
static SHORT *pDacBuf = NULL;

/* Указатель на структуру параметров кольцевого буфера ЦАП */
static CIRBUF_Param *pDacBufParam;

/* Указатель на буфер пакета ЦАП */
static SHORT *pDacPacket = NULL;

/* Признак опустошения буфера ЦАП */
static BOOL dacBufUnderflow;

/* Признак остановки ЦАП */
static BOOL dacStopped;

/* Размер пакета записи в АЦП */
static DWORD adcPacketSize;

/* Указатель на буфер АЦП */
static SHORT *pAdcBuf = NULL;

/* Указатель на структуру параметров кольцевого буфера АЦП */
static CIRBUF_Param *pAdcBufParam;

/* Указатель на буфер пакета ЦАП */
static SHORT *pAdcPacket = NULL;

/* Признак переполнения буфера АЦП */
static volatile BOOL adcBufOverflow;

/* Признак остановки АЦП */
static volatile BOOL adcStopped;

/* Признак ошибки при сборе данных с АЦП */
static volatile BOOL readThreadErrDetect;

/* Признак ошибки при передачи данных в ЦАП */
static volatile BOOL writeThreadErrDetect;

/* Признак завершения сбора данных */
static volatile BOOL isReadThreadComplete = FALSE;

/* Признак завершения вывода данных */
static volatile BOOL isWriteThreadComplete = FALSE;

/* Флаг разрешения выходных линий внешнего цифрового разъёма */
static BOOL ttlOutEnabled = 0;

/***************************************
 *      MODULE E154 variables          *
 **************************************/

/* Полная информация о модуле */
static MODULE_DESCRIPTION_E154 e154Desc;

/* Параметры модуля АЦП */
static ADC_PARS_E154 e154AdcParam;

/***************************************
 *      MODULE E140 variables          *
 **************************************/

/* Полная информация о модуле */
static MODULE_DESCRIPTION_E140 e140Desc;

/* Параметры модуля АЦП */
static ADC_PARS_E140 e140AdcParam;


/***************************************
 *      MODULE E440 variables          *
 **************************************/

/* Полная информация о модуле */
static MODULE_DESCRIPTION_E440 e440Desc;

/* Cтруктура параметров DAC модуля */
static DAC_PARS_E440 e440DacParam;

/* Параметры модуля АЦП */
static ADC_PARS_E440 e440AdcParam;


/***************************************
 *      MODULE E2010 variables         *
 **************************************/

/* Полная информация о модуле */
static MODULE_DESCRIPTION_E2010 e2010Desc;

/* Параметры модуля АЦП */
static ADC_PARS_E2010 e2010AdcParam;


/************************************************************************************************
 *																                                *								
 *                               	 COMMON FUNCTIONS DEFINITIONS       	    			    *
 *																								*																							
 ***********************************************************************************************/

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Точка входа в библиотеку.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	switch (reason)
		{
		/* присоединение к адресному пространству текущего процесса */
		case DLL_PROCESS_ATTACH:
			break;
		/* завершение процесса либо вызов FreeLibrary */
		case DLL_PROCESS_DETACH:
			//LV_StopRealTimeAdcSampling();
			//LV_CloseModule();
			break;
		/* процесс создает новый поток */
		case DLL_THREAD_ATTACH:
			break;
		/* завершение потока */
		case DLL_THREAD_DETACH:
			//LV_StopRealTimeAdcSampling();
			//LV_CloseModule();
			break;
		}
    return (1);
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Возвращает текущую версию библиотеки.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) DWORD WINAPI LV_GetDllVersion(VOID)
{
    return (GetDllVersion());
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Открытие модуля.
 *
 * 	ОПИСАНИЕ:
 * 	Открывается интерфейс и делается попытка обнаружить модуль.
 * 	Если модуль найден считывается его имя. Если имя соответствует имени одного из
 * 	поддерживаемых устройств, производится конфигурация и считывание параметров модуля.
 * 	Возвращает TRUE, если модуль открыт успешно, иначе FALSE.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_OpenModule(PCHAR _moduleName, BYTE* _moduleIdx)
{
    WORD slotIdx;
	CHAR moduleName[16];

	if (_moduleIdx == NULL)
		return FALSE;
		
	*_moduleIdx = 0x00;

	/* Закрытие указателя на возможно открытый ранее интерфейс */
    LV_CloseModule();

	/* Определение типа подключенного модуля.
	 * Для этого создается указатель на базовый интерфейс,
	 * и делается попытка обнаружить модуль. Если модуль
	 * обнаружен, читается его имя, которое однозначно
	 * идентифицирует его тип.
	 */

	/* Получение указателя на базовый интерфейс */
    pModule = CreateLInstance("Lusbbase");
    if (!pModule)
    {
        LV_CloseModule();
		return FALSE;
    }

	/* Попытка обнаружить модуль E14-1140 в первых
	 * WORD MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI виртуальных слотах
	 */
    for (slotIdx = 0x0; slotIdx < MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI; slotIdx++)
    {
        if (((ILUSBBASE*)pModule)->OpenLDevice(slotIdx))
            break;
    }
    // что-нибудь обнаружили?
    if (slotIdx == MAX_VIRTUAL_SLOTS_QUANTITY_LUSBAPI)
    {
        LV_CloseModule();
        return FALSE;
    }

	/* Чтение названия модуля в обнаруженном виртуальном слоте */
    if (!((ILUSBBASE*)pModule)->GetModuleName(moduleName))
    {
        LV_CloseModule();
		return FALSE;
    }
	
	/* Закрытие указателя на базовый интерфейс */
	LV_CloseModule();

	/* Получение идентификатора модуля по имени */
	if(!strcmpi(moduleName, "E140"))
		moduleId = E14_140_ID;
	else if(!strcmpi(moduleName, "E154"))
		moduleId = E154_ID;
	else if(!strcmpi(moduleName, "E440"))
		moduleId = E14_440_ID;
	else if(!strcmpi(moduleName, "E20-10"))
		moduleId = E2010_ID;
	else
	{	
		*_moduleIdx = 0xff;
		return (FALSE);
	}

	*_moduleIdx = moduleId;

    /* Получение указателя на интерфейс найденного модуля */
    pModule = CreateLInstance((CHAR*)moduleNames[moduleId]);
    if (!pModule)
    {
        LV_CloseModule();
		return FALSE;
    }

	/* Открытие интерфейса */
	if (!((ILUSBBASE*)pModule)->OpenLDevice(slotIdx))
    {
        LV_CloseModule();
        return FALSE;
    }

    // попробуем прочитать дескриптор устройства
    moduleHandle = ((ILUSBBASE*)pModule)->GetModuleHandle();
    if (moduleHandle == INVALID_HANDLE_VALUE)
    {
        LV_CloseModule();
        return FALSE;
    }

    // попробуем получить скорость работы шины USB
    if (!((ILUSBBASE*)pModule)->GetUsbSpeed(&usbSpeed))
    {
        LV_CloseModule();
        return FALSE;
    }

    /* Для модулей с DSP необходимо вызвать функции загрузки */
	switch (moduleId)
	{
		case (E154_ID):
		case (E14_140_ID):
			/* Do nothing */
			break;

		case (E14_440_ID):
			if (!((ILE440*)pModule)->TEST_MODULE())
			{
				// код LBIOS'а возьмём из соответствующего ресурса штатной DLL библиотеки
				if (!((ILE440*)pModule)->LOAD_MODULE())
				{
					LV_CloseModule();
					return FALSE;
				}
		
				// проверим загрузку модуля
				if (!((ILE440*)pModule)->TEST_MODULE())
				{
					LV_CloseModule();
					return FALSE;
				}
			}
			break;

		case (E2010_ID):
			if (!((ILE2010*)pModule)->TEST_MODULE())
			{
				// код LBIOS'а возьмём из соответствующего ресурса штатной DLL библиотеки
				if (!((ILE2010*)pModule)->LOAD_MODULE())
				{
					LV_CloseModule();
					return FALSE;
				}

				// проверим загрузку модуля
				if (!((ILE2010*)pModule)->TEST_MODULE())
				{
					LV_CloseModule();
					return FALSE;
				}
			}
			break;
	}

    /* Получение информации из ППЗУ модуля */
	switch (moduleId)
	{
		case (E154_ID):
			if (!((ILE154*)pModule)->GET_MODULE_DESCRIPTION(&e154Desc))
			{
				LV_CloseModule();
				return FALSE;
			}
			strcpy(_moduleName, (char*)e154Desc.Module.DeviceName);
			moduleRev = e154Desc.Module.Revision;
			strncat(_moduleName, (char*)(&moduleRev), 1);
			break;

		case (E14_140_ID):
			if (!((ILE140*)pModule)->GET_MODULE_DESCRIPTION(&e140Desc))
			{
				LV_CloseModule();
				return FALSE;
			}
			strcpy(_moduleName, (char*)e140Desc.Module.DeviceName);
			moduleRev = e140Desc.Module.Revision;
			strncat(_moduleName, (char*)(&moduleRev), 1);
			break;

		case (E14_440_ID):
			if (!((ILE440*)pModule)->GET_MODULE_DESCRIPTION(&e440Desc))
			{
				LV_CloseModule();
				return FALSE;
			}
			strcpy(_moduleName, (char*)e440Desc.Module.DeviceName);
			moduleRev = e440Desc.Module.Revision;
			strncat(_moduleName, (char*)(&moduleRev), 1);			
			break;

		case (E2010_ID):
			if (!((ILE2010*)pModule)->GET_MODULE_DESCRIPTION(&e2010Desc))
			{
				LV_CloseModule();
				return FALSE;
			}
			strcpy(_moduleName, (char*)e2010Desc.Module.DeviceName);
			moduleRev = e2010Desc.Module.Revision;
			strncat(_moduleName, (char*)(&moduleRev), 1);
			break;
	}

    return TRUE;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Закрытие модуля.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) VOID WINAPI LV_CloseModule(VOID)
{
    if (pModule != NULL)
    {
		while (hReadThread != NULL || hWriteThread != NULL)
			;
        ((ILUSBBASE*)pModule)->ReleaseLInstance();
        pModule = NULL;
		ttlOutEnabled = 0;
    }
}


/************************************************************************************************
 *																                                *								
 *                               	 ADC FUNCTIONS DEFINITIONS       	    				    *
 *																								*																							
 ***********************************************************************************************/

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Обработка отсчета АЦП.
 *
 * 	ОПИСАНИЕ:
 * 		Преобразование кода АЦП в вольты(если задано) и калибровка полученного
 * 		значения(если задано).
 * 		Возвращает TRUE, если преобразование прошло успешно, иначе FALSE.
 *-----------------------------------------------------------------------------------------------*/
static BOOL WINAPI E154_ProcessOnePoint(const SHORT _sampleCode,
										const WORD _chNum,
										const BYTE _needCalibr,
										const BYTE _transToVolt,
										double* _result
										)
{
    double offset;
	double scale;
    WORD gainIdx;
    const double scaleGain[4] =
    {
        5.0 / 2000., 1.6 / 2000., 0.5 / 2000., 0.16 / 2000.
    };

	if (_result == NULL)
		return FALSE;
	
    // подготовим коэффициенты
    gainIdx = (_chNum >> 6) & 3;
    if (_needCalibr == FALSE)
    {
        // без калибровки
        if (_transToVolt == FALSE)
        {
            // коды АЦП, без калибровки
            offset = 0;
            scale = 1.0;
        }
        else
        {
            // вольты, без калибровки
            offset = 0;
            scale = scaleGain[gainIdx];
        }
    }
    else
    {
        // с калибровкой
        if (_transToVolt == FALSE)
        {
            // коды АЦП, с калибровкой
            offset = e154Desc.Adc.OffsetCalibration[gainIdx];
            scale = e154Desc.Adc.ScaleCalibration[gainIdx];
        }
        else
        {
            // вольты, с калибровкой
            offset = e154Desc.Adc.OffsetCalibration[gainIdx];
            scale = e154Desc.Adc.ScaleCalibration[gainIdx] * scaleGain[gainIdx];
        }
    }

    *_result = (_sampleCode + offset) * scale;

    return TRUE;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Обработка отсчета АЦП.
 *
 * 	ОПИСАНИЕ:
 * 		Преобразование кода АЦП в вольты(если задано) и калибровка полученного
 * 		значения(если задано).
 * 		Возвращает TRUE, если преобразование прошло успешно, иначе FALSE.
 *-----------------------------------------------------------------------------------------------*/
static BOOL WINAPI E140_ProcessOnePoint(const SHORT _sampleCode,
										const WORD _chNum,
										const BYTE _needCalibr,
										const BYTE _transToVolt,
										double* _result
									   )
{
	double offset, scale;
	WORD gainIdx;
	double scaleGain[4] = {10.0/8000., 2.5/8000., 0.625/8000., 0.15625/8000.};

	if (_result == NULL)
		return FALSE;
		
    // подготовим коэффициенты
	gainIdx = (_chNum >> 6) & 3;
	if (_needCalibr == FALSE)
	{
		// без калибровки
		if (_transToVolt == FALSE)
		{
			// коды АЦП, без калибровки
			offset=0;
			scale=1.0;
		}
	    else
		{
			// вольты, без калибровки
			offset=0;
			scale=scaleGain[gainIdx];
		}
	}
	else
	{
		// с калибровкой
		if (_transToVolt == FALSE)
		{
			// коды АЦП, с калибровкой
			offset=e140Desc.Adc.OffsetCalibration[gainIdx];
			scale=e140Desc.Adc.ScaleCalibration[gainIdx];
		}
		else
		{
			// вольты, с калибровкой
			offset=e140Desc.Adc.OffsetCalibration[gainIdx];
			scale=e140Desc.Adc.ScaleCalibration[gainIdx]*scaleGain[gainIdx];
		}
	}

	*_result = (_sampleCode + offset) * scale;

	return TRUE;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Обработка отсчета АЦП.
 *
 * 	ОПИСАНИЕ:
 * 		Преобразование кода АЦП в вольты(если задано) и калибровка полученного
 * 		значения(если задано).
 * 		Возвращает TRUE, если преобразование прошло успешно, иначе FALSE.
 *-----------------------------------------------------------------------------------------------*/
static BOOL WINAPI E440_ProcessOnePoint(const SHORT _sampleCode,
										const WORD _chNum,
										const BYTE _needCalibr,
										const BYTE _transToVolt,
										double* _result
										)
{
    double offset, scale;
    WORD gainIdx;
    const double scaleGain[4] =
    {
        10.0 / 8000., 2.5 / 8000., 0.625 / 8000., 0.15625 / 8000.
    };

	if (_result == NULL)
		return FALSE;
		
    // подготовим коэффициенты
    gainIdx = (_chNum >> 6) & 3;
    if (_needCalibr == FALSE)
    {
        // без калибровки
        if (_transToVolt == FALSE)
        {
            // коды АЦП, без калибровки
            offset = 0;
            scale = 1.0;
        }
        else
        {
            // вольты, без калибровки
            offset = 0;
            scale = scaleGain[gainIdx];
        }
    }
    else
    {
        // с калибровкой
        if (_transToVolt == FALSE)
        {
            // коды АЦП, с калибровкой
            offset = e440Desc.Adc.OffsetCalibration[gainIdx];
            scale = e440Desc.Adc.ScaleCalibration[gainIdx];
        }
        else
        {
            // вольты, с калибровкой
            offset = e440Desc.Adc.OffsetCalibration[gainIdx];
            scale = e440Desc.Adc.ScaleCalibration[gainIdx] * scaleGain[gainIdx];
        }
    }

    *_result = (_sampleCode + offset) * scale;

    return TRUE;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Обработка отсчета АЦП.
 *
 * 	ОПИСАНИЕ:
 * 		Преобразование кода АЦП в вольты(если задано) и калибровка полученного
 * 		значения(если задано).
 * 		Возвращает TRUE, если преобразование прошло успешно, иначе FALSE.
 *-----------------------------------------------------------------------------------------------*/
static BOOL WINAPI E2010_ProcessOnePoint(const SHORT _adcSample,
										 const WORD _chNum,
										 const BYTE _needCalibr,
										 const BYTE _transToVolt,
										 double* _result
										)
{
    double offset, scale;
    WORD gainIdx;
    double scaleGain[3] =
    {
        3.0 / 8000., 1.0 / 8000., 0.3 / 8000.
    };
	
	if (_result == NULL)
		return FALSE;	

    // подготовим коэффициенты
    gainIdx = e2010AdcParam.InputRange[_chNum];
    if (_needCalibr == FALSE)
    {
        // без калибровки
        if (_transToVolt == FALSE)
        {
            // коды АЦП, без калибровки
            offset = 0;
            scale = 1.0;
        }
        else
        {
            // вольты, без калибровки
            offset = 0;
            scale = scaleGain[gainIdx];
        }
    }
    else
    {
        // с калибровкой
        if (_transToVolt == FALSE)
        {
            // коды АЦП, с калибровкой
            offset = e2010Desc.Adc.OffsetCalibration[gainIdx];
            scale = e2010Desc.Adc.ScaleCalibration[gainIdx];
        }
        else
        {
            // вольты, с калибровкой
            offset = e2010Desc.Adc.OffsetCalibration[gainIdx];
            scale = e2010Desc.Adc.ScaleCalibration[gainIdx] * scaleGain[gainIdx];
        }
    }

    *_result = (_adcSample + offset) * scale;

    return TRUE;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Обработка отсчета АЦП.
 *
 * 	ОПИСАНИЕ:
 * 		Преобразование кода АЦП в вольты(если задано) и калибровка полученного
 * 		значения(если задано).
 * 		Возвращает TRUE, если преобразование прошло успешно, иначе FALSE.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_ProcessSingleAdcSample(const SHORT _sampleCode,
															const WORD _chNum,
															const BYTE _needCalibr,
															const BYTE _transToVolt,
															double* _result
															)
{
	if (_result == NULL)
		return FALSE;
		
	switch (moduleId)
	{
		case (E154_ID):
			return E154_ProcessOnePoint(_sampleCode, _chNum, _needCalibr, _transToVolt, _result);

		case (E14_140_ID):
			return E140_ProcessOnePoint(_sampleCode, _chNum, _needCalibr, _transToVolt, _result);

		case (E14_440_ID):
			return E440_ProcessOnePoint(_sampleCode, _chNum, _needCalibr, _transToVolt, _result);

		case (E2010_ID):
			return E2010_ProcessOnePoint(_sampleCode, _chNum, _needCalibr, _transToVolt, _result);

      default:
      	return FALSE;
	}
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Обработка массива отсчетов АЦП.
 *
 * 	ОПИСАНИЕ:
 * 		Преобразование кодов АЦП в вольты(если задано) и калибровка полученного
 * 		значения(если задано).
 * 		Возвращает TRUE, если преобразование прошло успешно, иначе FALSE.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_ProcessAdcArray(const SHORT* _pSampleData,
													 const DWORD _size,
													 const BYTE _needCalibr,
													 const BYTE _transToVolt,
													 double* _pResultData
												    )
{

	DWORD i;
	WORD j;
	WORD chQnt = 0;
	WORD chNum;
	const SHORT *pin = _pSampleData;
	double *pout = _pResultData;

	if ((_pSampleData == NULL) || (_pResultData == NULL))
        return FALSE;
	
	switch (moduleId)
	{
		case (E154_ID):
			chQnt = e154AdcParam.ChannelsQuantity;
            break;
		
		case (E14_140_ID):
			chQnt = e140AdcParam.ChannelsQuantity;
            break;

		case (E14_440_ID):
			chQnt = e440AdcParam.ChannelsQuantity;
            break;
		
		case (E2010_ID):
			chQnt = e2010AdcParam.ChannelsQuantity;
            break;
        default:
            return FALSE;  
	}
	if (!chQnt)
		return FALSE;

	for (i = j = 0; i < _size; i++)
	{
		switch (moduleId)
		{
			case (E154_ID):
				chNum = e154AdcParam.ControlTable[j];
				break;
			
			case (E14_140_ID):
				chNum = e140AdcParam.ControlTable[j];
				break;
			
			case (E14_440_ID):
				chNum = e440AdcParam.ControlTable[j];
				break;
			
			case (E2010_ID):
				chNum = e2010AdcParam.ControlTable[j];
				break;
            default:
                return FALSE;
		}

		LV_ProcessSingleAdcSample(*pin,  chNum, _needCalibr, _transToVolt, pout);
		pout++;
		pin++;
		if (++j >= chQnt)
			j = 0;
	}

	return TRUE;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Создание логического номера канала.
 *
 * 	ОПИСАНИЕ:
 * 		Логические номер канала - это значение, содержащеее информацию о физическом номере
 *		канала, усилении и режиме работы данного физического канала.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) WORD WINAPI LV_CreateAdcChannel(const WORD _chNum,
													   const WORD _gain,
													   const WORD _diffMode
													  )
{
    return (_chNum & 0x1F) | ((_gain & 3) << 6) | ((_diffMode & 1) << 5);
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Установка параметров синхронного ввода.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI E154_SetAdcSynchroParameters
	(
	const WORD 		_clkSource,						
	const WORD 		_enableClkOutput,				
	const WORD 		_inputMode,							
	const WORD 		_synchroAdType,						
	const WORD 		_synchroAdMode, 						
	const WORD 		_synchroAdChannel,  				
	const SHORT 	_synchroAdPorog, 					
	const WORD 		_channelsQuantity,					
	const WORD* 	_controlTable,					
	const double 	_adcRate,	  			  			
	const double 	_interKadrDelay,		  			
	double* 		_kadrRate,
	double*			_actualAdcRate,
	double*			_actualInterKadrDelay					
	)
{
    int i;

    if (!_channelsQuantity)
        return false;
		
	if ((pModule == NULL) || (_controlTable == NULL))
		return FALSE;

    // получим текущие параметры работы АЦП
    if (!((ILE154 *)pModule)->GET_ADC_PARS(&e154AdcParam))
        return false;

    // установим желаемые параметры АЦП
    e154AdcParam.ClkSource = _clkSource;
    e154AdcParam.EnableClkOutput = _enableClkOutput;
    e154AdcParam.InputMode = _inputMode;
    e154AdcParam.SynchroAdType = _synchroAdType;
    e154AdcParam.SynchroAdMode = _synchroAdMode;
    e154AdcParam.SynchroAdChannel = _synchroAdChannel;
    e154AdcParam.SynchroAdPorog = _synchroAdPorog;
    e154AdcParam.ChannelsQuantity = _channelsQuantity;

    for (i = 0x0; i < _channelsQuantity; i++)
        e154AdcParam.ControlTable[i] = _controlTable[i];

	e154AdcParam.AdcRate = _adcRate;
    e154AdcParam.InterKadrDelay = _interKadrDelay;

    // передадим требуемые параметры работы АЦП в модуль
    if (!((ILE154 *)pModule)->SET_ADC_PARS(&e154AdcParam))
        return false;

    if (!((ILE154 *)pModule)->GET_ADC_PARS(&e154AdcParam))
        return false;

	if (_kadrRate != NULL)
		*_kadrRate = e154AdcParam.KadrRate;
	if (_actualAdcRate != NULL)
		*_actualAdcRate = e154AdcParam.AdcRate;
	if (_actualInterKadrDelay != NULL)
		*_actualInterKadrDelay = e154AdcParam.InterKadrDelay;

    return true;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Установка параметров синхронного ввода.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI E140_SetAdcSynchroParameters
	(
	const WORD _clkSource,
	const WORD _enableClkOutput,
	const WORD _inputMode,
	const WORD _synchroAdType,
	const WORD _synchroAdMode,
	const WORD _synchroAdChannel,
	const SHORT _synchroAdPorog,
	const WORD  _channelsQuantity,
	const WORD* _controlTable,
	double 		_adcRate,						
	double 		_interKadrDelay,
	double* 	_kadrRate,									
	double* 	_actualAdcRate,
	double* 	_actualInterKadrDelay
	)
{
    int i;

    if (!_channelsQuantity)
        return false;
		
	if ((pModule == NULL) || (_controlTable == NULL))
		return FALSE;		

    // получим текущие параметры работы АЦП
    if (!((ILE140 *)pModule)->GET_ADC_PARS(&e140AdcParam))
        return false;

    // установим желаемые параметры АЦП
	e140AdcParam.ClkSource = _clkSource;
	e140AdcParam.EnableClkOutput = _enableClkOutput;
    e140AdcParam.InputMode = _inputMode;
    e140AdcParam.SynchroAdType = _synchroAdType;
    e140AdcParam.SynchroAdMode = _synchroAdMode;
    e140AdcParam.SynchroAdChannel = _synchroAdChannel;
    e140AdcParam.SynchroAdPorog = _synchroAdPorog;
    e140AdcParam.ChannelsQuantity = _channelsQuantity;

    for (i = 0x0; i < _channelsQuantity; i++)
        e140AdcParam.ControlTable[i] = _controlTable[i];

    e140AdcParam.AdcRate = _adcRate;
	e140AdcParam.InterKadrDelay = _interKadrDelay;

    // передадим требуемые параметры работы АЦП в модуль
    if (!((ILE140 *)pModule)->SET_ADC_PARS(&e140AdcParam))
        return false;

    if (!((ILE140 *)pModule)->GET_ADC_PARS(&e140AdcParam))
        return false;

	if (_kadrRate != NULL)
		*_kadrRate = e140AdcParam.KadrRate;
	if (_actualAdcRate != NULL)
		*_actualAdcRate = e140AdcParam.AdcRate;
	if (_actualInterKadrDelay != NULL)
		*_actualInterKadrDelay = e140AdcParam.InterKadrDelay;

    return true;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Установка параметров синхронного ввода.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI E440_SetAdcSynchroParameters
	(
	const WORD 		_isCorrectionEnabled,		
	const WORD 		_adcClockSource,					
	const WORD 		_inputMode,						
	const WORD 		_synchroAdType,				
	const WORD 		_synchroAdMode, 					
	const WORD 		_synchroAdChannel,  			
	const SHORT 	_synchroAdPorog, 				
	const WORD 		_channelsQuantity,				
	const WORD* 	_controlTable,	
	const double 	_adcRate,	  			  		
	const double 	_interKadrDelay,		  					
	const WORD 		_adcFifoBaseAddress,			
	const WORD 		_adcFifoLength,					
	const double* 	_adcOffsetCoefs,	
	const double* 	_adcScaleCoefs,
	double* 		_kadrRate,
	double* 		_actualAdcRate,
	double* 		_actualInterKadrDelay	
	)
{
	int i;
	
	if (!_channelsQuantity)
		return false;
		
	if ((pModule == NULL) || (_controlTable == NULL))
		return FALSE;			
	
	// получим текущие параметры работы АЦП
	if(!((ILE440 *)pModule)->GET_ADC_PARS(&e440AdcParam))
		return false;
	
	// установим желаемые параметры АЦП
	e440AdcParam.IsCorrectionEnabled = (_isCorrectionEnabled > 0) ? (TRUE) : (FALSE);
	e440AdcParam.AdcClockSource = _adcClockSource;
	e440AdcParam.InputMode = _inputMode;
	e440AdcParam.SynchroAdType = _synchroAdType;
	e440AdcParam.SynchroAdMode = _synchroAdMode;
	e440AdcParam.SynchroAdChannel = _synchroAdChannel;
	e440AdcParam.SynchroAdPorog = _synchroAdPorog;
	e440AdcParam.ChannelsQuantity = _channelsQuantity;

	for (i = 0x0; i < _channelsQuantity; i++)
		e440AdcParam.ControlTable[i] = _controlTable[i];

   if (_isCorrectionEnabled)
   {
	if (_adcOffsetCoefs != NULL && _adcScaleCoefs != NULL)
	{
		for (i = 0x0; i < _channelsQuantity; i++)
		{
			e440AdcParam.AdcOffsetCoefs[i] = _adcOffsetCoefs[i];
			e440AdcParam.AdcScaleCoefs[i] = _adcScaleCoefs[i];
		}
	}
	else
	{
		for (i = 0x0; i < _channelsQuantity; i++)
		{
			e440AdcParam.AdcOffsetCoefs[i] = e440Desc.Adc.OffsetCalibration[i];
			e440AdcParam.AdcScaleCoefs[i] = e440Desc.Adc.ScaleCalibration[i];
		}
	}
   }
	
	e440AdcParam.AdcRate = _adcRate;
	e440AdcParam.InterKadrDelay = _interKadrDelay;
	e440AdcParam.AdcFifoBaseAddress = _adcFifoBaseAddress;
	e440AdcParam.AdcFifoLength = _adcFifoLength;

	// передадим требуемые параметры работы АЦП в модуль
	if(!((ILE440 *)pModule)->SET_ADC_PARS(&e440AdcParam)) return false;
	
	if(!((ILE440 *)pModule)->GET_ADC_PARS(&e440AdcParam)) return false;
	
	if (_kadrRate)
		*_kadrRate = e440AdcParam.KadrRate;
	if (_actualAdcRate)
		*_actualAdcRate = e440AdcParam.AdcRate;
	if (_actualInterKadrDelay)
		*_actualInterKadrDelay = e440AdcParam.InterKadrDelay;
	
	return true;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Установка параметров синхронного ввода.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI E2010_SetAdcSynchroParameters
	(
	const WORD	 	_isAdcCorrectionEnabled,
	const WORD 	 	_overloadMode,
	const WORD 	 	_inputCurrentControl,
	const WORD	 	_startSource,				
	const DWORD  	_startDelay, 					
	const WORD	 	_synchroSource,				
	const DWORD  	_stopAfterNKadrs,				
	const WORD	 	_synchroAdMode,   				
	const WORD	 	_synchroAdChannel,			
	const SHORT  	_synchroAdPorog,  				
	const BYTE	 	_isBlockDataMarkerEnabled,	
	const WORD 	 	_channelsQuantity,
	const WORD*	 	_controlTable,
	const WORD*	 	_inputRange,
	const WORD*	 	_inputSwitch,
	const double 	_adcRate,
	const double 	_interKadrDelay,
	const double*	_adcOffsetCoefs,
	const double*	_adcScaleCoefs,
	double*			_kadrRate,
	double* 	 	_actualAdcRate,
	double*		 	_actualInterKadrDelay
    )
{
	WORD i, j;
	
	if (!_channelsQuantity)
		return false;
	
	if ((pModule == NULL) || (_controlTable == NULL) || (_inputRange == NULL) || (_inputSwitch == NULL))
		return FALSE;	
		
	// получим текущие параметры работы АЦП
	if(!((ILE2010 *)pModule)->GET_ADC_PARS(&e2010AdcParam))
		return false;
	
	/* Установка параметров АЦП.
	 */
	e2010AdcParam.IsAdcCorrectionEnabled = (_isAdcCorrectionEnabled > 0) ? (TRUE) : (FALSE);		
	e2010AdcParam.OverloadMode = _overloadMode;
	e2010AdcParam.InputCurrentControl = _inputCurrentControl;
	e2010AdcParam.SynchroPars.StartSource = _startSource;
	e2010AdcParam.SynchroPars.StartDelay = _startDelay;
	e2010AdcParam.SynchroPars.SynhroSource = _synchroSource;
	e2010AdcParam.SynchroPars.StopAfterNKadrs = _stopAfterNKadrs;
	e2010AdcParam.SynchroPars.SynchroAdMode = _synchroAdMode;
	e2010AdcParam.SynchroPars.SynchroAdChannel = _synchroAdChannel;
	e2010AdcParam.SynchroPars.SynchroAdPorog = _synchroAdPorog;
	e2010AdcParam.ChannelsQuantity = _channelsQuantity; 	
	
	/* Формирование управляющей таблицы */	
	for (i = 0x0; i < _channelsQuantity; i++)
		e2010AdcParam.ControlTable[i] = _controlTable[i];

	/* Диапазон входного напряжения и тип подключения входного сигнала */
	for (i = 0x0; i < _channelsQuantity; i++)
	{
		e2010AdcParam.InputRange[i] = _inputRange[i];  		// входной диапазоне
		e2010AdcParam.InputSwitch[i] = _inputSwitch[i];		// источник входа - сигнал
	}

	// передаём в структуру параметров работы АЦП корректировочные коэффициенты АЦП
	for (i = 0x0; i < ADC_INPUT_RANGES_QUANTITY_E2010; i++)
	{	
		if (_adcOffsetCoefs != NULL && _adcScaleCoefs != NULL)
		{
			// корректировка смещения
			e2010AdcParam.AdcOffsetCoefs[i][j] = _adcOffsetCoefs[j + i*ADC_CHANNELS_QUANTITY_E2010];
			// корректировка масштаба
			e2010AdcParam.AdcScaleCoefs[i][j] = _adcScaleCoefs[j + i*ADC_CHANNELS_QUANTITY_E2010];
		}
		else
		{
			for (j = 0x0; j < _channelsQuantity; j++)
			{
				// корректировка смещения
				e2010AdcParam.AdcOffsetCoefs[i][j] = e2010Desc.Adc.OffsetCalibration[j + i*ADC_CHANNELS_QUANTITY_E2010];
				// корректировка масштаба
				e2010AdcParam.AdcScaleCoefs[i][j] = e2010Desc.Adc.ScaleCalibration[j + i*ADC_CHANNELS_QUANTITY_E2010];
			}
		}
	}
		
	/* Частота дискретизации */
	e2010AdcParam.AdcRate = _adcRate;

	/* Межкадровая задержка */
	e2010AdcParam.InterKadrDelay = _interKadrDelay;
	
	// передадим требуемые параметры работы АЦП в модуль
	if(!((ILE2010 *)pModule)->SET_ADC_PARS(&e2010AdcParam)) return false;
	
	if(!((ILE2010 *)pModule)->GET_ADC_PARS(&e2010AdcParam)) return false;
	
	if (_kadrRate)
		*_kadrRate = e2010AdcParam.KadrRate;
	if (_actualInterKadrDelay)
		*_actualInterKadrDelay = e2010AdcParam.InterKadrDelay;
	if (_actualAdcRate)
		*_actualAdcRate = e2010AdcParam.AdcRate;
	
	return true;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Запуск сбора данных с АЦП в реальном времени.
 *
 * 	ОПИСАНИЕ:
 * 		_bufSize - размер буфера в двухбайтных словах
 * 		_packetSize - размер пакета.
 *
 * 		Внимание! Размер буфера должен быть кратен степени двойки. Если размер буфера заданный
 * 		пользователем не удовлетворяет этому условию, размер буфера будет установлен к ближайшему
 * 		кратному значению.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_StartRealTimeAdcSampling(const DWORD _bufSize, const DWORD _packetSize)
{
	DWORD bufSize = 1;
	DWORD packetSize = _packetSize;

	switch (moduleId)
	{
		case (E154_ID):
			if (!e154AdcParam.ChannelsQuantity)
				return false;
			break;

		case (E14_140_ID):
			if (!e140AdcParam.ChannelsQuantity)
				return false;
			break;

		case (E14_440_ID):
			if (!e440AdcParam.ChannelsQuantity)
				return false;
			break;

		case (E2010_ID):
			if (!e2010AdcParam.ChannelsQuantity)
				return false;
			break;
	}

	if (_bufSize < 1024 || (_bufSize & 1))
        return false;

	while (1)
	{
		bufSize *= 2;
		if (bufSize >= _bufSize)
			break;
	}

    if (packetSize > bufSize / 2)
        packetSize = bufSize / 2;

    if (packetSize < 32)
        packetSize = 32;

    if (packetSize % 32)
    {
        packetSize = (packetSize / 32);
        packetSize = (packetSize + 1)*32;
    }

	adcStopped = 0;
    isReadThreadComplete = FALSE;
	readThreadErrDetect = FALSE;
	hReadThread = NULL;

    /* Выделение памяти под буфер */
    pAdcBuf = new SHORT[bufSize];
    if (NULL == pAdcBuf)
    {
        return (false);
    }

    /* Выделение памяти под пакет */
    pAdcPacket = new SHORT[RD_REQ_QNT*packetSize];
    if (NULL == pAdcPacket)
    {
        free(pAdcBuf);
        return (false);
    }

    /* Выделение памяти под параметры буфера */
    pAdcBufParam = new CIRBUF_Param;
    if (NULL == pAdcBufParam)
    {
        free(pAdcBuf);
        free(pAdcPacket);
        return (false);
    }

    /* Инициализация буфера */
    CIRBUF_Init(bufSize, pAdcBufParam);

	/* Критическая секция */
	InitializeCriticalSection(&rdCritSection);
	pAdcBufParam->EnterCriticalSection = f_RdEnterCritSect;
	pAdcBufParam->LeaveCriticalSection = f_RdLeaveCritSect;

	/* Создание и запуск потока чтения данных */
    hReadThread = CreateThread(0, 0x2000, ServiceRealTimeReadThread, 0, 0, &readThreadId);
    if (!hReadThread)
	{	
        free(pAdcBuf);
        free(pAdcPacket);
        free(pAdcBufParam);
        return (false);
	}
	adcPacketSize = packetSize;

    if (readThreadErrDetect)
        return false;

    return true;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Остановка чтения в реальном времени данных с АЦП.
 *
 * 	ОПИСАНИЕ:
 * 		Освобождается память занятая буферами.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_StopRealTimeAdcSampling(void)
{
    adcStopped = 1;

    for (; !isReadThreadComplete; )
    {
        Sleep(20);
    }

    // ждём окончания работы потока ввода данных
    WaitForSingleObject(hReadThread, INFINITE);
    //TerminateThread(hReadThread, 0);
    DeleteCriticalSection(&rdCritSection);

    CloseHandle(hReadThread);
    hReadThread = NULL;
    free(pAdcBuf);
    free(pAdcPacket);
    free(pAdcBufParam);
    pAdcBuf = NULL;
    pAdcPacket = NULL;
	pAdcBufParam = NULL;

    if (readThreadErrDetect)
        return false;

    return true;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Получение данных с АЦП в синхронном режиме.
 *
 * 	ОПИСАНИЕ:
 * 		_pData - указатель на буфер куда будут помещены данные.
 * 		_sampleQnt - количество выборок которые надо считать из буфера.
 * 		_timeoutMs - тайм-аут операции чтения, мс.
 * 		_waitComplete - если значение больше нуля, ожидается окончание операции чтения до
 * 						истечения тайм-аута.
 * 		_pProcessedSampleQnt - количество помещенных в буфер выборок.
 * 		_restSampleQnt - количество оставшихся выборок.
 * 		_overflow - флаг переполнения буфера.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_GetRealTimeAdcData(SHORT* _pData, 																												
														const DWORD _sampleQnt,
														const DWORD _timeoutMs,
														const BYTE _waitToComplete,
                                                        DWORD* _pProcessedSampleQnt,
														DWORD* _restSampleQnt,
                                                        BYTE* _overflow
													    )
{
    DWORD n_ready, n_to_send;
    DWORD time_waited;

	if (pAdcBufParam == NULL || _pData == NULL)
		return FALSE;
	
	if (_pProcessedSampleQnt != NULL)
		*_pProcessedSampleQnt = 0;
		
    time_waited = 0;

	/* Ожидание поступления требуемого количества данных
	 * в буфер или истечения тайм-аута.
	 */
    for (; ;)
    {
		n_ready = CIRBUF_GetDataSize(pAdcBufParam);
		if (_restSampleQnt != NULL)
			*_restSampleQnt = n_ready;

        if (n_ready >= _sampleQnt)
        {
            n_to_send = _sampleQnt;
            break;
        }

        if (!_waitToComplete)
        {
            n_to_send = n_ready;
            break;
        }

        if (readThreadErrDetect)
        {
			if (_overflow != NULL)
				*_overflow = adcBufOverflow;
            return false;
        }

        Sleep(10);
        time_waited += 10;

        if (_timeoutMs > 0)
        {
        		if (time_waited > _timeoutMs)
        		{
				if (_overflow != NULL)
					*_overflow = adcBufOverflow;
            	return false;
        		}
        }
    }


    if (!n_to_send)
    {
		if (_restSampleQnt != NULL)
        *_restSampleQnt = n_ready;
		if (_overflow != NULL)
			*_overflow = adcBufOverflow;
        return true;
    }

	/* Извлечение данных из буфера */
	CIRBUF_POP_MULTIPLE_FRONT(pAdcBuf, (*pAdcBufParam), _pData, n_to_send);

	if (_restSampleQnt != NULL)
		*_restSampleQnt = n_ready - n_to_send;
	if (_pProcessedSampleQnt)
		*_pProcessedSampleQnt = n_to_send;
	if (_overflow != NULL)
		*_overflow = adcBufOverflow;

    return readThreadErrDetect ? false : true;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Однократное получение данных с АЦП.
 *
 * 	ОПИСАНИЕ:
 * 		_chNum - номер канала АЦП.
 * 		_needCalibr - признак необходимости применения калибровочных коэффициентов.
 * 		_error - признак ошибки.
 *
 * 		Возвращает полученное значение.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) double WINAPI LV_DoSingleAdcSample(const WORD _chNum, const BYTE _needCalibr, BYTE* _error)
{
    double f = 0.0;
    SHORT adc;
    BOOL result;

	if (pModule == NULL)
		{
		*_error = 0;
		return 0.0;
		}
	
	/* Получение данных с АЦП */
	switch (moduleId)
	{
		case (E154_ID):
			result = ((ILE154*)pModule)->ADC_SAMPLE(&adc, _chNum);
			break;

		case (E14_140_ID):
			result = ((ILE140*)pModule)->ADC_SAMPLE(&adc, _chNum);
			break;

		case (E14_440_ID):
			result = ((ILE440*)pModule)->ADC_SAMPLE(&adc, _chNum);
			break;

		case (E2010_ID):
			result = FALSE;
			break;
	}
	if (_error != NULL)
		*_error = (result == TRUE) ? 1 : 0;
    if (result == FALSE)
        return f;

	/* Обработка полученных данных */
	switch (moduleId)
	{
		case (E154_ID):
			result = E154_ProcessOnePoint(adc, _chNum, _needCalibr, TRUE, &f);
			break;

		case (E14_140_ID):
			result = E140_ProcessOnePoint(adc, _chNum, _needCalibr, TRUE, &f);
			break;

		case (E14_440_ID):
			result = E440_ProcessOnePoint(adc, _chNum, _needCalibr, TRUE, &f);
			break;

		case (E2010_ID):
			result = E2010_ProcessOnePoint(adc, _chNum, _needCalibr, TRUE, &f);
			break;
	}
	if (_error != NULL)
		*_error = (result == TRUE) ? 1 : 0;

    return f;
}


/************************************************************************************************
 *																                                *								
 *                               	 DAC FUNCTIONS DEFINITIONS       	    				    *
 *																								*																							
 ***********************************************************************************************/

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Установка параметров ЦАП.
 *
 * 	ОПИСАНИЕ:
 * 		_rate - частота выборки на канал.
 * 		_syncWithAdc - синхронизировать запуск с  пуском АЦП.
 * 		_zeroOnStop - при остановки потокового вывода уст. на выходе ЦАП нулевое значение.
 * 		_actualRate - реально установленная частота выборки на канал.
 *
 * 		Возвращает true, если конфигурация прошла успешно, иначе false.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_E140_SetDacSyncParameters(const double _rate,
                                                               const BYTE _syncWithAdc,
                                                               const BYTE _zeroOnStop,
                                                               double* _actualRate
                                                              )
{
    DAC_PARS_E140 dp;
    double 		  rate;

	if (pModule == NULL)
		return FALSE;
	if (_actualRate != NULL)
		*_actualRate = 0;

	/* Проверка наличия ЦАП */
	if (DAC_INACCESSIBLED_E140 == e140Desc.Dac.Active)
		return (FALSE);

    /* Получение текущих параметров работы ЦАП */
    if (!((ILE140 *)pModule)->GET_DAC_PARS(&dp))
        return false;

    /* Проверка корректности параметров */
    rate = (_rate > E140_MAX_DAC_FREQ) ? (E140_MAX_DAC_FREQ) : (_rate);

    /* Установка требуемых параметров ЦАП */
    dp.DacRate = rate;
    dp.SyncWithADC = _syncWithAdc;
    dp.SetZeroOnStop = _zeroOnStop;

    /* Передача параметров в плату */
    if (!((ILE140 *)pModule)->SET_DAC_PARS(&dp))
        return false;

    if (!((ILE140 *)pModule)->GET_DAC_PARS(&dp))
        return false;

	if (_actualRate != NULL)
		*_actualRate = dp.DacRate;
    return true;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Установка параметров ЦАП.
 *
 * 	ОПИСАНИЕ:
 * 		_rate - частота выборки на канал.
 * 		_fifoLength - длина буфера FIFO.
 * 		_actualRate - реально установленная частота выборки на канал.
 *
 * 		Возвращает true, если конфигурация прошла успешно, иначе false.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_E440_SetDacSyncParameters(const double _rate,
                                                               const WORD _fifoLength,
                                                               double* _actualRate
                                                              )
{
    DAC_PARS_E440 dp;
    double 		  rate;

	if (pModule == NULL)
		return FALSE;
	if (_actualRate != NULL)	
	*_actualRate = 0;

	/* Проверка наличия ЦАП */
	if (DAC_INACCESSIBLED_E440 == e440Desc.Dac.Active)
		return (FALSE);

    /* Получение текущих параметров работы ЦАП */
    if (!((ILE440 *)pModule)->GET_DAC_PARS(&dp))
        return false;

    /* Проверка корректности параметров */
    rate = (_rate > E440_MAX_DAC_FREQ) ? (E440_MAX_DAC_FREQ) : (_rate);

    /* Установка требуемых параметров ЦАП */
    dp.DacRate = rate;
	dp.DacFifoBaseAddress = 0x3000;
    dp.DacFifoLength = _fifoLength;
    dp.DacEnabled = 1;
	if (_actualRate != NULL)
		*_actualRate = rate;

    /* Передача параметров в плату */
    if (!((ILE440 *)pModule)->SET_DAC_PARS(&dp))
        return false;

    if (!((ILE440 *)pModule)->GET_DAC_PARS(&dp))
        return false;

	if (_actualRate != NULL)	
		*_actualRate = dp.DacRate;
    return true;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *  	Запуск передачи данных ЦАП в реальном времени.
 *
 * 	ОПИСАНИЕ:
 * 		_bufSize - размер буфера в двухбайтных словах
 * 		_packetSize - размер пакета.
 *
 * 		Внимание! Размер буфера должен быть кратен степени двойки. Если размер буфера заданный
 * 		пользователем не удоволетворяет этому условию, размер буфера будет установлен к ближайшему
 * 		кратному значению.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_StartRealTimeDacSampling(const DWORD _bufSize, const DWORD _packetSize)
{
	DWORD bufSize = 1;
	DWORD packetSize = _packetSize;

	if (_bufSize < 1024 || (_bufSize & 1))
        return false;

	while (1)
	{
		bufSize *= 2;
		if (bufSize >= _bufSize)
			break;
	}

    if (packetSize > bufSize / 2)
        packetSize = bufSize / 2;

    if (packetSize < 32)
        packetSize = 32;

    if (packetSize % 32)
    {
        packetSize = (packetSize / 32);
        packetSize = (packetSize + 1) * 32;
    }

	dacStopped = 0;
    isWriteThreadComplete = FALSE;
	writeThreadErrDetect = FALSE;
	hWriteThread = NULL;

    /* Выделение памяти под буфер */
    pDacBuf = new SHORT[bufSize];
    if (NULL == pDacBuf)
    {
        return (false);
    }

    /* Выделение памяти под пакет */
    pDacPacket = new SHORT[2 * packetSize];
    if (NULL == pDacPacket)
    {
        free(pDacBuf);
        return (false);
    }

    /* Выделение памяти под параметры буфера */
    pDacBufParam = new CIRBUF_Param;
    if (NULL == pDacBufParam)
    {
        free(pDacBuf);
        free(pDacPacket);
        return (false);
    }

    /* Инициализация буфера */
    CIRBUF_Init(bufSize, pDacBufParam);

	/* Критическая секция */
	InitializeCriticalSection(&wrCritSection);
	pDacBufParam->EnterCriticalSection = NULL;//f_WrEnterCritSect;
	pDacBufParam->LeaveCriticalSection = NULL;//f_WrLeaveCritSect;

    /* Создание и запуск потока отправки данных */
    hWriteThread = CreateThread(0, 0x2000, ServiceRealTimeWriteThread, 0, 0, &writeThreadId);
    //SetThreadPriority(hWriteThread, THREAD_PRIORITY_ABOVE_NORMAL);
    if (!hWriteThread)
    {
        free(pDacBuf);
        free(pDacPacket);
        free(pDacBufParam);
        return (false);
    }

    dacPacketSize = packetSize;

    if (writeThreadErrDetect)
        return (false);

    return (true);
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Остановка передачи данных в реальном времени.
 *
 * 	ОПИСАНИЕ:
 * 		Освобождается память занятая буферами.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_StopRealTimeDacSampling(void)
{
    dacStopped = 1;

    /* Ожидание завершения операций записи */
    for (; !isWriteThreadComplete;)
    {
        Sleep(20);
    }

    /* Ожидание завершения потока записи данных */
    WaitForSingleObject(hWriteThread, INFINITE);
    //TerminateThread(hWriteThread, 0);
    DeleteCriticalSection(&wrCritSection);

    /* Освобождение занятых ресурсов */
    CloseHandle(hWriteThread);
    hWriteThread = NULL;
    free(pDacBuf);
    free(pDacPacket);
    free(pDacBufParam);
    pDacBuf = NULL;
    pDacPacket = NULL;

    if (writeThreadErrDetect)
        return (false);

    return (true);
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Запись данных в ЦАП в реальном времени.
 *
 * 	ОПИСАНИЕ:
 * 		_pData указатель на данных:
 * 		_pointQnt - количество записываемых точек;
 * 		_pointProcessed - количество фактически записанных точек;
 * 		_pointFreeQnt - количество точек в буфере;
 * 		_timeout - таймаут операции записи, мс;
 * 		_underflow - флаг опустошения буфера.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_WriteRealTimeData(const SHORT *_pData,
                                                       const DWORD _pointQnt,
                                                       const DWORD _timeout,
                                                       DWORD *_pointProcessed,
                                                       DWORD *_pointFreeQnt,
                                                       BYTE *_underflow
                                                      )
{
    DWORD writeDataSize;
    DWORD restDataSize;
    DWORD timePassed = 0;

	if (_pData == NULL || _pointProcessed == NULL || _pointFreeQnt == NULL)
		return FALSE;
	
	if (_pointProcessed)
		*_pointProcessed = 0;
	if (_underflow)
		*_underflow = dacBufUnderflow;
    while (true)
    {
        /* Вычисление размера свободного места */
        *_pointFreeQnt = CIRBUF_GetEmptySize(pDacBufParam);

        /* Вычисление размера записываемых данных */
        restDataSize = _pointQnt - *_pointProcessed;

        /* Помещение данных в буфер */
        if (restDataSize)
        {
            /* Вычисление размера записываемых данных */
            writeDataSize = (restDataSize > *_pointFreeQnt) ? *_pointFreeQnt : restDataSize;

            /* Помещение данных в буфер */
            CIRBUF_PUSH_MULTIPLE_BACK(pDacBuf, (*pDacBufParam), (&_pData[*_pointProcessed]), writeDataSize);

            /* Вычисление количества переданных данных */
            *_pointProcessed += writeDataSize;

            /* Корректировка размера свободного места */
            *_pointFreeQnt -= writeDataSize;

            /* Eсли все данные помещены в буфер выходим */
            if (*_pointProcessed == _pointQnt)
                return (true);

            /* Проверка тайм-аута */
            timePassed += 10;
            if (timePassed > _timeout)
            {
                return (false);
            }
            Sleep(10);
        }
        else
            break;
    }

    return (true);
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Однократный вывод напряжения на ЦАП.
 *
 * 	ОПИСАНИЕ:
 * 		_voltage - устанавливаемое напряжение;
 * 		_dacNum - номер канала ЦАП;
 * 		_needCalibr - необходимость калибровки;
 *
 * 		Возвращает true, если операция прошла успешно, иначе false.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_SetDacVoltage(const double _voltage,
												   const WORD _dacNum,
												   const BYTE _needCalibr
												  )
{
	WORD maxDacCode;
	double maxDacAmp;
    SHORT val;

	if (pModule == NULL)
		return FALSE;
	
	maxDacCode = dacMaxCodes[moduleId];
	maxDacAmp = dacMaxAmplitudes[moduleId];

	/* Напряжение в кодах ЦАП */
	val = (SHORT)(_voltage * (maxDacCode / maxDacAmp));

	switch (moduleId)
	{
		case (E154_ID):
			if (_needCalibr == TRUE)
				val = (val + e154Desc.Dac.OffsetCalibration[_dacNum]) *
					  e154Desc.Dac.ScaleCalibration[_dacNum];
			return (BOOL) ((ILE154 *)pModule)->DAC_SAMPLE(&val, _dacNum);

		case (E14_140_ID):
			if (_needCalibr == TRUE)
				val = (val + e140Desc.Dac.OffsetCalibration[_dacNum]) *
					  e140Desc.Dac.ScaleCalibration[_dacNum];
			return (BOOL) ((ILE140 *)pModule)->DAC_SAMPLE(&val, _dacNum);

		case (E14_440_ID):
			if (_needCalibr == TRUE)
				val = (val + e440Desc.Dac.OffsetCalibration[_dacNum]) *
					  e440Desc.Dac.ScaleCalibration[_dacNum];
			return (BOOL) ((ILE440 *)pModule)->DAC_SAMPLE(&val, _dacNum);

		case (E2010_ID):
			if (_needCalibr == TRUE)
				val = (val + e2010Desc.Dac.OffsetCalibration[_dacNum]) *
					  e2010Desc.Dac.ScaleCalibration[_dacNum];
			return (BOOL) ((ILE2010 *)pModule)->DAC_SAMPLE(&val, _dacNum);

		default:
			return FALSE;
	}
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Преобразование значений отсчетов ЦАП в вольтах в коды и их калибровка.
 *
 * 	ОПИСАНИЕ:
 * 		_data - массив значений ЦАП в вольтах;
 * 		_size - количество точек;
 * 		_needCalibr - необходимость калибровки;
 * 		_result - массив результата.
 *
 * 		Возвращает true, если операция прошла успешно, иначе false.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_ProcessDacData(const double* _data,
													const DWORD _size,
													const BYTE _needCalibr,
													SHORT* _result
													)
{
    WORD dacNum;
    DWORD idx;
    double offset;
    double scale;
	WORD maxDacCode;
	double maxDacAmp;
	SHORT val;

	if (_data == NULL || _result == NULL)
		return FALSE;
	
	if (E14_140_ID == moduleId && moduleRev == 'B')
	{
		maxDacCode = E140_M_MAX_DAC_CODE;
	}
	else
		maxDacCode = dacMaxCodes[moduleId];
	maxDacAmp = dacMaxAmplitudes[moduleId];

    for (idx = 0; idx < _size; idx++)
    {
        /* Перевод значений в вольтах в коды */
		val = (SHORT)(_data[idx] * (maxDacCode / maxDacAmp));

        /* Калибровка */
        if (_needCalibr)
        {
            dacNum = idx % 2;

            /* Калибровка с учетом коэффициентов */
			switch (moduleId)
			{
				case (E154_ID):
					offset = e154Desc.Dac.OffsetCalibration[dacNum];
					scale = e154Desc.Dac.ScaleCalibration[dacNum];
					break;
		
				case (E14_140_ID):
					offset = e140Desc.Dac.OffsetCalibration[dacNum];
					scale = e140Desc.Dac.ScaleCalibration[dacNum];
					break;
		
				case (E14_440_ID):
					offset = e440Desc.Dac.OffsetCalibration[dacNum];
					scale = e440Desc.Dac.ScaleCalibration[dacNum];
					break;
		
				case (E2010_ID):
					offset = e2010Desc.Dac.OffsetCalibration[dacNum];
					scale = e2010Desc.Dac.ScaleCalibration[dacNum];
					break;
			}
            _result[idx] = (val + offset) * scale;
        }
		else
			_result[idx] = val;
    }

    return (TRUE);
}

/************************************************************************************************
 *																                                *								
 *                              DIGITAL I/O FUNCTIONS DEFINITIONS         					    *
 *																								*																							
 ***********************************************************************************************/

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Чтение входных линии внешнего цифрового разъёма.
 *
 * 	ОПИСАНИЕ:
 * 		_error - признак ошибки.
 *
 * 		Возвращает состояние первых 16 входов.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) WORD WINAPI LV_GetInputState(BYTE* _error)
{
    BOOL result;
    WORD ttl;

	if (pModule == NULL)
		return FALSE;
		
	switch (moduleId)
	{
		case (E154_ID):
			result = ((ILE154*)pModule)->TTL_IN(&ttl);
			break;

		case (E14_140_ID):
			result = ((ILE140*)pModule)->TTL_IN(&ttl);
			break;

		case (E14_440_ID):
			result = ((ILE440*)pModule)->TTL_IN(&ttl);
			break;

		case (E2010_ID):
			result = ((ILE2010*)pModule)->TTL_IN(&ttl);
			break;
	}
    if (result == FALSE)
    {
		if (_error != NULL)
			*_error = 0;
        return 0;
    }

	if (_error != NULL)
		*_error = 1;
    return (WORD) ttl;
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Установка состояния дискретных выходов.
 *
 * 	ОПИСАНИЕ:
 *
 * 		Возвращает true, если операция прошла успешно, иначе возвращает 0.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) BOOL WINAPI LV_SetOutputState(const WORD _state)
{
	if (pModule == NULL)
		return FALSE;
		
    if (!ttlOutEnabled)
    {
		switch (moduleId)
		{
			case (E154_ID):
				if (!((ILE154*)pModule)->ENABLE_TTL_OUT(true))
					return 0;
				break;
	
			case (E14_140_ID):
				if (!((ILE140*)pModule)->ENABLE_TTL_OUT(true))
					return 0;
				break;
	
			case (E14_440_ID):
				if (!((ILE440*)pModule)->ENABLE_TTL_OUT(true))
					return 0;
				break;
	
			case (E2010_ID):
				if (!((ILE2010*)pModule)->ENABLE_TTL_OUT(true))
					return 0;
				break;
		}
        ttlOutEnabled = TRUE;
    }

	switch (moduleId)
	{
		case (E154_ID):
			return (BOOL) ((ILE154*)pModule)->TTL_OUT(_state);

		case (E14_140_ID):
			return (BOOL) ((ILE140*)pModule)->TTL_OUT(_state);

		case (E14_440_ID):
			return (BOOL) ((ILE440*)pModule)->TTL_OUT(_state);

		case (E2010_ID):
			return (BOOL) ((ILE2010*)pModule)->TTL_OUT(_state);

      default:
      	return FALSE;
	}
}

/************************************************************************************************
 *																                                *								
 *                              	OTHER FUNCTIONS DEFINITIONS         					    *
 *																								*																							
 ***********************************************************************************************/


/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Поиск точки в массиве удоволетворяющей условию срабатывания триггера.
 *
 * 	ОПИСАНИЕ:
 * 		_pData - указатель на массив данных;
 * 		_dataSize - количество элементов в массиве;
 * 		_slope - тип синхронизации : 1 - по фронту, 0 - по спаду;
 * 		_trigLevel - уровень триггера, В;
 * 		_threshold - порог срабатывания триггера, В;
 * 		_detectVal - значение при котором сработал триггер, В
 * 		_isDetect - флаг срабывания триггера.
 *
 * 		Возвращает индекс элемента при значении которого сработал триггер.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) DWORD WINAPI LV_DetectTrigPointIdx(const double* _pData,
														 const DWORD _dataSize,
														 const BYTE _slope,
														 const double _trigLevel,
														 const double _threshold,
														 double* _detectVal,
														 BYTE* _isDetect
														 )
{
    DWORD idx;
    double val;
    double prevVal;

    *_detectVal = 0;
    *_isDetect = 0;

    /* Поиск значения удоволетворяющему условию срабатывания триггера.
     * Если установлена синхронизация по фронту сигнала, условием срабатывания
     * является превышение значения уровня триггера на величину большую или равную
     * порогу срабатывания, если синхронизация по спаду, условием является понижение
     * уровня сигнала на величину меньшую или равную значению порога.
     */
    prevVal = _pData[0];
    for (idx = 0; idx < _dataSize; idx++)
    {
        val = _pData[idx];

        /* Синхронизация по переднему фронту */
        if (_slope)
        {
            if (val >= (_trigLevel + _threshold) &&
				val > prevVal &&
				prevVal < _trigLevel
			   )
            {
                *_isDetect = 1;
                break;
            }
        }
        /* Синхронизация по заднему фронту */
        else
        {
            if (val <= (_trigLevel - _threshold) &&
				val < prevVal &&
				prevVal > _trigLevel
			   )
            {
                *_isDetect = 1;
                break;
            }
        }

        prevVal = val;
    }

    *_detectVal = val;

    if (*_isDetect)
    {
        return (idx);
    }
    else
    {
        return (0);
    }
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Алгоритм прореживания массива.
 *
 * 	ОПИСАНИЕ:
 * 		_pData - указатель на массив данных;
 * 		_dataSize - количество элементов в массиве;
 * 		_decimationFactor - коэффициент прореживания;
 * 		_pDecimatedData - массив результата.
 *-----------------------------------------------------------------------------------------------*/
__declspec(dllexport) VOID WINAPI LV_DecimateArray(const double* _pData,          	
												   const DWORD _dataSize,         	
												   const DWORD _decimationFactor,
												   double* _pDecimatedData
												   )
{
	DWORD idx;
	DWORD ctr;
	if (_decimationFactor < 1)
	{
		memcpy(_pDecimatedData, _pData, _dataSize * sizeof(double));	
		return;
	}

	idx = 0;
	ctr = 0;
	while (idx < _dataSize)
	{
		_pDecimatedData[ctr] = _pData[idx];
		idx += _decimationFactor;
		++ctr;
	}

	return;
}

/************************************************************************************************
 *																                                *								
 *                              	THREADS DEFINITIONS         							    *
 *																								*																							
 ***********************************************************************************************/

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Поток, в котором осуществляется сбор данных от АЦП.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
DWORD WINAPI ServiceRealTimeReadThread(PVOID)
{
    WORD RequestNumber;
    DWORD i;

    // идентификатор массива их двух событий
    HANDLE ReadEvent[RD_REQ_QNT];

    // массив OVERLAPPED структур из двух элементов
    OVERLAPPED ReadOv[RD_REQ_QNT];

    IO_REQUEST_LUSBAPI IoReq[RD_REQ_QNT];
	BOOL ok;
	double	adcRate;

    // остановим работу АЦП и одновременно сбросим USB-канал чтения данных
	switch (moduleId)
	{
		case (E154_ID):
			ok = ((ILE154*)pModule)->STOP_ADC();
			adcRate = e154AdcParam.AdcRate;
			break;

		case (E14_140_ID):
			ok = ((ILE140*)pModule)->STOP_ADC();
			adcRate = e140AdcParam.AdcRate;
			break;

		case (E14_440_ID):
			ok = ((ILE440*)pModule)->STOP_ADC();
			adcRate = e440AdcParam.AdcRate;
			break;

		case (E2010_ID):
			ok = ((ILE2010*)pModule)->STOP_ADC();
			adcRate = e2010AdcParam.AdcRate;
			break;
	}
    if (!ok)
    {
        readThreadErrDetect = TRUE;
        isReadThreadComplete = true;
        return 0x0;
    }

    // формируем необходимые для сбора данных структуры
    for (i = 0x0; i < RD_REQ_QNT; i++)
    {
        // создаём событие
        ReadEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

        // инициализация структуры типа OVERLAPPED
        ZeroMemory(&ReadOv[i], sizeof(OVERLAPPED));
        ReadOv[i].hEvent = ReadEvent[i];

        // формируем структуру IoReq
        IoReq[i].Buffer = pAdcPacket + i * adcPacketSize;
        IoReq[i].NumberOfWordsToPass = adcPacketSize;
        IoReq[i].NumberOfWordsPassed = 0x0;
        IoReq[i].Overlapped = &ReadOv[i];
        IoReq[i].TimeOut = adcPacketSize / adcRate + 2000;
    }

    // делаем предварительный запрос на ввод данных
    RequestNumber = 0x0;
	for (i = 0; i < RD_REQ_QNT; i++)
	{
		switch (moduleId)
		{
			case (E154_ID):
				ok = ((ILE154*)pModule)->ReadData(&IoReq[i]);
				break;
	
			case (E14_140_ID):
				ok = ((ILE140*)pModule)->ReadData(&IoReq[i]);
				break;
	
			case (E14_440_ID):
				ok = ((ILE440*)pModule)->ReadData(&IoReq[i]);
				break;
	
			case (E2010_ID):
				ok = ((ILE2010*)pModule)->ReadData(&IoReq[i]);
				break;
		}
	}
    if (!ok)
    {
		for (i = 0x0; i < RD_REQ_QNT; i++)
			CloseHandle(ReadEvent[i]);
        readThreadErrDetect = TRUE;
        isReadThreadComplete = TRUE;
        return 0x0;
    }

    // запустим АЦП
	switch (moduleId)
	{
		case (E154_ID):
			ok = ((ILE154*)pModule)->START_ADC();
			break;

		case (E14_140_ID):
			ok = ((ILE140*)pModule)->START_ADC();
			break;

		case (E14_440_ID):
			ok = ((ILE440*)pModule)->START_ADC();
			break;

		case (E2010_ID):
			ok = ((ILE2010*)pModule)->START_ADC();
			break;
	}
    if (ok)
    {
        // цикл сбора данных
		for (;;)
        {
            // ждём завершения операции сбора предыдущей порции данных
			for (;;)
			{
				if (adcStopped)
					break;

				if (WaitForSingleObject(ReadEvent[RequestNumber], IoReq[RequestNumber].TimeOut) ==
					 WAIT_TIMEOUT)
				{
					continue;
				}
				else
					break;
			}

        if (adcStopped)
         	break;

			/* Помещение принятых данных в буфер. Если данных принято
			 * больше чем свободного места в буфере затираются данные в голове буфера.
			 */
			CIRBUF_PUSH_MULTIPLE_BACK(pAdcBuf, (*pAdcBufParam), IoReq[RequestNumber].Buffer, adcPacketSize);

			/* Проверка буфера на переполнение */
			if (CIRBUF_DATA_LOST == CIRBUF_GetStatus(pAdcBufParam))
				adcBufOverflow = TRUE;

            if (readThreadErrDetect)
                break;
			else
			{
				// сделаем запрос на очередную порции данных
				switch (moduleId)
				{
					case (E154_ID):
						ok = ((ILE154*)pModule)->ReadData(&IoReq[RequestNumber]);
						break;
			
					case (E14_140_ID):
						ok = ((ILE140*)pModule)->ReadData(&IoReq[RequestNumber]);
						break;
			
					case (E14_440_ID):
						ok = ((ILE440*)pModule)->ReadData(&IoReq[RequestNumber]);
						break;
			
					case (E2010_ID):
						ok = ((ILE2010*)pModule)->ReadData(&IoReq[RequestNumber]);
						break;
				}
				if (!ok)
				{
					readThreadErrDetect = TRUE;
					break;
				}
				else
				{
					if (++RequestNumber >= RD_REQ_QNT)
						RequestNumber = 0;
				}
			}
        }
    }
    else
        readThreadErrDetect = TRUE;

    // остановим работу АЦП
	switch (moduleId)
	{
		case (E154_ID):
			ok = ((ILE154*)pModule)->STOP_ADC();
			break;

		case (E14_140_ID):
			ok = ((ILE140*)pModule)->STOP_ADC();
			break;

		case (E14_440_ID):
			ok = ((ILE440*)pModule)->STOP_ADC();
			break;

		case (E2010_ID):
			ok = ((ILE2010*)pModule)->STOP_ADC();
			break;
	}
    if (!ok)
        readThreadErrDetect = TRUE;

    // прервём возможно незавершённый асинхронный запрос на приём данных
    if (!CancelIo(moduleHandle))
        readThreadErrDetect = TRUE;

    // освободим все идентификаторы событий
    for (i = 0x0; i < RD_REQ_QNT; i++)
    {
        CloseHandle(ReadEvent[i]);
    }
	
	//CloseHandle(moduleHandle);

    // небольшая задержка
    Sleep(20);

    // установим флажок завершения работы потока сбора данных
    isReadThreadComplete = true;

    // теперь можно спокойно выходить из потока
    ExitThread(0);
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Поток, в котором осуществляется вывод данных на ЦАП.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
DWORD WINAPI ServiceRealTimeWriteThread(PVOID)
{
    DWORD i;
    WORD requestNum;

    /* Идентификаторы событий */
    HANDLE writeEvent[2];

    /* OVERLAPPED структуры */
    OVERLAPPED writeOv[2];

    /* Параметры запроса */
    IO_REQUEST_LUSBAPI ioReq[2];

	BOOL errNum;
	BOOL ok;

    /* Остановка работы ЦАП */
	switch (moduleId)
	{
		case (E154_ID):
			ok = 0;
			break;

		case (E14_140_ID):
			ok = ((ILE140*)pModule)->STOP_DAC();
			break;

		case (E14_440_ID):
			ok = ((ILE440*)pModule)->STOP_DAC();
			break;

		case (E2010_ID):
			ok = 0;
			break;

      default:
      	ok = 0;
	}
    if (!ok)
    {
        isWriteThreadComplete = true;
        return (0);
    }

	/* Заполнение FIFO буфера ЦАП в модуле E440 */
	if (E14_440_ID == moduleId)
	{
		errNum = ((ILE440*)pModule)->PUT_DM_ARRAY(e440DacParam.DacFifoBaseAddress,
												  e440DacParam.DacFifoLength,
												  pDacPacket
												 );
		if (!errNum)
		{
			writeThreadErrDetect = TRUE;
			isWriteThreadComplete = TRUE;
			return 1;
		}
	}

    /* Формирование данных структуры */
    for (i = 0x00; i < 0x2; i++)
    {
        writeEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        ZeroMemory(&writeOv[i], sizeof(OVERLAPPED));
        writeOv[i].hEvent = writeEvent[i];

        /* Заполнение структуры параметров запроса */
        ioReq[i].Buffer = pDacPacket + i * dacPacketSize;
        ioReq[i].NumberOfWordsToPass = dacPacketSize;
        ioReq[i].NumberOfWordsPassed = 0x0;
        ioReq[i].Overlapped = &writeOv[i];
        ioReq[i].TimeOut = 2000;
    }

    /* Инициализация данных пакета */
    memset(pDacPacket, 0x00, dacPacketSize * sizeof(pDacPacket[0]));

    /* Инициация передачи данных */
	switch (moduleId)
	{
		case (E154_ID):
			ok = 0;
			break;

		case (E14_140_ID):
			ok = ((ILE140*)pModule)->WriteData(&ioReq[0]);
			break;

		case (E14_440_ID):
			ok = ((ILE440*)pModule)->WriteData(&ioReq[0]);
			break;

		case (E2010_ID):
			ok = 0;
			break;

		default:
			ok = 0;
	}
    if (!ok)
    {
        CloseHandle(writeEvent[0]);
        CloseHandle(writeEvent[1]);
        writeThreadErrDetect = TRUE;
        isWriteThreadComplete = true;
        return (0);
    }

    /* Запуск ЦАП и потоковая передача данных в ЦАП */
	switch (moduleId)
	{
		case (E154_ID):
			ok = 0;
			break;

		case (E14_140_ID):
			ok = ((ILE140*)pModule)->START_DAC();
			break;

		case (E14_440_ID):
			ok = ((ILE440*)pModule)->START_DAC();
			break;

		case (E2010_ID):
			ok = 0;
			break;

		default:
			ok = 0;
	}

    if (ok)
    {
        requestNum = 0;
        while (1)
        {
            /* Запрос на запись очередного пакета */
            requestNum ^= 0x1;
			switch (moduleId)
			{
				case (E154_ID):
					ok = 0;
					break;
		
				case (E14_140_ID):
					ok = ((ILE140*)pModule)->WriteData(&ioReq[requestNum]);
					break;
		
				case (E14_440_ID):
					/* Добавление номера канала в каждый отсчет */
					for (i = 0; i < dacPacketSize; i++)
					{
						*(pDacPacket + requestNum*dacPacketSize + i) &= (WORD)(0xFFF);
						*(pDacPacket + requestNum*dacPacketSize + i) |= (WORD)((i % 2) << 12);
					}

					/* Запись данных */
					ok = ((ILE440*)pModule)->WriteData(&ioReq[requestNum]);
					break;
		
				case (E2010_ID):
					ok = 0;
					break;

				default:
					ok = 0;
			}
            if (!ok)
            {
                writeThreadErrDetect = TRUE;
                break;
            }

            /* Ожидание окончания записи очередного пакета */
            while (1)
            {
                if (dacStopped)
                    break;

                if (WaitForSingleObject(writeEvent[requestNum ^ 0x1], ioReq[requestNum ^ 0x1].TimeOut) ==
                    WAIT_TIMEOUT)
                    continue;
                else
                    break;
            }

            if (dacStopped)
            {
                break;
            }

            /* Анализ количества данных в буфере.
             * Если буфер пуст или количество данных меньше размера пакета,
             * устанавливается флаг опустошения буфера и ожидается поступление достаточного
             * количества данных в буфер.
             * Иначе из буфера извлекается новая порция данных.
             */
            while (1)
            {
				DWORD currDataSize;

                currDataSize = CIRBUF_GetDataSize(pDacBufParam);
                if (currDataSize >= dacPacketSize)
                {
                    /* Извлечение новой порции данных из буфера и помещение ее в пакет */
                    CIRBUF_POP_MULTIPLE_FRONT(pDacBuf,
                                              (*pDacBufParam),
                                              (pDacPacket + dacPacketSize * (requestNum ^ 0x1)),
                                              dacPacketSize
                                             );
                    dacBufUnderflow = 0;
                    break;
                }
                else if (!currDataSize)
                {
                    dacBufUnderflow = 1;
                }
				
				if (dacStopped)
                    break;
            }

            if (writeThreadErrDetect || dacStopped)
                break;
        }
    }
    else
        writeThreadErrDetect = TRUE;

    /* Остановка работы ЦАП */
	switch (moduleId)
	{
		case (E154_ID):
			ok = 0;
			break;

		case (E14_140_ID):
			ok = ((ILE140*)pModule)->STOP_DAC();
			break;

		case (E14_440_ID):
			ok = ((ILE440*)pModule)->STOP_DAC();
			break;

		case (E2010_ID):
			ok = 0;
			break;

		default:
			ok = 0;

	}
    if (!ok)
        writeThreadErrDetect = TRUE;

    /* Прерывание возможно незавершённый асинхронный запрос на запись данных */
    if (!CancelIo(((ILUSBBASE*)pModule)->GetModuleHandle()))
        writeThreadErrDetect = TRUE;

    /* Закрытие событий */
    for (i = 0x0; i < 0x2; i++)
    {
        CloseHandle(writeEvent[i]);
    }

    /* Небольшая задержка */
    Sleep(20);

    isWriteThreadComplete = true;
    return (0);
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Вход в критическую секцию для потока чтения.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
void f_RdEnterCritSect(void)
{
	EnterCriticalSection(&rdCritSection);
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Выход из критической секции для потока чтения.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
void f_RdLeaveCritSect(void)
{
	LeaveCriticalSection(&rdCritSection);
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Вход в критическую секцию для потока записи.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
void f_WrEnterCritSect(void)
{
	EnterCriticalSection(&wrCritSection);
}

/*-----------------------------------------------------------------------------------------------**
 * 	НАЗНАЧЕНИЕ:
 *		Выход из критической секции для потока записи.
 *
 * 	ОПИСАНИЕ:
 *-----------------------------------------------------------------------------------------------*/
void f_WrLeaveCritSect(void)
{
	LeaveCriticalSection(&wrCritSection);
}

/*===============================================================================================*/










