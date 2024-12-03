from enum import Enum
from PyQt5.QtWidgets import QWidget
from PyQt5.QtCore import pyqtSignal, QObject

PS_STATES = Enum('PS_STATES', ['DATA', 'QT_SIGNAL'])

class QSignalObject(QObject):
    changed = pyqtSignal()

class MyProperty:
    def __init__(self, value):
        self.Value = value
        self.myQSignalObject = QSignalObject()
        self.State = PS_STATES.DATA

    @property
    def changed(self):
        return self.myQSignalObject.changed
    
    def __get__(self, instance, owner):
        if self.State == PS_STATES.DATA:
            return self.Value
        elif self.State == PS_STATES.QT_SIGNAL:
            return self.changed
        else:
            raise NameError("MyProperty.get: invalid PropertyState")

    def __set__(self, instance, value):
        if (value is PS_STATES.QT_SIGNAL) or (value is PS_STATES.DATA):
            self.State = value
        elif self.State == PS_STATES.DATA:
            self.Value = value
            self.changed.emit()
        else:
            raise NameError("MyProperty.set: invalid PropertyState")

def connectSlotToProperty(slot, class_instance, property_name):
  setattr(class_instance, property_name, PS_STATES.QT_SIGNAL) # setting PS_STATES does not affect Property.Value
  getattr(class_instance, property_name).connect(slot)        # just changes its behaviour
  setattr(class_instance, property_name, PS_STATES.DATA)      # when "property = PS_STATES.DATA" is written, 

if __name__ == "__main__":
    import pandas as pd
    import numpy as np
    class Example:
      x = MyProperty(0)
      def __init__(self):
          self.x = pd.DataFrame(np.ones((1,7)))
    
      def do_something(self):
          self.x = 13

      def f_example():
          print("signal function call")
      
    e = Example()
    print("set without connected calls")
    e.x = 3 # можно pd.DataFrame сюда присвоить
    print("set: ", e.x)
    d = Delegate(e.f_example, None)
    connectSlotToProperty(d, e, "x") 
  
    print("set with connected calls")
    e.x = -3
    print("set", e.x)
    e.do_something()



"""
                 #fget = (lambda val: val),
                 #fset = (lambda val, args: val),
                 #fset_args = None):
        #self.fget = fget
        #self.fset = fset
        #self.fset_args = fset_args

class PropertyState:
    pass

class PS_QtSignal(PropertyState):
    pass

class PS_Data(PropertyState):
    pass

def getPropertyCopy(inst, name):
    k = inst.__slots__[name]
    print("k: ", type(k))
    return k
"""
