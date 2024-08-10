import Abstract_Device
import Measurements_Data
from typing import Callable

from lcomp.lcomp import LCOMP
from lcomp.ldevioctl import (E440, L_ADC_PARAM,
                             L_ASYNC_ADC_INP, L_ASYNC_DAC_OUT, L_ASYNC_TTL_CFG,
                             L_ASYNC_TTL_INP, L_ASYNC_TTL_OUT, L_EVENT_ADC_BUF,
                             L_STREAM_ADC, L_USER_BASE, WASYNC_PAR, WDAQ_PAR)
from lcomp.device import e440

import configparser
import threading
import time

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

'''
Перед использованием и модификацией классов Lcard на python,
прочитайте опыт неприятных ошибок:

Следующие методы LCOMP:
RequestBufferStream() 
SetParametersStream()
EnableCorrection()
StartLDevice()

Всегда должны находиться в одном Thread.
Нарушение этого правила приведет вас к синему экрану при запуске SetParametersStream
и 2 часам поиска ошибок без возможности дебага.
'''

class DeviceParameter:
    def __init__(self, device, par_name: str,
                 is_gettable: callable = (lambda dev: True),
                 get_value: callable = (lambda dev, name: getattr(dev, name)),
                 is_settable: callable = (lambda dev, val: False),
                 set_value: callable = (lambda dev, name, val: setattr(dev, name, val))):
        
        self.myDevice = device
        self.myParName = par_name
        
        self.myIsSettable = is_settable
        self.mySetValue = set_value

        self.myIsGettable = is_gettable
        self.myGetValue = get_value
        return

    def Get(self):
        print("DeviceParameter::Get")
        if (self.myIsGettable(self.myDevice)):
            return self.myGetValue(self.myDevice, self.myParName)
        return None
    
    def Set(self, new_value)-> bool:
        print("DeviceParameter::Set")
        if (self.myIsSettable(self.myDevice, new_value)):
            self.mySetValue(self.myDevice, self.myParName, new_value)
            return True
        print("DeviceParameter", self.myParName, "set: declined")
        return False 

class MultipleChoiceDeviceParameter(DeviceParameter):
    def __init__(self, device, par_name: str,
                 choice_to_value: {},
                 value_to_choice: {},
                 is_settable: callable = (lambda dev, val: True)):
        super().__init__(device, par_name, is_settable=is_settable)
        self.dChoiceToValue = dchoice_to_value
        self.dValueToChoice = dvalue_to_choice

    def Get(self):
        print("MultipleChoice::Get")
        k = super().Get()
        if(k in self.dValueToChoice.keys()):
            return self.dValueToChoice[k]
        print(k, "not found in values of", self.myParName)
        return None

    def Set(self, choise):
        print("MultipleChoice::Set")
        if(choice in self.dChoiceToValue.keys()):
            return super().Set(self.dChoiceToValue[choise])
        print(choice, "not found in choices of", self.myParName)
        return False 


class plDescrParameterE440(DeviceParameter):
    def __init__(self, device, par_name: str,
                 is_gettable: callable = (lambda dev: bool(dev.plDescr)),
                 get_value: callable = (lambda dev, name: getattr(dev.plDescr.t4, name))):
        super().__init__(
            device, par_name,
            is_gettable, get_value
            )
        return

class slotParameter(DeviceParameter):
    def __init__(self, device, par_name: str,
                 is_gettable = (lambda dev: bool(dev.slPar)),
                 get_value = (lambda dev, name: getattr(dev.slPar, name))):
        super().__init__(
            device, par_name,
            is_gettable, get_value
            )
        return

class adcParameterE440(DeviceParameter):
    def __init__(self, device, par_name, set_value : Callable,
                 is_gettable: callable = (lambda dev: bool(dev.adcPar)),
                 get_value: callable = (lambda dev, name: getattr(dev.adcPar.t3, name)),
                 is_settable: callable = (lambda dev, val: not(dev.MeasurementIsActive))):
        super().__init__(
            device, par_name,
            is_gettable, get_value,
            is_settable, set_value
            )
        return



class LcardE440_Autoread(Abstract_Device.Device):
    def __init__(self, config_filename: str):
        super().__init__(config_filename)
        self.buffer_size = None
        self.adcPar = None
        self.slPar = None
        self.ldev = None
        self.ThreadSleepTime = None
        self.myThread = None
        self.plDescr = None
        self.MeasurementIsActive = False
        self.data_ptr = None
        self.syncd = None
        self.__myData = Measurements_Data(np.zeros((1,7)), columns=['time','min','max','mean','var', 'linK', 'linB'])
        #self.rawData = []
        self.DotsPerHalfBuffer = None
        self.SavePeriodTime = None
        return

    @property
    def myData(self):
      return self.__myData.Data

    def ConnectToPhysicalDevice(self, slot: int = 0):
        self.ldev = LCOMP(slot)
        self.ldev.OpenLDevice()
        self.ldev.LoadBios("E440")
        
        print("Lcard E440 PlataTest: {}".format(self.ldev.PlataTest()))

        self.slPar = self.ldev.GetSlotParam()
        self.plDescr = self.ldev.ReadPlataDescr()
        self.adcPar = WDAQ_PAR()
        return

    def DisconnectFromPhysicalDevice(self):
        self.ldev.CloseLDevice()
        return
    
    def LoadConfiguration(self):
        f = open(self.ConfigFilename)
        config = configparser.ConfigParser()
        config.read_file(f)
        
        if (config["Validation"]["BoardType"] != "E440"):
            raise NameError("Lcard E440: invalid BoardType ini file: ")

        self.ThreadSleepTime = config["CodeControls"].getfloat("ThreadSleepTime")
        self.DotsPerHalfBuffer = config["CodeControls"].getint("DotsPerHalfBuffer")
        self.SavePeriodTime = config["CodeControls"].getfloat("SavePeriodTime")

        self.adcPar = WDAQ_PAR()
        self.adcPar.t3.s_Type = L_ADC_PARAM                       # Для E440:
        ADCpar = config["ADC_Parameters"]
        self.adcPar.t3.FIFO = ADCpar.getint("FIFO")               # 4096
        self.adcPar.t3.IrqStep = ADCpar.getint("IrqStep")         # 4096
        self.adcPar.t3.Pages = ADCpar.getint("Pages")             # 32
        self.adcPar.t3.AutoInit = ADCpar.getint("AutoInit")       # 0
        self.adcPar.t3.dRate = ADCpar.getfloat("dRate")           # 400.0
        self.adcPar.t3.dKadr = ADCpar.getfloat("dKadr")           # 0.0025

        self.adcPar.t3.SynchroType = e440.dSYNC_TYPE[ADCpar["SynchroType"]] # e440.NO_SYNC

        self.adcPar.t3.SynchroSensitivity = e440.dSYNC_SENSIVITY[ADCpar["SynchroSensitivity"]] # e440.A_SYNC_LEVEL

        self.adcPar.t3.SynchroMode = e440.dSYNC_MODE[ADCpar["SynchroMode"]] # e440.A_SYNC_UP_EDGE
                       
        self.adcPar.t3.AdChannel = ADCpar.getint("AdChannel")     # 0
        self.adcPar.t3.AdPorog = ADCpar.getint("AdPorog")         # 0
        self.adcPar.t3.NCh = ADCpar.getint("NCh")                 # 1 - 16

        self.adcPar.t3.Chn[0] = e440.CH_0 | e440.V10000 | e440.dCH_TYPE[ADCpar["Ch0Mode"]]
        # adcPar.t3.Chn[1] = e140.CH_1 | e140.V2500         # e440.CH_1 | e440.V2500    e154.CH_1 | e154.V1600
        # adcPar.t3.Chn[2] = e140.CH_2 | e140.V0625         # e440.CH_2 | e440.V0625    e154.CH_2 | e154.V0500
        # adcPar.t3.Chn[3] = e140.CH_3 | e140.V0156         # e440.CH_3 | e440.V0156    e154.CH_3 | e154.V0160
        self.adcPar.t3.IrqEna = ADCpar.getint("IrqEna")
        self.adcPar.t3.AdcEna = ADCpar.getint("AdcEna")
        return

    def StartMeasurements(self, measurements_file: str):
        if(self._MeasurementsFile):
            self.FinishMeasurements() #если есть активная запись в другой файл, завершим её

        self._MeasurementsFile = open(measurements_file, "ab") # может стоит вынести в Abstract_Device

        self.myThread = threading.Thread(target = self.TakeMeasurements,
                                         daemon = False) # считыванием данных будет заниматься отдельный поток
        self.ldev.FillDAQparameters(self.adcPar.t3)
        self.MeasurementIsActive = True
        self.myThread.start()
        return 

    def TakeMeasurements(self):
        #RequestBufferStream и SetParametersStream неразлучны,
        #вплоть до смерти вашего компа во время исполнения
        
        self.buffer_size = self.ldev.RequestBufferStream(size=131072, stream_id=L_STREAM_ADC)
        half_buffer = self.buffer_size // 2
        self.data_ptr, self.syncd = self.ldev.SetParametersStream(self.adcPar.t3, self.buffer_size)
        self.ldev.EnableCorrection(True)
        self.ldev.InitStartLDevice()
        self.ldev.StartLDevice()
        self._MeasurementsFile.write(b"time min max mean var lin1 lin2\n")
        ComputerTime = time.time()
        NextSaveTime = ComputerTime + self.SavePeriodTime
        previous_syncd = 0

        while(self.MeasurementIsActive):
            if((previous_syncd % half_buffer) <= (self.syncd() % half_buffer)): 
                previous_syncd = self.syncd()
                time.sleep(self.ThreadSleepTime) # ожидаем заполнения следующего полубуффера
            else:
                CurrentTime = time.time()
                current_syncd = syncd()
                x = e440.GetDataADC(self.adcPar.t3, self.plDescr, # считываем буфер с Lcard
                                    self.data_ptr, self.buffer_size) 

                print("End of sleep", current_syncd, " - ", previous_syncd)
                PreviousBufferEndTime = CurrentBufferEndTime #будем заносить время в измерения
                first_half = (previous_syncd <= half_buffer)
                if(first_half):
                        df = x[0][:half_buffer] # выбираем нужный полубуфер
                        CurrentBufferEndTime = PreviousBufferEndTime + (CurrentTime - PreviousBufferEndTime)*65536/current_syncd
                else:
                        df = x[0][half_buffer:]
                        CurrentBufferEndTime = PreviousBufferEndTime + (CurrentTime - PreviousBufferEndTime)*65536/(current_syncd + 65536)
                #self.rawData.append(df)
                df = df.reshape(-1, half_buffer//self.DotsPerHalfBuffer)
                DataPiece = np.concatenate(( # DataPiece - те данные, что мы будем сохранять
                    np.linspace(PreviousBufferEndTime, CurrentBufferEndTime, self.DotsPerHalfBuffer).reshape(1,-1),
                    np.array([np.min(df, axis = 1),
                              np.max(df, axis = 1), # вычисляем нужные характеристики
                              np.mean(df, axis = 1),
                              np.var(df, axis = 1)]),
                    np.polyfit(x = np.arange(half_buffer//self.DotsPerHalfBuffer),
                               y = df.T,
                               deg = 1)),
                    axis = 0).T
                
                self.myData = pd.concat( # добавляем новые данные в DataFrame
                    [self.myData,
                    pd.DataFrame(
                        DataPiece,
                        columns = list(self.myData))
                     ],
                    axis = 0,
                    ignore_index = True)
                

                self._MeasurementsFile.write(b"\n")
                np.savetxt(self._MeasurementsFile, DataPiece) # сохраняем данные в объекте файла
                if(NextSaveTime <= CurrentTime):
                    self._MeasurementsFile.flush() # периодически отправляем новые данные в оперативку
                    NextSaveTime = ComputerTime + self.SavePeriodTime
                # иногда в основном потоке нужно вызывать os.fsync()
                # чтобы дозаписать все файлы на жесткие диски
                
                previous_syncd = 0
        print("Measurements Finished")
        return

    def FinishMeasurements(self):
        print("Finishing Measurements")
        self.MeasurementIsActive = False #говорим дочернему потоку, что пора заканчивать
        
        self.myThread.join() #ожидаем завершения дочернего потока
        self._MeasurementsFile.close()  #стоит вынести в Abstract_Device
        
        self._MeasurementsFile = None
        self.myThread = None
        return

    def GetDeviceParameters(self):
        self.DeviceParameters = [
            DeviceParameter(self, "ThreadSleepTime", is_settable = (lambda dev, val: val>0),
                            get_value = (lambda dev, name: dev.ThreadSleepTime)),
            DeviceParameter(self, "SavePeriodTime", is_settable = (lambda dev, val: val>0)),
            DeviceParameter(self, "DotsPerHalfBuffer", is_settable = (lambda dev, val: (val>0 and val%1==0 and dev.buffer_size > val))),
            DeviceParameter(self, "MeasurementIsActive"),
            DeviceParameter(self, "buffer_size"),
            DeviceParameter(self, "myData"),
            
            slotParameter(self, "Base"),
            slotParameter(self, "BaseL"),
            slotParameter(self, "Base1"),
            slotParameter(self, "BaseL1"),
            slotParameter(self, "Mem"),
            slotParameter(self, "MemL"),
            slotParameter(self, "Mem1"),
            slotParameter(self, "MemL1"),
            slotParameter(self, "Irq"),
            slotParameter(self, "BoardType"),
            slotParameter(self, "DSPType"),
            slotParameter(self, "Dma"),
            slotParameter(self, "DmaDac"),
            slotParameter(self, "DTA_REG"),
            slotParameter(self, "IDMA_REG"),
            slotParameter(self, "CMD_REG"),
            slotParameter(self, "IRQ_RST"),
            slotParameter(self, "DTA_ARRAY"),
            slotParameter(self, "RDY_REG"),
            slotParameter(self, "CFG_REG"),
            
            plDescrParameterE440(self, "SerNum"),
            plDescrParameterE440(self, "BrdName"),
            plDescrParameterE440(self, "Rev"),
            plDescrParameterE440(self, "DspType"),
            plDescrParameterE440(self, "IsDacPresent"),
            plDescrParameterE440(self, "Quartz"),
            
            adcParameterE440(self, "AutoInit",
                            set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "AutoInit", bool(val)))),            
            adcParameterE440(self, "FIFO", is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (1 <= val <= 4096))),
                            set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "FIFO", int(val)))),
            adcParameterE440(self, "IrqStep", is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (1 <= val <= 4096))),
                            set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "IrqStep", int(val)))),
            adcParameterE440(self, "Pages", is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (1 <= val <= 32))),
                            set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "IrqStep", int(val)))),            
            adcParameterE440(self, "dRate", is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (0 < val <= 400.0))),
                             set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "dRate", float(val)))),
            adcParameterE440(self, "dKadr", is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (0.0025 <= val < dev.adcPar.t3.dRate/4))),
                             set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "dRate", float(val)))),
            
            adcParameterE440(self, "AdChannel", is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (0 <= val <= 16))),
                            set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "AdChannel", int(val)))),
            adcParameterE440(self, "AdPorog", is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (0 <= val <= 16))),
                            set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "AdPorog", int(val)))),
            adcParameterE440(self, "IrqEna",
                            set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "IrqEna", bool(val)))),
            adcParameterE440(self, "AdcEna",
                            set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "AdcEna", bool(val)))),
            #adcParameterE440(self, "NCh", is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (1 <= val <= 16))),
            #                set_value = (lambda dev, name, val: setattr(dev.adcPar.t3, "NCh", int(val))))
                            ]
        """
        self.adcPar.t3.s_Type = L_ADC_PARAM                       # Для E440:

        self.adcPar.t3.SynchroType = e440.dSYNC_TYPE[ADCpar["SynchroType"]] # e440.NO_SYNC
        self.adcPar.t3.SynchroSensitivity = e440.dSYNC_SENSIVITY[ADCpar["SynchroSensitivity"]] # e440.A_SYNC_LEVEL
        self.adcPar.t3.SynchroMode = e440.dSYNC_MODE[ADCpar["SynchroMode"]] # e440.A_SYNC_UP_EDGE

        self.adcPar.t3.Chn[0] = e440.CH_0 | e440.V10000 | e440.dCH_TYPE[ADCpar["Ch0Mode"]]
        # adcPar.t3.Chn[1] = e140.CH_1 | e140.V2500         # e440.CH_1 | e440.V2500    e154.CH_1 | e154.V1600
        # adcPar.t3.Chn[2] = e140.CH_2 | e140.V0625         # e440.CH_2 | e440.V0625    e154.CH_2 | e154.V0500
        # adcPar.t3.Chn[3] = e140.CH_3 | e140.V0156         # e440.CH_3 | e440.V0156    e154.CH_3 | e154.V0160
        """
        return self.DeviceParameters

if __name__ == "__main__":
    print("threads: ", threading.active_count())
    myLcard = LcardE440_Autoread("LcardE440.ini")
    myLcard.ConnectToPhysicalDevice(slot=0)
    """
    myLcard.ThreadSleepTime = 10
    pars = myLcard.GetDeviceParameters()
    myLcard.ThreadSleepTime = 20
    print(pars[0].Get())
    print(pars[0].Set(15))
    print(pars[0].Get())
    print(pars[0].Set(-15))
    print(pars[0].Get())
    print(pars[0].Set(30.4))
    print(pars[0].Get())
    for par in pars:
        print(par.myParName, "-", par.Get())
    """
    myLcard.LoadConfiguration()
    print("threads: ", threading.active_count())
    myLcard.StartMeasurements("auto_channel1.log")
    time.sleep(10)
    myLcard.FinishMeasurements()
        
    Data = myLcard.myData[1:]
    plt.scatter(Data['time'], Data['mean'], s = 1, c = 'g')
    plt.scatter(Data['time'], Data['mean'] - np.sqrt(Data['var']), s = 1, c = 'orange')
    plt.scatter(Data['time'], Data['mean'] + np.sqrt(Data['var']), s = 1, c = 'orange')
    plt.scatter(Data['time'], Data['min'], s = 1, c = 'r')
    plt.scatter(Data['time'], Data['max'], s = 1, c = 'r')
    plt.show()
        #Raw = np.concatenate(myLcard.rawData, axis = 0)
        #print(Raw.shape)
        #plt.scatter(np.arange(len(Raw)), Raw, s = 1)
        #plt.show()
