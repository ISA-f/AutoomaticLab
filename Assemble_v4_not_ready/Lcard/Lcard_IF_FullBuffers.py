import numpy as np
import pandas as pd
import time

from PyQt5 import QtCore, QtGui, QtWidgets

import LcardDataInterface as LDIF
from Lcard_syncdController import LcardSyncdController



"""
Lcard_Interface_FullBuffers continuously saves all buffers
received from LcardDevice.

Buffers are stored in Lcard_Interface_FullBuffers.myData.

This interface uses LcardSyncdController to understand, when
to read data. Data is read when each half of the buffer is full,
then data is cropped to the needed part of half buffer and
stored into Lcard_Interface_FullBuffers.myData.
"""

class Lcard_Interface_FullBuffers(object):
        def __init__(self, LcardDevice):
                self.myLcardDataInterface = LDIF.LcardDataInterface(LcardDevice)
                self.myData = np.array()
                self.myLcardController = LcardSyncdController(LcardDevice)
                self.last_syncd = 0
                self.buffer_size = 128000 # value should be set in startFullBuffersRead

        def startFullBuffersRead(self, ThreadSleepTime):
                self.last_syncd = self.myLcardController.myLcardDevice.syncd()
                self.half_buffer = self.myLcardController.myLcardDevice.buffer_size // 2
                self.myLcardController.startController(EventListener = self.onControllerCall,
                                                       ThreadSleepTime = ThreadSleepTime)
                return

        def onControllerCall(self, syncd):
                if (self.last_syncd < self.half_buffer) and (syncd < self.half_buffer):
                        return # сейчас заполняется первый полубуффер
                if (self.last_syncd > self.half_buffer) and (syncd > self.half_buffer):
                        return # сейчас заполняется второй полубуффер
                self.myLcardDataInterface.readBuffer()
                LDIF.cropBuffer(lcard_IF = self.myLcardDataInterface,
                                start = self.last_syncd,
                                end = self.myLcardDataInterface.syncd)
                if self.myData == None:
                         self.myData = self.myLcardDataInterface.data
                else:
                        self.myData = np.concatenate([self.myData,
                                                      self.myLcardDataInterface.data],
                                                     axis = 1)
                self.last_syncd = self.myLcardDataInterface.syncd
                return

        def finishFullBuffersRead(self):
                self.myLcardController.finishController()
                return
