import Abstract_Device

from lcomp.lcomp import LCOMP
from lcomp.ldevioctl import (E2010, E2010B, L_ADC_PARAM,
                             L_ASYNC_ADC_INP, L_ASYNC_DAC_OUT, L_ASYNC_TTL_CFG,
                             L_ASYNC_TTL_INP, L_ASYNC_TTL_OUT, L_EVENT_ADC_BUF,
                             L_STREAM_ADC, L_USER_BASE, WASYNC_PAR, WDAQ_PAR)
from lcomp.device import e2010

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

class LcardE2010B_Autoread(Abstract_Device.Device):
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
        self.myData = pd.DataFrame(np.zeros((1,7)), columns=['time','min','max','mean','var', 'linK', 'linB'])
        #self.rawData = []
        self.DotsPerHalfBuffer = 1
        self.SavePeriodTime = 10
        return

    def ConnectToPhysicalDevice(self, slot: int = 0):
        self.ldev = LCOMP(slot)
        self.ldev.OpenLDevice()
        self.ldev.LoadBios("e2010m")
        
        print("Lcard E2010 PlataTest: {}".format(self.ldev.PlataTest()))

        self.slPar = self.ldev.GetSlotParam()
        self.plDescr = self.ldev.ReadPlataDescr()
        return

    def DisconnectFromPhysicalDevice(self):
        self.ldev.CloseLDevice()
        return
    
    def LoadConfiguration(self):
        f = open(self.ConfigFilename)
        config = configparser.ConfigParser()
        config.read_file(f)
        
        if (config["Validation"]["BoardType"] != "E2010B"):
            raise NameError("Lcard E2010B: invalid BoardType ini file: ")

        self.ThreadSleepTime = config["CodeControls"].getfloat("ThreadSleepTime")
        self.DotsPerHalfBuffer = config["CodeControls"].getint("DotsPerHalfBuffer")
        self.SavePeriodTime = config["CodeControls"].getfloat("SavePeriodTime")

        ADCpar = config["ADC_Parameters"]
        self.adcPar = WDAQ_PAR()
        print(type(self.adcPar))
        self.adcPar.t4.s_Type = L_ADC_PARAM                                     # Для E2010B:
        self.adcPar.t4.FIFO = ADCpar.getint("FIFO")                             # 4096
        self.adcPar.t4.IrqStep = ADCpar.getint("IrqStep")                       # 4096
        self.adcPar.t4.Pages = ADCpar.getint("Pages")                           # 32
        self.adcPar.t4.AutoInit = ADCpar.getint("AutoInit")                     # 1
        self.adcPar.t4.dRate = ADCpar.getfloat("dRate")                         # 1000.0
        self.adcPar.t4.dKadr = ADCpar.getfloat("dKadr")                         # 0.001
        self.adcPar.t4.SynchroType = e2010.dSynchroType[ADCpar["SynchroType"]]  # e2010.INT_START_TRANS
        self.adcPar.t4.SynchroSrc = e2010.dSynchroSrc[ADCpar["SynchroSrc"]]     # e2010.INT_CLK_TRANS
        
        MaskPar = ADCpar["AdcIMask"].split()
        self.adcPar.t4.AdcIMask = e2010.dCH_BITS[MaskPar[0]]
        for j in range(1, len(MaskPar)):
            self.adcPar.t4.AdcIMask = self.adcPar.t4.AdcIMask | e2010.dCH_BITS[MaskPar[j]]
            # | e2010.SIG_1 | e2010.V10_1 # | e2010.SIG_2 | e2010.V03_2 # | e2010.GND_3
        
        self.adcPar.t4.NCh = ADCpar.getint("NCh")                               # 1 - 4
        for i in range(self.adcPar.t4.NCh):
            self.adcPar.t4.Chn[i] = e2010.dChn[i]                               # e2010.CH_0
        self.adcPar.t4.IrqEna = ADCpar.getint("IrqEna")                         # 1
        self.adcPar.t4.AdcEna = ADCpar.getint("AdcEna")                         # 1

        self.ldev.FillDAQparameters(self.adcPar.t4)
        return

    def StartMeasurements(self, measurements_file: str):
        if(self._MeasurementsFile):
            self.FinishMeasurements() #если есть активная запись в другой файл, завершим её

        self._MeasurementsFile = open(measurements_file, "ab") # ab - append - режим добавления строк в файл

        self.myThread = threading.Thread(target = self.TakeMeasurements,
                                         daemon = False) # считыванием данных будет заниматься отдельный поток
        self.MeasurementIsActive = True
        self.myThread.start()
        return 

    def TakeMeasurements(self):
        """
        RequestBufferStream, SetParametersStream и GetDataADC неразлучно
        должны существовать в одном потоке
        вплоть до смерти вашего компа во время исполнения
        """
        
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
                print("sleep ", previous_syncd)
                time.sleep(self.ThreadSleepTime) # ожидаем заполнения следующего полубуффера
            else:
                first_half = (previous_syncd <= half_buffer)
                print(first_half, self.syncd(), " - ", previous_syncd)
                x = e2010.GetDataADC(self.adcPar.t4, self.plDescr, # считываем буффер с Lcard
                                    self.data_ptr, self.buffer_size) 
                
                PreviousTime = ComputerTime #будем заносить время в измерения
                ComputerTime = time.time()
                if(first_half):
                        df = x[0][:half_buffer] # выбираем нужный полубуффер
                else:
                        df = x[0][half_buffer:]
                #self.rawData.append(df)
                df = df.reshape(-1, half_buffer//self.DotsPerHalfBuffer)
                DataPiece = np.concatenate(( # DataPiece - те данные, что мы будем сохранять
                    np.linspace(PreviousTime, ComputerTime, self.DotsPerHalfBuffer).reshape(1,-1),
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
                if(NextSaveTime <= ComputerTime):
                    self._MeasurementsFile.flush() # периодически отправляем новые данные в оперативку
                    NextSaveTime = ComputerTime + self.SavePeriodTime
                    print("Lcard E2010: measurements flushed")
                # иногда в основном потоке нужно вызывать os.fsync()
                # чтобы дозаписать все файлы на жесткие диски
                
                previous_syncd = 0

        
        self.ldev.StopLDevice()
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

    def GetReadonlyParameters(self):
        Parameters = {"Base" : self.slPar.Base,
                      "BaseL" : self.slPar.BaseL,
                      "Base1" : self.slPar.Base1,
                      "BaseL1" : self.slPar.BaseL1,
                      "Mem" : self.slPar.Mem,
                      "Irq" : self.slPar.Irq,
                      "BoardType" : self.slPar.BoardType,
                      "DSPType" : self.slPar.DSPType,
                      "Dma" : self.slPar.Dma,
                      "DmaDac" : self.slPar.DmaDac,
                      "DTA_REG" : self.slPar.DTA_REG,
                      "IDMA_REG" : self.slPar.IDMA_REG,
                      "CMD_REG" : self.slPar.CMD_REG,
                      "IRQ_RST" : self.slPar.IRQ_RST,
                      "DTA_ARRAY" : self.slPar.DTA_ARRAY,
                      "RDY_REG" : self.slPar.RDY_REG,
                      "CFG_REG" : self.slPar.CFG_REG,
                      "SerNum" : self.plDescr.t6.SerNum,
                      "BrdName" : self.plDescr.t6.BrdName,
                      "Rev" : self.plDescr.t6.Rev,
                      "DspType" : self.plDescr.t6.DspType,
                      "IsDacPresent" : self.plDescr.t6.IsDacPresent,
                      # "KoefADC" : self.plDescr.t6.KoefADC,
                      # "KoefDAC" : self.plDescr.t6.KoefDAC,
                      "Quartz" : self.plDescr.t6.Quartz
                      }


        return Parameters

    def GetChangeableParameters(self):
        if (self.MeasurementIsActive):
            return {
        "ThreadSleepTime" : self.ThreadSleepTime,
        "DotsPerHalfBuffer" : self.DotsPerHalfBuffer,
        "SavePeriodTime" : self.SavePeriodTime
        }
        """
        else:
        ADCpar = config["ADC_Parameters"]
        self.adcPar = WDAQ_PAR()
        print(type(self.adcPar))
        self.adcPar.t4.s_Type = L_ADC_PARAM                                     # Для E2010B:
        self.adcPar.t4.FIFO = ADCpar.getint("FIFO")                             # 4096
        self.adcPar.t4.IrqStep = ADCpar.getint("IrqStep")                       # 4096
        self.adcPar.t4.Pages = ADCpar.getint("Pages")                           # 32
        self.adcPar.t4.AutoInit = ADCpar.getint("AutoInit")                     # 1
        self.adcPar.t4.dRate = ADCpar.getfloat("dRate")                         # 1000.0
        self.adcPar.t4.dKadr = ADCpar.getfloat("dKadr")                         # 0.001
        self.adcPar.t4.SynchroType = e2010.dSynchroType[ADCpar["SynchroType"]]  # e2010.INT_START_TRANS
        self.adcPar.t4.SynchroSrc = e2010.dSynchroSrc[ADCpar["SynchroSrc"]]     # e2010.INT_CLK_TRANS
        
        MaskPar = ADCpar["AdcIMask"].split()
        self.adcPar.t4.AdcIMask = e2010.dCH_BITS[MaskPar[0]]
        for j in range(1, len(MaskPar)):
            self.adcPar.t4.AdcIMask = self.adcPar.t4.AdcIMask | e2010.dCH_BITS[MaskPar[j]]
            # | e2010.SIG_1 | e2010.V10_1 # | e2010.SIG_2 | e2010.V03_2 # | e2010.GND_3
        
        self.adcPar.t4.NCh = ADCpar.getint("NCh")                               # 1 - 4
        for i in range(self.adcPar.t4.NCh):
            self.adcPar.t4.Chn[i] = e2010.dChn[i]                               # e2010.CH_0
        self.adcPar.t4.IrqEna = ADCpar.getint("IrqEna")                         # 1
        self.adcPar.t4.AdcEna = ADCpar.getint("AdcEna") 
            return {
                
                }

    """
        
    def GetReadOnlyParameters(d : {}):
        return

if __name__ == "__main__":
        print("threads: ", threading.active_count())
        myLcard = LcardE2010B_Autoread("LcardE2010B.ini")
        myLcard.ConnectToPhysicalDevice(slot=0)
        k = myLcard.GetChangeableParameters()
        k["DotsPerHalfBuffer"] = 24
        print(myLcard.GetChangeableParameters()["DotsPerHalfBuffer"])
        """
        myLcard.LoadConfiguration()
        print("threads: ", threading.active_count())
        myLcard.StartMeasurements("auto_channel2010.log")
        time.sleep(10)
        myLcard.FinishMeasurements()
        
        Data = myLcard.myData[1:]
        plt.plot(Data['time'], Data['mean'], linewidth = 1, c = 'g')
        plt.scatter(Data['time'], Data['mean'] - np.sqrt(Data['var']), s = 1, c = 'orange')
        plt.scatter(Data['time'], Data['mean'] + np.sqrt(Data['var']), s = 1, c = 'orange')
        #plt.scatter(Data['time'], Data['min'], s = 1, c = 'r')
        #plt.scatter(Data['time'], Data['max'], s = 1, c = 'r')
        plt.show()
        #Raw = np.concatenate(myLcard.rawData, axis = 0)
        #print(Raw.shape)
        #plt.scatter(np.arange(len(Raw)), Raw, s = 1)
        #plt.show()"""
