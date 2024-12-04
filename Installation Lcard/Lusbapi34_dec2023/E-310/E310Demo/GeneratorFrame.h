//---------------------------------------------------------------------------
#ifndef GeneratorFrameH
#define GeneratorFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "LMDBaseEdit.hpp"
#include "LMDCustomBevelPanel.hpp"
#include "LMDCustomButtonGroup.hpp"
#include "LMDCustomControl.hpp"
#include "LMDCustomEdit.hpp"
#include "LMDCustomGroupBox.hpp"
#include "LMDCustomPanel.hpp"
#include "LMDCustomPanelFill.hpp"
#include "LMDCustomParentPanel.hpp"
#include "LMDCustomRadioGroup.hpp"
#include "LMDEdit.hpp"
#include "LMDGroupBox.hpp"
#include "LMDRadioGroup.hpp"
#include "LMDLabel.hpp"
#include "LMDButton.hpp"
#include "LMDComboBox.hpp"
#include "LMDCustomButton.hpp"
#include "LMDCustomComboBox.hpp"
#include "LMDBaseControl.hpp"
#include "LMDBaseGraphicControl.hpp"
#include "LMDBaseLabel.hpp"
#include "LMDControl.hpp"
#include "LMDCustomLabel.hpp"
#include "LMDLabel.hpp"
#include "LMDButtonControl.hpp"
#include "LMDCheckBox.hpp"
#include "LMDCustomCheckBox.hpp"
#include "Lusbapi.h"
#include "LMDThemedComboBox.hpp"

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class TE310GeneratorFrame : public TFrame
{
__published:	// IDE-managed Components
	TLMDGroupBox *MasterClockLMDGroupBox;
	TLMDRadioGroup *MasterClockSourceLMDRadioGroup;
	TLMDLabel *InternalMasterClockValueLMDLabel;
	TLMDLabel *MasterClockUnitLMDLabel;
	TLMDEdit *ExternalMasterClockValueLMDEdit;
	TLMDGroupBox *AnalogOutputsParsLMDGroupBox;
	TLMDRadioGroup *AnalogOutputsTypeLMDRadioGroup;
	TLMDGroupBox *AnalogOutputsGaindBLMDGroupBox;
	TLMDLabel *AnalogOutputsGaindBLMDLabel;
	TLMDComboBox *AnalogOutputsGaindBLMDComboBox;
	TLMDLabel *AnalogOutputsGaindBLabelLMDLabel;
	TLMDGroupBox *Output10OhmParsLMDGroupBox;
	TLMDGroupBox *OffsetOutput10OhmLMDGroupBox;
	TLMDRadioGroup *OffsetTypeOutput10OhmLMDRadioGroup;
	TLMDGroupBox *FrequencyParsLMDGroupBox;
	TLMDLabel *FinalFrequencyValueLMDLabel;
	TLMDLabel *FinalFrequencyUnitLMDLabel;
	TLMDLabel *FinalFrequencyLabelLMDLabel;
	TLMDLabel *NumberOfIncrementsLabelLMDLabel;
	TLMDEdit *NumberOfIncrementsValueLMDEdit;
	TLMDGroupBox *IncrementIntervalParsLMDGroupBox;
	TLMDEdit *BaseIntervalsNumberLMDEdit;
	TLMDLabel *DurationLMDLabel;
	TLMDLabel *DurationUnitLMDLabel;
	TLMDRadioGroup *CyclicAutoScanLMDRadioGroup;
	TLMDGroupBox *DigitalLinesParsLMDGroupBox;
	TLMDRadioGroup *CtrlLineTypeLMDRadioGroup;
	TLMDRadioGroup *InterrupLineTypeLMDRadioGroup;
	TLMDGroupBox *SyncoutLineParsLMDGroupBox;
	TLMDCheckBox *EnaSyncoutLMDCheckBox;
	TLMDRadioGroup *SyncoutTypeLMDRadioGroup;
	TLMDRadioGroup *IncrementTypeLMDRadioGroup;
	TLMDGroupBox *BaseIntervalParsLMDGroupBox;
	TLMDRadioGroup *BaseIntervalMultiplierLMDRadioGroup;
	TLMDRadioGroup *BaseIntervalTypeLMDRadioGroup;
	TLMDLabel *BaseIntervalsNumberLabelLMDLabel;
	TLMDGroupBox *MiscellaneousLMDGroupBox;
	TLMDCheckBox *EnaSquareWaveOutputLMDCheckBox;
	TLMDGroupBox *StartFrequencyLMDGroupBox;
	TLMDEdit *DesiredStartFrequencyValueLMDEdit;
	TLMDLabel *StartFrequencyUnitLMDLabel;
	TLMDLabel *ActualStartFrequencyValueLMDLabel;
	TLMDLabel *DesiredStartFrequencyLabelLMDLabel;
	TLMDLabel *ActualStartFrequencyLMDLabel;
	TLMDGroupBox *FrequencyIncrementLMDGroupBox;
	TLMDLabel *DesiredFrequencyIncrementLabelLMDLabel;
	TLMDEdit *DesiredFrequencyIncrementValueLMDEdit;
	TLMDLabel *FrequencyIncrementsUnitLMDLabel;
	TLMDLabel *ActualFrequencyIncrementLabeLMDLabel;
	TLMDLabel *ActualFrequencyIncrementLMDLabel;
	TLMDGroupBox *OffsetValueLMDGroupBox;
	TLMDGroupBox *Output50OhmParsLMDGroupBox;
	TLMDLabel *Output50OhmAmplitudeLMDLabel;
	TLMDLabel *ApmlitudeOutput50OhmIndBLMDLabel;
	TLMDLabel *LMDLabel2;
	TLMDLabel *ApmlitudeOutput50OhmInVLMDLabel;
	TLMDLabel *LMDLabel4;
	TLMDLabel *Output10OhmAmplitudeLMDLabel;
	TLMDLabel *ApmlitudeOutput10OhmIndBLMDLabel;
	TLMDLabel *ApmlitudeOutput10OhmIndBLabelLMDLabel;
	TLMDLabel *ApmlitudeOutput10OhmInVLMDLabel;
	TLMDLabel *ApmlitudeOutput10OhmInVLabelLMDLabel;
	TLMDLabel *OffsetValueOutput10OhmLabelLMDLabel;
	TLMDLabel *LMDLabel5;
	TLMDLabel *ActualOffsetValueOutput10OhmLMDLabel;
	TLMDLabel *ActualOffsetValueOutput10OhmLabelLMDLabel;
	TLMDEdit *DesiredOffsetValueOutput10OhmLMDEdit;
	TLMDButton *ControlGeneratorLMDButton;
	TLMDLabel *DurationLabelLMDLabel;
	void __fastcall MasterClockSourceLMDRadioGroupChange(TObject *Sender, int ButtonIndex);
	void __fastcall CyclicAutoScanLMDRadioGroupChange(TObject *Sender, int ButtonIndex);
	void __fastcall IncrementTypeLMDRadioGroupChange(TObject *Sender, int ButtonIndex);
	void __fastcall CtrlLineTypeLMDRadioGroupChange(TObject *Sender, int ButtonIndex);
	void __fastcall InterrupLineTypeLMDRadioGroupChange(TObject *Sender, int ButtonIndex);
	void __fastcall EnaSyncoutLMDCheckBoxChange(TObject *Sender);
	void __fastcall SyncoutTypeLMDRadioGroupChange(TObject *Sender, int ButtonIndex);
	void __fastcall EnaSquareWaveOutputLMDCheckBoxChange(TObject *Sender);
	void __fastcall DesiredStartFrequencyValueLMDEditKeyPress(TObject *Sender, char &Key);
	void __fastcall DesiredStartFrequencyValueLMDEditChange(TObject *Sender);
	void __fastcall DesiredFrequencyIncrementValueLMDEditKeyPress( TObject *Sender, char &Key);
	void __fastcall DesiredFrequencyIncrementValueLMDEditChange( TObject *Sender);
	void __fastcall NumberOfIncrementsValueLMDEditKeyPress(TObject *Sender, char &Key);
	void __fastcall NumberOfIncrementsValueLMDEditChange(TObject *Sender);
	void __fastcall ExternalMasterClockValueLMDEditKeyPress(TObject *Sender, char &Key);
	void __fastcall ExternalMasterClockValueLMDEditChange(TObject *Sender);
	void __fastcall BaseIntervalTypeLMDRadioGroupChange(TObject *Sender, int ButtonIndex);
	void __fastcall BaseIntervalMultiplierLMDRadioGroupChange(TObject *Sender, int ButtonIndex);
	void __fastcall BaseIntervalsNumberLMDEditKeyPress(TObject *Sender, char &Key);
	void __fastcall BaseIntervalsNumberLMDEditChange(TObject *Sender);
	void __fastcall OffsetTypeOutput10OhmLMDRadioGroupChange(TObject *Sender, int ButtonIndex);
	void __fastcall DesiredOffsetValueOutput10OhmLMDEditKeyPress( TObject *Sender, char &Key);
	void __fastcall DesiredOffsetValueOutput10OhmLMDEditChange( TObject *Sender);
	void __fastcall AnalogOutputsTypeLMDRadioGroupChange(TObject *Sender, int ButtonIndex);
	void __fastcall AnalogOutputsGaindBLMDComboBoxChange(TObject *Sender);
	void __fastcall ControlGeneratorLMDButtonClick(TObject *Sender);

	private:	// User declarations
		void InitAnalogOutputsGaindBComboBox(void);
		void GetControlElementState(void);
		void ControlElements(bool EnabledFlag);
		void WinControlElements(TWinControl *WinControlElement, bool EnabledFlag);
		void DesiredStartFrequencyDiplay(bool Flag);
		void DesiredDeltaFrequencyDiplay(bool Flag);
		void IncrementIntervalDiplay(bool Flag);
		bool CalculateStopFrequency(void);
		void CalculateIncrementInterval(void);
		void DesiredOffsetValueDiplay(bool Flag);
		void CalculateAnalogOutputsGains(void);
		void GetGeneratorPars(void);

		// флажок реентерабельности
		bool IsReenteringInProgress;
		// текущее состо€ние генератора: работает или остановлен
		BYTE GeneratorState;
		// возможные состо€ни€ генератора
		enum { GENERATOR_IS_OFF, GENERATOR_IS_ON };

		// указатель на интерфейс модул€ E-310
		ILE310 *pModule;
		// различные параметры работы модул€
		GENERATOR_PARS_E310 GeneratorPars;
		// максимальный код частоты DDS
		DWORD MAX_FREQ_DDS_CODE;
		// минимаксные значени€ частоты тактирующего сигнала генератора в к√ц
		double MIN_MASTER_CLOCK, MAX_MASTER_CLOCK;
		// частота внутреннего тактирующего сигнала генератора в к√ц
		double INTERNAL_MASTER_CLOCK;
		// массив умножителей интервала приращени€
		double IncrementIntervalMultiplierArray[0x4];
		// массив усилений выходного тракта генератора в дЅ
		double GeneratorGaindBArray[ANALOG_OUTPUT_GAINS_QUANTITY_E310];
		// массив доступных аплитуд на 10 ќм выхода в ¬
		double Output10OhmVArray[ANALOG_OUTPUT_GAINS_QUANTITY_E310];
		// массив доступных амплитуд на выходе 10 ќм в дЅ
		double Output10OhmdBArray[ANALOG_OUTPUT_GAINS_QUANTITY_E310];
		// массив доступных аплитуд на 50 ќм выхода в ¬
		double Output50OhmVArray[ANALOG_OUTPUT_GAINS_QUANTITY_E310];
		// массив доступных амплитуд на выходе 50 ќм в дЅ
		double Output50OhmdBArray[ANALOG_OUTPUT_GAINS_QUANTITY_E310];

		// актуальные величины
		double ActualStartFrequency, ActualDeltaFrequency, ActualStopFrequency;
		double ActualOffsetValue;

		WORD ControlElementsQuantity;
		TControl **ControlElementsAddr;
		bool *ControlElementsState;

//		int DebugArray[0x10];

	public:		// User declarations
		__fastcall TE310GeneratorFrame(TComponent* Owner);
		__fastcall ~TE310GeneratorFrame();

		void InitGeneratorFramePars(ILE310 * const pModule, GENERATOR_PARS_E310 * const GeneratorPars);
		void GetGeneratorFramePars(GENERATOR_PARS_E310 * const GeneratorPars);
};
//---------------------------------------------------------------------------
extern PACKAGE TE310GeneratorFrame *E310GeneratorFrame;
//---------------------------------------------------------------------------
#endif
