import numpy as np
import pandas as pd
import time

from PyQt5 import QtCore, QtGui, QtWidgets

import LcardDataInterface as LDIF
from Lcard_syncdController import LcardSyncdController



"""
Input: Lcard_Interface_ContinuousRead reads buffers
received from LcardDevice. 

Output: Lcard_Interface_ContinuousRead processes, 
stores and continuously saves to file received data

Averaged buffers are stored in Lcard_Interface_ContinuosRead.myData

This interface uses LcardSyncdController to understand, when
to read data. Data is read when each half of the buffer is full,
then data is cropped to the needed part of half buffer, processed and
stored into Lcard_Interface_ContinuousRead.myData.
"""

class Lcard_Interface_ContinuousRead(object):
        def __init__(self, LcardDevice, filename: str):
                self.myLcardDataInterface = LDIF.LcardDataInterface(LcardDevice)
                self.myData = None
                self.myLcardController = LcardSyncdController(LcardDevice)
                self.last_syncd = 0
                self.buffer_size = 128000 # this is a default value
                                          # value should be set in startFullBuffersRead
                self.mySaveFile = None

        def startContinuousRead(self, ThreadSleepTime, measurements_file):
                self.last_syncd = self.myLcardController.myLcardDevice.syncd()
                self.buffer_size = self.myLcardController.myLcardDevice.buffer_size
                self.half_buffer = self.buffer_size // 2
                self._MeasurementsFile = open(measurements_file, "ab")
                self.myLcardController.startController(EventListener = self.onControllerCall,
                                                       ThreadSleepTime = ThreadSleepTime)
                return

        def onControllerCall(self, syncd):
                if (self.last_syncd < self.half_buffer) and (syncd < self.half_buffer):
                        return # сейчас заполняется первый полубуффер
                if (self.last_syncd > self.half_buffer) and (syncd > self.half_buffer):
                        return # сейчас заполняется второй полубуффер

                # когда один из полубуфферов заполнился:
                self.myLcardDataInterface.readBuffer()
                LDIF.cropToRequestedBuffer(lcard_IF = self.myLcardDataInterface,
                                           requested_buffer_size = self.myRequestedBufferSize)
                LDIF.calculateAverage(lcard_IF = self.myLcardDataInterface)
                DataPiece = np.concatenate(self.myLcardDataInterface.read_time, 
                                           self.myLcardDataInterface.data)
                if not(self.myData):
                         self.myData = DataPiece
                else:
                        self.myData = np.concatenate([self.myData, DataPiece],
                                                     axis = 1)
                self.last_syncd = self.myLcardDataInterface.syncd
                self._saveDataPiece(DataPiece)
                return

        def finishContinuousRead(self):
                self.myLcardController.finishController()

                return

        def _saveDataPiece(self, DataPiece):
            self._MeasurementsFile.write(b"\n")
            np.savetxt(self._MeasurementsFile, DataPiece) # сохраняем данные в объекте файла
            if(self.NextSaveTime <= self.myLcardDataInterface.read_time):
                    self._MeasurementsFile.flush() # периодически отправляем новые данные в оперативной памяти, в объекте файла
                    self.NextSaveTime = self.myLcardDataInterface.read_time + self.SavePeriodTime
                    print("Lcard E2010: measurements flushed")
            return

        def clearData(self):
            self.myData = np.array()

