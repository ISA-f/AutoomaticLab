from Lcard_EmptyDevice import LcardE2010B_EmptyDevice
import numpy as np
import pandas as pd
import time

from enum import Enum
class LCARD_NAMES(Enum):
    COMP_TIME = "Lcard_time"
    CH0MEAN = "Lcard_Ch0_Mean"
    CH1MEAN = "Lcard_Ch1_Mean"
    CH2MEAN = "Lcard_Ch2_Mean"
    CH3MEAN = "Lcard_Ch3_Mean"
    CH0STD = "Lcard_Ch0_Std"
    CH1STD = "Lcard_Ch1_Std"
    CH2STD = "Lcard_Ch2_Std"
    CH3STD = "Lcard_Ch3_Std"
    CH0MIN = "Lcard_Ch0_Min"
    CH1MIN = "Lcard_Ch1_Min"
    CH2MIN = "Lcard_Ch2_Min"
    CH3MIN = "Lcard_Ch3_Min"
    CH0MAX = "Lcard_Ch0_Max"
    CH1MAX = "Lcard_Ch1_Max"
    CH2MAX = "Lcard_Ch2_Max"
    CH3MAX = "Lcard_Ch3_Max"

class LcardDataInterface:
    def __init__(self, LcardDevice):
        self.myLcardDevice = LcardDevice
        self.data = None
        self.syncd = None
        self.read_time = -1
        
    def readBuffer(self):
        if not(self.myLcardDevice):
            return
        self.data, self.syncd = self.myLcardDevice.readBuffer()
        #print("data, syncd: ", self.data, self.syncd)
        self.read_time = time.time()

    def free(self):
        self.data = None
        self.syncd = None
        
        
def calculateAverage(lcard_IF):
    if not(lcard_IF.data):
        time_sistem = time.time()
        lcard_IF.data = pd.Series([None]*17, index = LCARD_NAMES._member_map_.values())
        return 
    N_channels = lcard_IF.data.shape[0]
    columns = [[f"MeanCh{i}" for i in range(N_channels)], [f"StdCh{i}" for i in range(N_channels)],
              [f"MinCh{i}" for i in range(N_channels)], [f"MaxCh{i}" for i in range(N_channels)]]
    DataPiece = np.ravel(
            [np.mean(lcard_IF.data, axis = 1),
             np.std(lcard_IF.data, axis = 1),
             np.min(lcard_IF.data, axis = 1),
             np.max(lcard_IF.data, axis = 1)])
    columns = np.ravel(columns)
    lcard_IF.data = pd.Series(DataPiece,index = columns)
    return

def cropBuffer(lcard_IF, start, end):
    if not(lcard_IF.data):
        return
    if start > end:
        lcard_IF.data = np.concatenate([lcard_IF.data[:,start : lcard_IF.data.shape[1]],
                                        lcard_IF.data[:, 0 : end]],
                                       axis = 1)
    else:
        lcard_IF.data = lcard_IF.data[:, start : end]
    return

def cropToRequestedBuffer(lcard_IF, requested_buffer_size):
    if not(lcard_IF.data):
        return
    end = lcard_IF.syncd
    #N_channels = self.myLcardDevice.adcPar.t4.NCh
    #print(N_channels)
    start = end - requested_buffer_size
    if start < 0:
        lcard_IF.data = np.concatenate([lcard_IF.data[:,(lcard_IF.data.shape[1] + start) : lcard_IF.data.shape[1]],
                                        lcard_IF.data[:, 0 : end]],
                                       axis = 1)
    else:
        lcard_IF.data = lcard_IF.data[:, start : end]
    return

def addSynthChannels(lcard_IF, synth_channels_function):
    if not(lcard_IF.data):
        return
    synth_data = synth_channels_function(lcard_IF.data)
    lcard_IF.data = np.concatenate([lcard_IF.data, synth_data])
    return


if __name__ == "__main__":
    import time
    lcard = LcardE2010B_EmptyDevice("LcardE2010B.ini")
    lcard_IF = LcardDataInterface(lcard)
    lcard_IF2 = LcardDataInterface(lcard)
    
    lcard.connectToPhysicalDevice()
    lcard.loadConfiguration()
    lcard.startMeasurements()
    time.sleep(1)

    lcard_IF.readBuffer()
    lcard_IF2.readBuffer()
    
    lcard.finishMeasurements()
    lcard.disconnectFromPhysicalDevice()

    calculateAverage(lcard_IF)
    cropToRequestedBuffer(lcard_IF2, 8000)
    
    
    
