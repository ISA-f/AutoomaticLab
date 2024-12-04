#ifndef _LCommonInterface_H_
#define _LCommonInterface_H_

	#include "Lusbapi.h"

	// ==========================================================================
	// ************************* L-Card Common Interface ************************
	// ==========================================================================
	// дескриптор 'Приложения'(Firmware) микроконтроллера
	struct FIRMWARE_DESCRIPTOR
	{
		BOOL	Active;												// флаг достоверности остальных полей структуры
		VERSION_INFO_LUSBAPI Version;							// информация о версии прошивки основной программы 'Приложение'(Application) микроконтроллера
	};
	// дескриптор 'Загрузчика'(BootLoader) микроконтроллера
	struct BOOT_LOADER_DESCRIPTOR
	{
		BOOL	Active;												// флаг достоверности остальных полей структуры
		VERSION_INFO_LUSBAPI Version;							// информация о версии прошивки 'Загрузчика'(BootLoader) микроконтроллера
	};
	// дескриптор микроконтроллера
	struct MCU_DESCRIPTOR
	{
		BOOL	Active;												// флаг достоверности остальных полей структуры
		BYTE	Name[NAME_LINE_LENGTH_LUSBAPI];				// название микроконтроллера
		double	ClockRate;							 			// тактовая частота работы микроконтроллера в кГц
		FIRMWARE_DESCRIPTOR		FirmwareDescriptor;
		BOOT_LOADER_DESCRIPTOR	BootLoaderDescriptor;
		BYTE	Comment[COMMENT_LINE_LENGTH_LUSBAPI];		// строка комментария
	};
	// дескриптор драйвера DSP
	struct DSP_DESCRIPTOR
	{
		BOOL	Active;												// флаг достоверности остальных полей структуры
		DSP_INFO_LUSBAPI Dsp;									// информация о DSP
	};
	// дескриптор ПЛИС
	struct PLD_DESCRIPTOR
	{
		BOOL	Active;											// флаг достоверности остальных полей структуры
		PLD_INFO_LUSBAPI Pld;								// информация о ПЛИС
	};

	// структура с дескрипторами модуля
	struct MODULE_DESCRIPTORS
	{
		MODULE_INFO_LUSBAPI		ModuleDescriptor;
		MCU_DESCRIPTOR				McuDescriptor;
		DSP_DESCRIPTOR				DspDescriptor;
		PLD_DESCRIPTOR				PldDescriptor;
		ADC_INFO_LUSBAPI			AdcDescriptor;
		DAC_INFO_LUSBAPI			DacDescriptor;
	};

	// общий интерфейс USB устройств 
	class ILCOMMONINTERFACE
	{
		public :
			// конструктор/деструктор
			ILCOMMONINTERFACE(void);
			~ILCOMMONINTERFACE();

			// функции общего назначения для работы с USB устройствами
			BOOL WINAPI CreateLInstance(void);
			BOOL WINAPI OpenLDevice(WORD VirtualSlot);
			BOOL WINAPI CloseLDevice(void);
			BOOL WINAPI ReleaseLInstance(void);
			// получение дескриптора устройства USB
			HANDLE WINAPI GetModuleHandle(void);
			// получение названия используемого модуля
			BOOL WINAPI GetModuleName(PCHAR const ModuleName);
			// получение текущей скорости работы шины USB
			BOOL WINAPI GetUsbSpeed(BYTE * const UsbSpeed);
			// функция выдачи строки с последней ошибкой
			BOOL WINAPI GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo);

			// функции загрузки модуля
			BOOL WINAPI LOAD_MODULE(PCHAR const FileName = NULL);
			BOOL WINAPI TEST_MODULE(void);

			// функции для работы со служебной информацией из ППЗУ
			BOOL WINAPI GET_MODULE_DESCRIPTORS(MODULE_DESCRIPTORS * const ModuleDescriptors);

		private:
			// указатель на базовый интерфейс ILUSBBASE
			ILUSBBASE *pLusbbase;
			// указатель на интерфейс модуля E14-140
			ILE140 *pE140;
			// указатель на интерфейс модуля E154
			ILE154 *pE154;
			// указатель на интерфейс модуля E-310
			ILE310 *pE310;
			// указатель на интерфейс модуля E14-440
			ILE440 *pE440;
			// указатель на интерфейс модуля E20-10
			ILE2010 *pE2010;

			// разнобразные константы
			enum 	{	DEVICE_NAME_LENGTH					= 16 };
			// название и серийный номер USB модуля
			BYTE ModuleName[DEVICE_NAME_LENGTH];
	};

#endif		// _LCommonInterface_H_

