import LcardDataInterface as LDIF
import Device_Korad as DKorad
import pandas as pd
import numpy as np

myLcardIF = LDIF.LcardDataInterface("debug str")

x = np.zeros((1,10000))
x[0] += 1
#x[1] += 10
#x[2] += 100
#x[3] += 1000
x[0] += (np.random.random(size = 10000)-0.5)*2
#x[1] += 4*(np.random.random(size = 10000)-0.5)*2
#x[2] += 30*(np.random.random(size = 10000)-0.5)*2
#x[3] += 200*(np.random.random(size = 10000)-0.5)*2

myLcardIF.data = x
myLcardIF.syncd = 2000

LDIF.cropToRequestedBuffer(myLcardIF, 
                            requested_buffer_size = 8000)
LDIF.calculateAverage(myLcardIF)
lcard_data = myLcardIF.data
                # data processing
myDataPiece = lcard_data.to_frame().T
                # data processing : update synth channel
                # Тут формулки, их желательно проверить на корректность
myDataPiece[["Ua", "Ia", "Imin", "sigmaI"]] = None
k1, c1, c2 = 1,1,1
if LDIF.LCARD_NAMES.CH0MEAN in lcard_data.index:
    myDataPiece["Ua"] = k1*lcard_data[LDIF.LCARD_NAMES.CH0MEAN]       # Ua = k1 <ch1>
if {LDIF.LCARD_NAMES.CH0MEAN, LDIF.LCARD_NAMES.CH1MEAN}.issubset(lcard_data.index):   # Ia = c1 <ch1> - c2 <ch2>
    myDataPiece["Ia"] = c1*lcard_data[LDIF.LCARD_NAMES.CH0MEAN] - c2*lcard_data[LDIF.LCARD_NAMES.CH1MEAN]
if {LDIF.LCARD_NAMES.CH0MIN, LDIF.LCARD_NAMES.CH1MAX}.issubset(lcard_data.index):     # Imin = c1 ch1_min - c2 ch2_max
    myDataPiece["Imin"] = c1*lcard_data[LDIF.LCARD_NAMES.CH0MIN] - c2*lcard_data[LDIF.LCARD_NAMES.CH1MAX]
if {LDIF.LCARD_NAMES.CH0STD, LDIF.LCARD_NAMES.CH1STD}.issubset(lcard_data.index):     # sigma = c1 sigma_1 - c2 sigma_2
    myDataPiece["sigmaI"] = np.sqrt((c1*lcard_data[LDIF.LCARD_NAMES.CH0STD])**2 + (c2*lcard_data[LDIF.LCARD_NAMES.CH1STD])**2)

print(myLcardIF.data)
print()
print(myDataPiece)
