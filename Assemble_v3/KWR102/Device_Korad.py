import configparser
import numpy as np
import time
from Abstract_Device import Device, DeviceParameter
import pandas as pd
import serial
from threading import Lock

class Korad(Device):

    type = "Korad"

    def __init__(self, config_filename:str):
        super().__init__(config_filename)
        self.myData = pd.DataFrame(columns = ['time', 'U', 'I'])
        self.DeviceParameters = []
        self.ser = None
        self.mutex = Lock()

    def StartExperiment(self):
        if self.ser == None:
            return
        print("Device_Korad.StartExperiment()")
        self.mutex.acquire()
        self.ser.write(f'OUT:1\r'.encode('ASCII'))
        self.mutex.release()
        return

    def FinishExperiment(self):
        if self.ser == None:
            return
        self.Set_v_i(0,0)
        self.mutex.acquire()
        self.ser.write(f'OUT:0\r'.encode('ASCII'))
        self.mutex.release()
        return
    
    def TakeMeasurements(self):
        if self.ser == None:
            print("Korad is not connected; tried Korad.TakeMeasurements()")
            return

        self.mutex.acquire()
        #self.ser.write(b'\r')
        self.ser.write(b'VOUT?\r')
        str_voltage = self.ser.readline().decode()[:-1]
        print("*"+str_voltage+"*")
        voltage = float(str_voltage)
        self.ser.write(b'IOUT?\r')
        current = float(self.ser.readline().decode()[:-1])
        self.mutex.release()
        time_sistem = time.time()
        DataPiece = np.array([time_sistem, voltage, current])
        return DataPiece

    def Set_v_i(self,v=None,i=None):
        if self.ser == None:
            print("Korad is not connected; tried Korad.Set_v_i()")
            return
        else:
            self.mutex.acquire()
            if v!=None:
                self.ser.write(f'VSET:{v}\r'.encode('ASCII'))
            if i!=None:
                self.ser.write(f'ISET:{i}\r'.encode('ASCII'))
            self.mutex.release()


    def ConnectToPhysicalDevice(self):
        config_dict = self.LoadConfiguration()
        self.ser = serial.Serial(config_dict['com port'],
                            config_dict['bits per second'],
                            timeout=1,
                            parity=config_dict['parity'],
                            stopbits=config_dict['stop bits'],
                            xonxoff=config_dict['xonxoff'],
                            rtscts=config_dict['rtscts'],
                            bytesize=config_dict['data bits'])

    def DisconnectFromPhysicalDevice(self):
        self.FinishExperiment()
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

if __name__ == "__main__":
    myKorad = Korad('Korad.ini')
    myKorad.ConnectToPhysicalDevice()
    myKorad.StartExperiment()
    print(myKorad.TakeMeasurements())
    myKorad.Set_v_i(1.5, None)
    #print(time.time())
    #for i in range(1000):
    #    v = myKorad.TakeMeasurements()
    #print(time.time())
        


