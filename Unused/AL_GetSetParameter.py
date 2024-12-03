from PyQt5.QtCore import pyqtSignal, QObject
import properties

class GetSetParameter(QObject):
    changed = pyqtSignal()
    def __init__(self, get_value, set_value, args = None):
        super().__init__()
        self.myGet = get_value
        self.mySet = set_value
        self.myArgs = args

    def __get__(self):
        return self.myGet(self.myArgs)

    def __set__(self, value):
        return self.mySet(self.myArgs)



class GetSetPropertyParameter(GetSetParameter):
    def __init__(self, class_inst, par_name,
                 get_value = None,
                 set_value = None):
        if not(get_value):
            get_value = self.Get
        if not(set_value):
            set_value = self.Set
        super().__init__(get_value, set_value)
        self.Instance = class_inst
        self.ParName = par_name
        setattr(self.Instance, self.ParName, PS_STATES.QT_SIGNAL)
        self.changed = getattr(self.Instance, self.ParName)
        setattr(self.Instance, self.ParName, PS_STATES.DATA)

    def Get(self, args):
        return getattr(self.Instance, self.ParName)

    def Set(self, value, args):
        return setattr(self.Instance, self.ParName, value)


class GetSetDataParameter(GetSetParameter):
    def __init__(self):
        super().__init__(get_value = self.Get,
                         set_value = self.Set)
        self.Instance = class_inst
        self.ParName = par_name
        
    def Get(self, args):
        return getattr(self.Instance, self.ParName)

    def Set(self, value, args):
        k = setattr(self.Instance, self.ParName, value)
        changed.emit()
        return k


if __name__ == "__main__":
    import sys
    from PyQt5.QtWidgets import QApplication
    import numpy as np
    import pandas as pd

    
    def hello2():
      print("changed2")
    
    app = QApplication(sys.argv)
    d2 = pd.DataFrame(np.zeros((1,4)), columns = ['time', 'U', 'I', 'Iout'])
    d = GetSetDataParameter(d2)
    d.changed.connect(hello2)
    d.Data = pd.concat([d.Data,
                         pd.DataFrame(np.ones((2,4)),columns = d.Data.columns)],
                        axis = 0, ignore_index = True)
