import numpy as np
import pandas as pd
import time

from PyQt5 import QtCore, QtGui, QtWidgets

import LcardDataInterface as LDIF
from Lcard_syncdController import LcardSyncdController



"""
Lcard_Interface_FullBuffers continuously saves all buffers
received from LcardDevice.

Buffers are stored in Lcard_Interface_FullBuffers.myData

This interface uses LcardSyncdController to understand, when
to read data. Data is read when each half of the buffer is full,
then data is cropped to the needed part of half buffer and
stored into Lcard_Interface_FullBuffers.myData.
"""

# TO DO : update to LDIF.LCARD_NAMES.CH0RAW and DataFrames

class Lcard_Interface_FullBuffers(object):
        def __init__(self, LcardDevice, onDataUpdate):
                self.onDataUpdate = onDataUpdate
                self.myLcardDataInterface = LDIF.LcardDataInterface(LcardDevice)
                self.myData = None
                self.myLcardController = LcardSyncdController(LcardDevice)
                self.last_syncd = 0
                self.buffer_size = 128000 # value should be set in startFullBuffersRead

        def startFullBuffersRead(self, ThreadSleepTime):
                self.myLcardDataInterface.readBuffer()
                self.last_syncd = self.myLcardDataInterface.syncd
                if self.myLcardController.myLcardDevice is None:
                        return
                if self.myLcardController.myLcardDevice.buffer_size is None:
                        return
                self.buffer_size = self.myLcardController.myLcardDevice.buffer_size
                self.half_buffer = self.buffer_size // 2
                self.myLcardController.startController(EventListener = self.onControllerCall,
                                                       ThreadSleepTime = ThreadSleepTime)
                return

        def onControllerCall(self, syncd):
                print("start onController call")
                try:
                        if (self.last_syncd < self.half_buffer) and (syncd < self.half_buffer):
                                return # сейчас заполняется первый полубуффер
                        print(">>1")
                        if (self.last_syncd > self.half_buffer) and (syncd > self.half_buffer):
                                return # сейчас заполняется второй полубуффер
                        print(">>2")
                        self.myLcardDataInterface.readBuffer()
                        print(">>3")
                        LDIF.cropBuffer(lcard_IF = self.myLcardDataInterface,
                                        start = self.last_syncd,
                                        end = self.myLcardDataInterface.syncd)
                        print(">>4")
                        if self.myData is None:
                                print(">>4.1")
                                self.myData = self.myLcardDataInterface.data
                        else:
                                print(">>4.2")
                                self.myData = np.concatenate([self.myData,
                                                              self.myLcardDataInterface.data],
                                                             axis = 1)
                        print(">>5")
                        self.last_syncd = self.myLcardDataInterface.syncd
                        print(">>6")
                        self.onDataUpdate(self.myData)
                        print(">>7")
                except Exception as e:
                        print(e)
                print("finish onControllerCall")
                return

        def finishFullBuffersRead(self):
                self.myLcardController.finishController()
                return

        def clearData(self):
            self.myData = None

        def getIsActiveInterface(self):
            if self.myLcardController is None:
                return False
            return self.myLcardController.IsActiveController

        def getParameters(self):
            d = self.myLcardController.getParameters()
            d["LcardIFFB.myData.shape"] = None
            if not(self.myData is None):
                    d["LcardIFFB.myData.shape"] = self.myData.shape
            return d


def test():
    print("Lcard_IF_FullBuffers test")
    datas = []
    def example(data):
        datas.append(data)
    
    import Lcard_EmptyDevice
    import matplotlib.pyplot as plt
    
    lcard = Lcard_EmptyDevice.LcardE2010B_EmptyDevice("LcardE2010B.ini")
    lcard.connectToPhysicalDevice()
    lcard.loadConfiguration()
    lcard.startMeasurements()
    LcardIFFB = Lcard_Interface_FullBuffers(lcard, example)
    LcardIFFB.startFullBuffersRead(ThreadSleepTime = 0.03)
    time.sleep(3)
    LcardIFFB.finishFullBuffersRead()

    lcard.finishMeasurements()
    lcard.disconnectFromPhysicalDevice()
    for idata in datas:
        print(idata.shape)
        plt.plot(np.arange(len(idata[0])), idata[0])
    plt.scatter(np.arange(len(LcardIFFB.myData[0])), LcardIFFB.myData[0]-0.12, s = 2)
    plt.show()

if __name__ == "__main__":
    try:
        test()
        print(">> success")
        print()
    except Exception as e:
        print(">>", e)
        a = input()
