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

#ifndef __WLUSBAPI__
#define __WLUSBAPI__
#include <windows.h>
#include <wtypes.h>


/* Версия библиотеки */
#define  WE140LC_VERMAJOR                       2       		// только одна цифра
#define  WE140LC_VERMINOR                       1               // только одна цифра
#define  WE140LC_CURRENT_VERSION    ((WE140LC_VERMAJOR<<16)|WE140LC_VERMINOR)

#ifdef __cplusplus
extern "C" 
{
#endif

/************************************************************************************************
 *																                                *								
 *                               	 COMMON FUNCTIONS DEFINITIONS       	   				    *
 *																								*																							
 ***********************************************************************************************/
__declspec(dllexport) DWORD WINAPI LV_GetDllVersion(VOID);
__declspec(dllexport) BOOL WINAPI LV_OpenModule(PCHAR _moduleName, BYTE* _moduleIdx);
__declspec(dllexport) VOID WINAPI LV_CloseModule(VOID);


/************************************************************************************************
 *																                                *								
 *                               	 ADC FUNCTIONS DEFINITIONS       	    				    *
 *																								*																							
 ***********************************************************************************************/
__declspec(dllexport) BOOL WINAPI LV_ProcessSingleAdcSample(const SHORT _sampleCode, 
															const WORD _chNum, 
															const BYTE _needCalibr, 
															const BYTE _transToVolt,
															double* _result
															);
__declspec(dllexport) BOOL WINAPI LV_ProcessAdcArray(const SHORT* _pSampleData, 
													 const DWORD _size, 
													 const BYTE _needCalibr, 
													 const BYTE _transToVolt,
													 double* _pResultData 
												    );
__declspec(dllexport) WORD WINAPI LV_CreateAdcChannel(const WORD _chNum, 
													  const WORD _gain, 
													  const WORD _diffMode
													  );
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
	);

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
	);

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
	);

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
    );

__declspec(dllexport) BOOL WINAPI LV_StartRealTimeAdcSampling(const DWORD _bufSize, const DWORD _packetSize);
__declspec(dllexport) BOOL WINAPI LV_StopRealTimeAdcSampling(void);
__declspec(dllexport) BOOL WINAPI LV_GetRealTimeAdcData(SHORT* _pData, 																												
														const DWORD _sampleQnt, 
														const DWORD _timeoutMs,
														const BYTE _waitToComplete,
                                                        DWORD* _pProcessedSampleQnt, 
														DWORD* _restSampleQnt,
                                                        BYTE* _overflow 
													    );
__declspec(dllexport) double WINAPI LV_DoSingleAdcSample(const WORD _chNum, const BYTE _needCalibr, BYTE* _error);


/************************************************************************************************
 *																                                *								
 *                               	 DAC FUNCTIONS DEFINITIONS       	    				    *
 *																								*																							
 ***********************************************************************************************/
__declspec(dllexport) BOOL WINAPI LV_E140_SetDacSyncParameters(const double _rate,
                                                               const BYTE _syncWithAdc,
                                                               const BYTE _zeroOnStop,
                                                               double* _actualRate
                                                              );
__declspec(dllexport) BOOL WINAPI LV_E440_SetDacSyncParameters(const double _rate,
                                                               const WORD _fifoLength,
                                                               double* _actualRate
                                                              );
__declspec(dllexport) BOOL WINAPI LV_StartRealTimeDacSampling(const DWORD _bufSize, const DWORD _packetSize);
__declspec(dllexport) BOOL WINAPI LV_StopRealTimeDacSampling(void);
__declspec(dllexport) BOOL WINAPI LV_WriteRealTimeData(const SHORT *_pData, 
                                                       const DWORD _pointQnt,
                                                       const DWORD _timeout,
                                                       DWORD *_pointProcessed,
                                                       DWORD *_pointFreeQnt,
                                                       BYTE *_underflow
                                                      );
__declspec(dllexport) BOOL WINAPI LV_SetDacVoltage(const double _voltage,
												   const WORD _dacNum, 
												   const BYTE _needCalibr
												  );
__declspec(dllexport) BOOL WINAPI LV_ProcessDacData(const double* _data,
													const DWORD _size, 
													const BYTE _needCalibr,
													SHORT* _result
													);

/************************************************************************************************
 *																                                *								
 *                              DIGITAL I/O FUNCTIONS DECLARATIONS         					    *
 *																								*																							
 ***********************************************************************************************/
__declspec(dllexport) WORD WINAPI LV_GetInputState(BYTE* _error);
__declspec(dllexport) BOOL WINAPI LV_SetOutputState(const WORD _state);


/************************************************************************************************
 *																                                *								
 *                              	OTHER FUNCTIONS DECLARATIONS         					    *
 *																								*																							
 ***********************************************************************************************/
__declspec(dllexport) DWORD WINAPI LV_DetectTrigPointIdx(const double* _pData,        
														 const DWORD _dataSize,         
														 const BYTE _slope,             
														 const double _trigLevel,       
														 const double _threshold,      
														 double* _detectVal,            
														 BYTE* _isDetect               
														 );
__declspec(dllexport) VOID WINAPI LV_DecimateArray(const double* _pData,          	
												   const DWORD _dataSize,         	
												   const DWORD _decimationFactor,        
												   double* _pDecimatedData
												   );

#ifdef __cplusplus
}
#endif
#endif
/*===============================================================================================*/
