import numpy as np
import pandas as pd
import time

from PyQt5 import QtCore, QtGui, QtWidgets

import LcardDataInterface as LDIF
from Lcard_syncdController import LcardSyncdController

"""
Lcard_Interface_FullBuffers continuously saves all buffers
received from LcardDevice.

Buffers are stored in list Lcard_Interface_FullBuffers.myDataChunks

This interface uses LcardSyncdController to understand, when
to read data. Data is read when each half of the buffer is full,
then data is cropped to the needed half of buffer and
stored into Lcard_Interface_FullBuffers.myDataChunks.
"""

"""
Design decision: 

Initially Lcard_Interface_FullBuffers supported events upon each buffer fullfillment.
But Lcard outputs data so fast, it calls listeners each 0.05 seconds

For matplotlib.pyplot this was too fast:
it created forbidden access of Data being already updated with Lcard,
but not being drawn in the plot.

In this version Lcard_Interface_FullBuffers stores all data in self.myDataChunks,
so that already stored DataChunks are not modified, and any listener 
can watch stored DataChunks on their own.
"""

class Lcard_Interface_FullBuffers(object):
        def __init__(self, LcardDevice):
                self.myLcardDataInterface = LDIF.LcardDataInterface(LcardDevice)
                self.myDataChunks = []
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
                        self.myDataChunks.append(np.copy(self.myLcardDataInterface.data))
                        print(">>5")
                        self.last_syncd = self.myLcardDataInterface.syncd
                        print(">>6")
                except Exception as e:
                        print(e)
                print("finish onControllerCall")
                return

        def finishFullBuffersRead(self):
            self.myLcardController.finishController()
            return

        def clearData(self):
            self.myDataChunks = []

        def getIsActiveInterface(self):
            if self.myLcardController is None:
                return False
            return self.myLcardController.IsActiveController

        def getNumpyData(self):
            if self.myDataChunks == []:
                    return np.zeros((1,4))
            if self.myLcardController.IsActiveController: 
                    return np.vstack(self.myDataChunks[:-1])
            return np.vstack(self.myDataChunks)

        def getParameters(self):
            d = self.myLcardController.getParameters()
            d["LcardIFFB.myData.shape"] = None
            d["len(LcardIFFB.myDataChunks)"] = len(self.myDataChunks)
            return d


def test():
    print("Lcard_IF_FullBuffers test")
    
    import Lcard_EmptyDevice
    import matplotlib.pyplot as plt
    
    lcard = Lcard_EmptyDevice.LcardE2010B_EmptyDevice("LcardE2010B.ini")
    lcard.connectToPhysicalDevice()
    lcard.loadConfiguration()
    lcard.startMeasurements()
    LcardIFFB = Lcard_Interface_FullBuffers(lcard)
    LcardIFFB.startFullBuffersRead(ThreadSleepTime = 0.03)
    time.sleep(3)
    LcardIFFB.finishFullBuffersRead()

    lcard.finishMeasurements()
    lcard.disconnectFromPhysicalDevice()

    for idata in LcardIFFB.myDataChunks:
        print(idata.shape)
        plt.plot(np.arange(len(idata[0])), idata[0])
    plt.scatter(np.arange(len(LcardIFFB.myDataChunks[0])), LcardIFFB.myDataChunks[0]-0.12, s = 2)
    plt.show()

if __name__ == "__main__":
    try:
        test()
        print(">> success")
        print()
    except Exception as e:
        print(">>", e)
        a = input()
