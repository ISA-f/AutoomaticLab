import LcardDataInterface as LDIF
import Device_Korad as DKorad
import Lcard_EmptyDevice
import numpy as np
import pandas as pd
import serial
from PyQt5 import QtCore, QtWidgets
import sys
import matplotlib
import matplotlib.pyplot as plt
from Updatable_QTCanvas import PyplotWidget, GraphWidget

data = np.zeros(0)
print(data.shape)
print(data[2:-3].shape)

