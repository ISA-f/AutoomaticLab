import configparser
import time
import numpy as np
import pandas as pd
from PyQt5.QtCore import QTimer

import Abstract_Device

from lcomp.lcomp import LCOMP
from lcomp.ldevioctl import (E2010, E2010B, L_ADC_PARAM,
                             L_ASYNC_ADC_INP, L_ASYNC_DAC_OUT, L_ASYNC_TTL_CFG,
                             L_ASYNC_TTL_INP, L_ASYNC_TTL_OUT, L_EVENT_ADC_BUF,
                             L_STREAM_ADC, L_USER_BASE, WASYNC_PAR, WDAQ_PAR)
from lcomp.device import e2010
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

class LcardE2010B_PeriodicCall(object):
    def __init__(self, config_filename: str):
        super().__init__()#config_filename)
        self.ConfigFilename = config_filename
        self.buffer_size = None
        self.adcPar = None
        self.slPar = None
        self.ldev = None
        self.plDescr = None
        self.data_ptr = None
        self.syncd = None
        self.IsActiveMeasurements = False
        self.IsConnected = False
        self.myData = pd.DataFrame(columns = ['time','buffer_size',
                                              'mean0', 'var0', 'min0', 'max0',
                                              'mean1', 'var1', 'min1', 'max1'])
        self.timer = None
        self.TimerSleepTime = None
        self._MeasurementsFile = None
        return

    def ConnectToPhysicalDevice(self, slot: int = 0):
        print("Try connect to Lcard")
        try:
            self.ldev = LCOMP(slot)
            self.ldev.OpenLDevice()
            self.ldev.LoadBios("e2010m")
            self.IsConnected = self.ldev.PlataTest()
            print("Connect to Lcard E2010. PlataTest: {}".format(self.IsConnected))

            self.slPar = self.ldev.GetSlotParam()
            self.plDescr = self.ldev.ReadPlataDescr()
        except Exception as e:
            print(e)
        return

    def DisconnectFromPhysicalDevice(self):
        if self.ldev:
            self.ldev.CloseLDevice()
            print("Lcard disconnected")
        self.IsConnected = False
        return
    
    def LoadConfiguration(self):
        if not(self.IsConnected):
            return
        f = open(self.ConfigFilename)
        config = configparser.ConfigParser()
        config.read_file(f)
        
        if (config["Validation"]["BoardType"] != "E2010B"):
            raise NameError("Lcard E2010B: invalid BoardType ini file: ")

        self.TimerSleepTime = int(config["CodeControls"].getfloat("TimerSleepTime")*1000)
        self.DotsPerHalfBuffer = config["CodeControls"].getint("DotsPerHalfBuffer")
        
        ADCpar = config["ADC_Parameters"]
        self.adcPar = WDAQ_PAR()
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

    def ReadBuffer(self):
        if not(self.IsConnected):
            return
        if not(self.IsActiveMeasurements):
            return
        """
        RequestBufferStream, SetParametersStream и GetDataADC неразлучно
        должны существовать в одном Thread
        вплоть до смерти вашего компа во время исполнения
        """
        k = self.syncd()
        x = e2010.GetDataADC(self.adcPar.t4, self.plDescr, # считываем буффер с Lcard
                            self.data_ptr, self.buffer_size)
        return x, k


    def TakeMeasurements(self, requested_buffer_size):
            if not(self.IsConnected):
                return pd.Series([None]*10)
            if not(self.IsActiveMeasurements):
                return pd.Series([None]*10)
            
            end = self.syncd() // self.adcPar.t4.NCh
            x = e2010.GetDataADC(self.adcPar.t4, self.plDescr, # считываем буффер с Lcard
                            self.data_ptr, self.buffer_size)
            current_time = time.time()
            start = max(end - requested_buffer_size, 0)
            
            DataPiece1 = np.array([time.time(),    #0
                                  end - start],    #1
                                  dtype = object)
            
            #DataPiece = self._ndarray_to_DataPiece(x.T[start:end].T, current_time)
            #return pd.Series(DataPiece)
            
            if x.shape[0] > 0:
                data0 = x[0][start:end]
                DataPiece2 = np.array([np.mean(data0), #2
                                       np.var(data0),  #3
                                       np.min(data0),  #4
                                       np.max(data0)], #5
                                      dtype = object)
            else:
                DataPiece2 = np.array([None, None, None, None], dtype = object)
            if x.shape[0] > 1:
                data1 = x[1][start:end]
                DataPiece3 = np.array([np.mean(data1), #6
                                       np.var(data1),  #7
                                       np.min(data1),  #8
                                       np.max(data1),  #9
                                       np.mean(data1 - data0), #10
                                       np.var(data1 - data0),  #11
                                       np.min(data1 - data0),  #12
                                       np.max(data1 - data0)], #13
                                      dtype = object)
            else:
                DataPiece3 = np.array([None, None, None, None], dtype = object)
            
            return pd.Series(np.concatenate([DataPiece1, DataPiece2, DataPiece3]))

    def _ndarray_to_DataPiece(x, current_time):
        DataPiece0 = np.array([current_time, x.size]) #0 is current_time
                                                      #1 is x.size
        if x.shape[0] > 0:                         
            data0 = x[0]                           
            DataPiece1 = np.array([np.mean(data0),    #2
                                   np.var(data0),     #3
                                   np.min(data0),     #4
                                   np.max(data0)],    #5
                                  dtype = object)
        else:
            DataPiece1 = np.array([None, None, None, None], dtype = object)

        if x.shape[0] > 1:
            data1 = x[1][start:end]
            DataPiece2 = np.array([np.mean(data1),    #6
                                   np.var(data1),     #7
                                   np.min(data1),     #8
                                   np.max(data1),     #9
                                   np.mean(data1 - data0), #10
                                   np.var(data1 - data0),  #11
                                   np.min(data1 - data0),  #12
                                   np.max(data1 - data0)], #13
                                  dtype = object)
        else:
            DataPiece2 = np.array([None, None, None, None, None, None, None, None], dtype = object)

        return np.concatenate([DataPiece0, DataPiece1, DataPiece2])


        
    def StartMeasurements(self, measurements_file: str):
        if not(self.IsConnected):
            return
        if self.IsActiveMeasurements:
            return
        if(self._MeasurementsFile):
            self.FinishMeasurements() #если есть активная запись в другой файл, завершим её
        #self._MeasurementsFile = open(measurements_file, "ab") # ab - append - режим добавления строк в файл
        
        self.buffer_size = self.ldev.RequestBufferStream(size=131072, stream_id=L_STREAM_ADC)
        #self.half_buffer = self.buffer_size // 2
        self.data_ptr, self.syncd = self.ldev.SetParametersStream(self.adcPar.t3, self.buffer_size)
        self.ldev.EnableCorrection(True)
        self.ldev.InitStartLDevice()
        self.ldev.StartLDevice()
        self.IsActiveMeasurements = True
        self.NextSaveTime = 0

        #self.timer = QTimer()
        #self.timer.timeout.connect(self.ContinuouslyReadBuffer)
        #self.timer.start(self.TimerSleepTime)
        #self.previous_syncd = 0
        self.ComputerTime = time.time()
        return

    def ContinuouslyReadBuffer(self):
        """ Здесь мы будем ожидать заполнения одной из половин буфера,
        и при ее заполнении считывать и запоминать ту часть буфера, куда
        не ведется запись"""
        print("called Lcard.ContinuouslyReadBuffer()")
        # Lcard имеет указатель на место текущей записи - self.syncd() 
        if((self.previous_syncd % self.half_buffer) <= (self.syncd() % self.half_buffer)): 
                self.previous_syncd = self.syncd()
                # проверка на то, не прошел ли указатель одну из половин буфера
                # если нет, ожидаем заполнения следующего полубуффера
                print("Lcard.ContinuoslyReadBuffer waiting")
                return 
        print("... executed")
        #выбираем, какой полубуфер сейчас не изменяется
        first_half = (self.previous_syncd <= self.half_buffer)
        print(first_half, self.syncd(), " - ", self.previous_syncd)

        # считываем полный буфер с Lcard
        x = e2010.GetDataADC(self.adcPar.t4, self.plDescr, self.data_ptr, self.buffer_size)
        print("executed GetDataADC")
        # сразу же постараемся записать время, когда мы считали буфер
        PreviousTime = self.ComputerTime
        self.ComputerTime = time.time()
        print("... executed")
        # выбираем нужный полубуфер
        if(first_half):
                df = x[0][:self.half_buffer]
        else:
                df = x[0][self.half_buffer:]
        df = df.reshape(-1, self.half_buffer//self.DotsPerHalfBuffer)
        print("... executed")
        # DataPiece - те данные, что мы будем сохранять
        # Из сырых данных вычисляем характеристики, сохранять будем только их
        DataPiece = np.concatenate((
            np.linspace(PreviousTime, ComputerTime, self.DotsPerHalfBuffer).reshape(1,-1),
            np.array([np.min(df, axis = 1),
                      np.max(df, axis = 1),
                      np.mean(df, axis = 1),
                      np.var(df, axis = 1)]),
            np.polyfit(x = np.arange(half_buffer//self.DotsPerHalfBuffer),y = df.T, deg = 1)),
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
        if(self.NextSaveTime <= ComputerTime):
            self._MeasurementsFile.flush() # периодически отправляем новые данные в оперативной памяти, в объекте файла
            self.NextSaveTime = ComputerTime + self.SavePeriodTime
            print("Lcard E2010: measurements flushed")
            # иногда в основном потоке нужно вызывать os.fsync()
            # чтобы дозаписать все файлы на жесткие диски
        print("end of ContinuoslyReadBuffer")
        return

    def FinishMeasurements(self):
        if self._MeasurementsFile:
            self._MeasurementsFile.close()
            self._MeasurementsFile = None
        if self.ldev:
            self.ldev.StopLDevice()
        self.IsActiveMeasurements = False
        return



if __name__ == "__main__":
        myLcard = LcardE2010B_PeriodicCall("LcardE2010B.ini")
        myLcard.ConnectToPhysicalDevice(slot=0)
        myLcard.LoadConfiguration()
        
        myLcard.StartMeasurements("Lcard_filename.log")
        print(time.time())
        time.sleep(3)
        #for i in range(10000):
        #    x = myLcard.TakeMeasurements()
        print(time.time())
        myLcard.FinishMeasurements()

