import configparser
import numpy as np
import time
from Abstract_Device import Device, DeviceParameter
import pandas as pd
import serial
from threading import Lock

from enum import Enum
class KORAD_NAMES(Enum):
    KORAD_TIME = "Korad_time"
    VOLTAGE = "Korad_U"
    CURRENT = "Korad_I"

class Korad(Device):

    type = "Korad"

    def __init__(self, config_filename:str):
        super().__init__(config_filename)
        self.ser = None
        self.mutex = Lock()
        self.myData = pd.DataFrame(columns = ['time', 'U', 'I']) # unused
        # unused but left for compatability
        self._IsActiveMeasurements = False

    @property
    def IsConnected(self):
        return not(self.ser is None)
    
    @property
    def IsActiveMeasurements(self):
        if not(self.IsConnected):
            self._IsActiveMeasurements = False
        return self._IsActiveMeasurements

    def StartExperiment(self):
        if not(self.IsConnected):
            return
        print("Device_Korad.StartExperiment()")
        self.mutex.acquire()
        self.ser.write(f'OUT:1\r'.encode('ASCII'))
        self._IsActiveMeasurements = True
        self.mutex.release()
        return

    def FinishExperiment(self):
        if not(self.IsConnected):
            return
        self.Set_v_i(0,0)
        self.mutex.acquire()
        self.ser.write(f'OUT:0\r'.encode('ASCII'))
        self._IsActiveMeasurements = False
        self.mutex.release()
        return
    
    def TakeMeasurements(self):
        if not(self.IsConnected):
            time_sistem = time.time()
            print("Korad is not connected; tried Korad.TakeMeasurements()")
            return pd.Series([time_sistem, None, None],index = KORAD_NAMES._member_map_.values())

        if not(self.IsActiveMeasurements):
            time_sistem = time.time()
            print("Korad is connected, but not active; tried Korad.TakeMeasurements()")
            return pd.Series([time_sistem, None, None],index = KORAD_NAMES._member_map_.values())

        self.mutex.acquire()
        #self.ser.write(b'\r')
        self.ser.write(b'VOUT?\r')
        voltage = float(self.ser.readline().decode()[:-1])
        self.ser.write(b'IOUT?\r')
        current = float(self.ser.readline().decode()[:-1])
        self.mutex.release()
        time_sistem = time.time()
        return pd.Series([time_sistem, voltage, current],index = KORAD_NAMES._member_map_.values())

    def Set_v_i(self,v=None,i=None):
        if not(self.IsConnected):
            print("Korad is not connected; tried Korad.Set_v_i()")
            return
        elif not(self.IsAciveMeasurements):
            print("Korad is connected, but not active; tried Korad.Set_v_i()")
        else:
            self.mutex.acquire()
            if v!=None:
                self.ser.write(f'VSET:{v}\r'.encode('ASCII'))
            if i!=None:
                self.ser.write(f'ISET:{i}\r'.encode('ASCII'))
            self.mutex.release()

    def ConnectToPhysicalDevice(self):
        try:
            config_dict = self.LoadConfiguration()
            self.ser = serial.Serial(config_dict['com port'],
                            config_dict['bits per second'],
                            timeout=1,
                            parity=config_dict['parity'],
                            stopbits=config_dict['stop bits'],
                            xonxoff=config_dict['xonxoff'],
                            rtscts=config_dict['rtscts'],
                            bytesize=config_dict['data bits'])
        except Exception as e:
            print("Try connect Korad:", e)
            self.ser = None
            return False
        return True 

    def DisconnectFromPhysicalDevice(self):
        self.FinishExperiment()
        if self.ser:
            self.ser.close()
            self.ser = None

    def LoadConfiguration(self):
        config = configparser.ConfigParser()
        config.read(self.ConfigFilename)
        bits_per_second = config['COM settings']['bits per second']
        data_bits = config['COM settings']['data bits']
        parity_name = config['COM settings']['parity']
        stop_bits = config['COM settings']['stop bits']
        flow_control_bits = config['COM settings']['flow control']
        flow_control_bits_xon_xoff = (flow_control_bits == 'Xon / Xoff')
        flow_control_bits_hardware = (flow_control_bits == 'Hardware')
        com_port = config['COM settings']['com port']
        dict_parity = {'None': serial.PARITY_NONE,
                       'Even': serial.PARITY_EVEN,
                       'Odd': serial.PARITY_ODD,
                       'Mark': serial.PARITY_MARK,
                       'Space': serial.PARITY_SPACE}
        parity = dict_parity[parity_name]
        config_dict = {'com port':com_port,
                       'bits per second':int(bits_per_second),
                       'data bits':int(data_bits),
                        'parity':parity,
                       'stop bits':float(stop_bits),
                       'xonxoff':flow_control_bits_xon_xoff,
                       'rtscts':flow_control_bits_hardware}
        return config_dict

    def set_uncheckedI(self, value):
        s = None
        try:
            s = float(value)
        except Exception as e:
            pass
        if(s):
            self.Set_v_i(i = s)
        return

    def set_uncheckedU(self, value):
        s = None
        try:
            s = float(value)
        except Exception as e:
            pass
        if(s):
            self.Set_v_i(v = s)
        return

    def __del__(self):
        self.DisconnectFromPhysicalDevice()
        return

    def getParameters(self):
        d = {"Korad.serial_port" : None}
        if not(self.ser):
            return d
        d["Korad.serial_port"] = self.ser.port
        d["Korad.baudrate"] = self.ser.baudrate
        d["Korad.bytesize"] = self.ser.bytesize
        d["Korad.parity"] = self.ser.parity
        d["Korad.stopbits"] = self.ser.stopbits
        d["Korad.timeout"] = self.ser.timeout
        d["Korad.xonxoff"] = self.ser.xonxoff
        d["Korad.rtscts"] = self.ser.rtscts
        d["Korad.dsrdtr"] = self.ser.dsrdtr
        return d

def test():
    print("Test: Device Korad")
    myKorad = Korad('Korad.ini')
    print(myKorad.getParameters())
    myKorad.ConnectToPhysicalDevice()
    myKorad.StartExperiment()
    print(">>", myKorad.TakeMeasurements())
    myKorad.Set_v_i(1, None)
    myKorad.FinishExperiment()
    myKorad.DisconnectFromPhysicalDevice()
    #print(time.time())
    #for i in range(1000):
    #    v = myKorad.TakeMeasurements()
    #print(time.time())
    return

if __name__ == "__main__":
    try:
        test()
        print(">> success")
    except Exception as e:
        print(">>",e)
        


