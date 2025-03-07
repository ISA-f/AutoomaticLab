import serial
import time
ser = serial.Serial("COM5",9600,timeout=1,stopbits=1)
ser.write(f'VSET:1.0\r'.encode('ASCII'))
ser.write(f'OUT:1'.encode('ASCII'))
ser.write(b'VOUT?\r')
print(ser.readline().decode())
ser.write(b'IOUT?\r')
print(ser.readline().decode())
"""
        voltage = float(self.ser.readline().decode()[:-1])
        self.ser.write(b'IOUT?\r')
        current = float(self.ser.readline().decode()[:-1])
        time_sistem = time.time()
        DataPiece = np.array([time_sistem, voltage, current])
        return DataPiece

    def Set_v_i(self,v=None,i=None):
        if self.ser == None:
            raise Exception("Korad is not connect")
        else:
            if v!=None:
                self.ser.write(f'VSET:{v}\r'.encode('ASCII'))
            if i!=None:
                self.ser.write(f'ISET:{i}\r'.encode('ASCII'))


    def ConnectToPhysicalDevice(self):
        config_dict = self.LoadConfiguration()
"""
