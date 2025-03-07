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
    CH0RAW = "Lcard_Ch0_Raw"
    CH1RAW = "Lcard_Ch1_Raw"
    CH2RAW = "Lcard_Ch2_Raw"
    CH3RAW = "Lcard_Ch3_Raw"
    INDEX = "Lcard_index"

AveragedDataColumns = np.array(
    [[LCARD_NAMES.CH0MEAN,LCARD_NAMES.CH1MEAN,LCARD_NAMES.CH2MEAN,LCARD_NAMES.CH3MEAN],
     [LCARD_NAMES.CH0STD, LCARD_NAMES.CH1STD, LCARD_NAMES.CH2STD, LCARD_NAMES.CH3STD],
     [LCARD_NAMES.CH0MIN, LCARD_NAMES.CH1MIN, LCARD_NAMES.CH2MIN, LCARD_NAMES.CH3MIN],
     [LCARD_NAMES.CH0MAX, LCARD_NAMES.CH1MAX, LCARD_NAMES.CH2MAX, LCARD_NAMES.CH3MAX]]
    )

RawChannelNames = np.array([LCARD_NAMES.CH0RAW, LCARD_NAMES.CH1RAW, LCARD_NAMES.CH2RAW, LCARD_NAMES.CH3RAW])

class LcardDataInterface:
    def __init__(self, LcardDevice):
        self.myLcardDevice = LcardDevice
        self.data = pd.DataFrame(columns = RawChannelNames)
        self.syncd = 0
        self.read_time = -1
        
    def readBuffer(self):
        if not(self.myLcardDevice):
            return
        _data, _syncd = self.myLcardDevice.readBuffer()
        self.read_time = time.time()
        if _data is None:
            self.data = pd.DataFrame(columns = [LCARD_NAMES.INDEX, LCARD_NAMES.CH0RAW])
            self.syncd = 0
            return

        N_channels, N_measurements = _data.shape[0], _data.shape[1]
        self.data = pd.DataFrame(_data, columns = RawChannelNames[:N_channels])
        self.data[RawChannelNames[N_channels:]] = None
        self.data[LCARD_NAMES.INDEX] = np.arange(N_measurements)
        return
        

def calculateAverage(lcard_IF):
    if lcard_IF is None:
        return
    if (lcard_IF.data is None) or not(LCARD_NAMES.INDEX in lcard_IF.data.columns):
        lcard_IF.data = pd.Series([lcard_IF.read_time, None], index = [LCARD_NAMES.COMP_TIME, LCARD_NAMES.CH0MEAN])
        return
    DataPiece = pd.DataFrame()
    DataPiece[AveragedDataColumns]
    _data = (lcard_IF.data.drop(LCARD_NAMES.INDEX, axis = 1)).to_numpy()
    N_channels = _data.shape[0]
    print(N_channels)
    columns = np.ravel(AveragedDataColumns[:, :N_channels])
    DataPiece = np.ravel(
            [np.mean(lcard_IF.data, axis = 1),
             np.std(lcard_IF.data, axis = 1),
             np.min(lcard_IF.data, axis = 1),
             np.max(lcard_IF.data, axis = 1)])
    print(DataPiece, columns)
    lcard_IF.data = pd.Series(DataPiece,index = columns)
    return

"""
def calculateAverage_old(lcard_IF):
    if lcard_IF.data is None:
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
"""

def cropBuffer(lcard_IF, start, end):
    if lcard_IF.data is None:
        return
    if start > end:
        lcard_IF.data = np.concatenate([lcard_IF.data[:,start : lcard_IF.data.shape[1]],
                                        lcard_IF.data[:, 0 : end]],
                                       axis = 1)
    else:
        lcard_IF.data = lcard_IF.data[:, start : end]
    return

def cropToRequestedBuffer(lcard_IF, requested_buffer_size):
    if lcard_IF.data is None:
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


def test():
    print("LcardDataInterface test")
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


if __name__ == "__main__":
    try:
        test()
        print(">> success")
    except Exception as e:
        print(">>", e)
    
    
    
