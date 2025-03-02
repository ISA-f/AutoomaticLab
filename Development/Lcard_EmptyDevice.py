import configparser
import time
import numpy as np

from lcomp.lcomp import LCOMP
from lcomp.ldevioctl import (E2010, E2010B, L_ADC_PARAM,
                             L_ASYNC_ADC_INP, L_ASYNC_DAC_OUT, L_ASYNC_TTL_CFG,
                             L_ASYNC_TTL_INP, L_ASYNC_TTL_OUT, L_EVENT_ADC_BUF,
                             L_STREAM_ADC, L_USER_BASE, WASYNC_PAR, WDAQ_PAR)
from lcomp.device import e2010
from threading import Lock

'''
Перед использованием и модификацией классов Lcard на python,
прочитайте опыт неприятных ошибок:

Следующие методы LCOMP:
RequestBufferStream() 
SetParametersStream()
EnableCorrection()
StartLDevice()

Всегда должны находиться в одном Thread.
Нарушение этого правила приводит вас к синему экрану при запуске SetParametersStream
и 2 часам поиска ошибок без возможности дебага.
'''

class LcardE2010B_EmptyDevice(object):
    def __init__(self, config_filename: str):
        super().__init__()
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
        self.BufferMutex = Lock()
        self.SyncdMutex = Lock()
        self.ListenersAmount = 0
        return

    def connectToPhysicalDevice(self, slot: int = 0):    
        print("Try connect to Lcard")
        if self.IsConnected:
            print("Already connected to Lcard")
            return True
        try:
            self.ldev = LCOMP(slot)
            self.ldev.OpenLDevice()
            self.ldev.LoadBios("e2010m")
            self.IsConnected = self.ldev.PlataTest()
            print("Connect to Lcard E2010. PlataTest: {}".format(self.ldev.PlataTest()))

            self.slPar = self.ldev.GetSlotParam()
            self.plDescr = self.ldev.ReadPlataDescr()
        except Exception as e:
            print(e)
            return False
        return True

    def disconnectFromPhysicalDevice(self):
        self.finishMeasurements()
        if self.ldev and self.IsConnected:
            self.ldev.CloseLDevice()
            print("Lcard disconnected")
        self.IsConnected = False
        return
    
    def loadConfiguration(self):
        if not(self.IsConnected):
            return
        if self.IsActiveMeasurements:
            print("Lcard: tried to LoadConfiguration while IsActiveMeasurements == True")
            return
        
        f = open(self.ConfigFilename)
        config = configparser.ConfigParser()
        config.read_file(f)
        
        if (config["Validation"]["BoardType"] != "E2010B"):
            raise NameError("Lcard E2010B: invalid BoardType ini file: ")
        
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
        f.close()
        self.ldev.FillDAQparameters(self.adcPar.t4)
        return

    def startMeasurements(self):
        if not(self.IsConnected):
            self.connectToPhysicalDevice()
            self.loadConfiguration()
        if not(self.IsConnected):
            return
        if self.IsActiveMeasurements:
            return
        self.IsActiveMeasurements = True
        self.buffer_size = self.ldev.RequestBufferStream(size=131072, stream_id=L_STREAM_ADC)
        self.data_ptr, self.syncd = self.ldev.SetParametersStream(self.adcPar.t3, self.buffer_size)
        self.ldev.EnableCorrection(True)
        self.ldev.InitStartLDevice()
        self.ldev.StartLDevice()
        return

    def finishMeasurements(self):
        if not(self.IsConnected):
            return
        if self.ldev:
            print("ldev.StopLDevice() call")
            self.ldev.StopLDevice()
        self.IsActiveMeasurements = False
        return

    def readBuffer(self):
        if not(self.IsConnected) or not(self.IsActiveMeasurements):
            print(f"Lcard.IsConnected = {self.IsConnected}. Lcard.IsActiveMeasurements = {self.IsActiveMeasurements}.Tried Lcard:readBuffer()")
            return None, None
        self.BufferMutex.acquire()
        self.SyncdMutex.acquire()
        syncd = self.syncd()
        data = e2010.GetDataADC(self.adcPar.t4, self.plDescr, # считываем буффер с Lcard
                            self.data_ptr, self.buffer_size)
        self.SyncdMutex.release()
        self.BufferMutex.release()
        return data, syncd

    def syncd(self):
        if not(self.IsConnected) or not(self.IsActiveMeasurements):
            return
        self.SyncdMutex.acquire()
        syncd = self.syncd()
        self.SyncdMutex.release()
        return syncd

    def addListener(self):
        self.ListenersAmount += 1
        if not(self.IsActiveMeasurements):
            self.startMeasurements()

    def removeListener(self):
        self.ListenersAmount -= 1
        if (self.ListenersAmount <= 0) and self.IsActiveMeasurements:
            self.finishMeasurements()
            self.ListenersAmount = 0

    def __del__(self):
        self.disconnectFromPhysicalDevice()
        return

    def getParameters(self):
        d = {"Connected" : self.IsConnected}
        if not(self.IsConnected):
            return d
        d["IsActiveMeasurements"] = self.IsActiveMeasurements
        if not(self.adcPar):
            return d
        d["s_Type"] = self.adcPar.t4.s_Type
        d["FIFO"] = self.adcPar.t4.FIFO
        d["IrqStep"] = self.adcPar.t4.IrqStep
        d["Pages"] = self.adcPar.t4.Pages
        d["AutoInit"] = self.adcPar.t4.AutoInit
        d["dRate"] = self.adcPar.t4.dRate
        d["dKadr"] = self.adcPar.t4.dKadr
        d["SynchroType"] = self.adcPar.t4.SynchroType
        d["SynchroSrc"] = self.adcPar.t4.SynchroSrc
        d["AdcIMask"] = self.adcPar.t4.AdcIMask
        d["NCh"] = self.adcPar.t4.NCh
        #d["Chn"] = np.array(self.adcPar.t4.Chn)
        d["IrqEna"] = self.adcPar.t4.IrqEna
        d["AdcEna"] = self.adcPar.t4.AdcEna
        return d


def test():
    print("LcardE2010B EmptyDevice test")
    myLcard = LcardE2010B_EmptyDevice("LcardE2010B.ini")
    myLcard.connectToPhysicalDevice(slot=0)
    myLcard.loadConfiguration()
        
    myLcard.startMeasurements()
    data, syncd = myLcard.readBuffer()
    time.sleep(3)
    data, syncd = myLcard.readBuffer()
    myLcard.finishMeasurements()
    myLcard.disconnectFromPhysicalDevice()
    if data is None:
        return
    print(">> data.shape, syncd:",data.shape, syncd)
    return

if __name__ == "__main__":
    try:
        test()
    except Exception as e:
        print(">>",e)


