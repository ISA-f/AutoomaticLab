#ifndef _LCommonInterface_H_
#define _LCommonInterface_H_

	#include "Lusbapi.h"

	// ==========================================================================
	// ************************* L-Card Common Interface ************************
	// ==========================================================================
	// ���������� '����������'(Firmware) ����������������
	struct FIRMWARE_DESCRIPTOR
	{
		BOOL	Active;												// ���� ������������� ��������� ����� ���������
		VERSION_INFO_LUSBAPI Version;							// ���������� � ������ �������� �������� ��������� '����������'(Application) ����������������
	};
	// ���������� '����������'(BootLoader) ����������������
	struct BOOT_LOADER_DESCRIPTOR
	{
		BOOL	Active;												// ���� ������������� ��������� ����� ���������
		VERSION_INFO_LUSBAPI Version;							// ���������� � ������ �������� '����������'(BootLoader) ����������������
	};
	// ���������� ����������������
	struct MCU_DESCRIPTOR
	{
		BOOL	Active;												// ���� ������������� ��������� ����� ���������
		BYTE	Name[NAME_LINE_LENGTH_LUSBAPI];				// �������� ����������������
		double	ClockRate;							 			// �������� ������� ������ ���������������� � ���
		FIRMWARE_DESCRIPTOR		FirmwareDescriptor;
		BOOT_LOADER_DESCRIPTOR	BootLoaderDescriptor;
		BYTE	Comment[COMMENT_LINE_LENGTH_LUSBAPI];		// ������ �����������
	};
	// ���������� �������� DSP
	struct DSP_DESCRIPTOR
	{
		BOOL	Active;												// ���� ������������� ��������� ����� ���������
		DSP_INFO_LUSBAPI Dsp;									// ���������� � DSP
	};
	// ���������� ����
	struct PLD_DESCRIPTOR
	{
		BOOL	Active;											// ���� ������������� ��������� ����� ���������
		PLD_INFO_LUSBAPI Pld;								// ���������� � ����
	};

	// ��������� � ������������� ������
	struct MODULE_DESCRIPTORS
	{
		MODULE_INFO_LUSBAPI		ModuleDescriptor;
		MCU_DESCRIPTOR				McuDescriptor;
		DSP_DESCRIPTOR				DspDescriptor;
		PLD_DESCRIPTOR				PldDescriptor;
		ADC_INFO_LUSBAPI			AdcDescriptor;
		DAC_INFO_LUSBAPI			DacDescriptor;
	};

	// ����� ��������� USB ��������� 
	class ILCOMMONINTERFACE
	{
		public :
			// �����������/����������
			ILCOMMONINTERFACE(void);
			~ILCOMMONINTERFACE();

			// ������� ������ ���������� ��� ������ � USB ������������
			BOOL WINAPI CreateLInstance(void);
			BOOL WINAPI OpenLDevice(WORD VirtualSlot);
			BOOL WINAPI CloseLDevice(void);
			BOOL WINAPI ReleaseLInstance(void);
			// ��������� ����������� ���������� USB
			HANDLE WINAPI GetModuleHandle(void);
			// ��������� �������� ������������� ������
			BOOL WINAPI GetModuleName(PCHAR const ModuleName);
			// ��������� ������� �������� ������ ���� USB
			BOOL WINAPI GetUsbSpeed(BYTE * const UsbSpeed);
			// ������� ������ ������ � ��������� �������
			BOOL WINAPI GetLastErrorInfo(LAST_ERROR_INFO_LUSBAPI * const LastErrorInfo);

			// ������� �������� ������
			BOOL WINAPI LOAD_MODULE(PCHAR const FileName = NULL);
			BOOL WINAPI TEST_MODULE(void);

			// ������� ��� ������ �� ��������� ����������� �� ����
			BOOL WINAPI GET_MODULE_DESCRIPTORS(MODULE_DESCRIPTORS * const ModuleDescriptors);

		private:
			// ��������� �� ������� ��������� ILUSBBASE
			ILUSBBASE *pLusbbase;
			// ��������� �� ��������� ������ E14-140
			ILE140 *pE140;
			// ��������� �� ��������� ������ E154
			ILE154 *pE154;
			// ��������� �� ��������� ������ E-310
			ILE310 *pE310;
			// ��������� �� ��������� ������ E14-440
			ILE440 *pE440;
			// ��������� �� ��������� ������ E20-10
			ILE2010 *pE2010;

			// ������������ ���������
			enum 	{	DEVICE_NAME_LENGTH					= 16 };
			// �������� � �������� ����� USB ������
			BYTE ModuleName[DEVICE_NAME_LENGTH];
	};

#endif		// _LCommonInterface_H_

