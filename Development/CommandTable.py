import configparser
import pandas as pd
#from PyQt5.QtCore import QTimer
import time
import threading

class ABSTRACT_DEFAULT_COMMAND_TO_FUNCTOR:
    def __getitem__(self, key):
        return self.pass_function

    def pass_function(self):
        return

DEFAULT_COMMAND_TO_FUNCTOR = ABSTRACT_DEFAULT_COMMAND_TO_FUNCTOR()



class CommandTable(object):
    def __init__(self, config_file, onFinish = (lambda: print("Table Execution finished")),
                 dCommand_to_Functor: {} = DEFAULT_COMMAND_TO_FUNCTOR):
        self.dCommand_to_Functor = dCommand_to_Functor
        self.Commands = None
        self.myThread = None
        self.IsActiveExecution = False
        self.__loadConfiguration(config_file)
        self.onFinish = onFinish
        #self.dCommand_to_Functor["TimeSleep"] = self.exec_time_sleep
        
    def __loadConfiguration(self, config_file):
        config = configparser.ConfigParser()
        config.read(config_file)
        if config["Validation"]["Type"] != "CommandTable":
            raise NameError("invalid config validation for CommandTable")
        self.Commands = pd.DataFrame(columns = ["Command_Name", "Command_Args"])
        for item in config.items("Commands"):
            tmp = item[1].split(" ", maxsplit = 1)
            index = int(item[0])
            func_name = tmp[0]
            func_args_str = tmp[1]
            self.Commands.loc[index] = [func_name, func_args_str]

    def exec_time_sleep(self, time_amount):
        flag = True
        try:
            float(time_amount)
        except Exception as e:
            print(e)
            flag = False
        if flag:
            time.sleep(float(time_amount))

    def startTableExecution(self):
        self.myThread = threading.Thread(target = self.executeTable)
        self.IsActiveExecution = True
        self.myThread.start()
            
    def executeTable(self):
        for i in range(len(self.Commands)):
            self.CurrentCommandIndex = i
            key = self.Commands["Command_Name"][i]
            if key == "TimeSleep":
                self.exec_time_sleep(self.Commands["Command_Args"][i])
            else:
                self.dCommand_to_Functor[key](self.Commands["Command_Args"][i])
            if not(self.IsActiveExecution):
                break
        self.IsActiveExecution = False
        self.CurrentCommandIndex = -1
        self.onFinish()
        return

    def interruptTableExecution(self):
        if not(self.IsActiveExecution):
            return
        self.IsActiveExecution = False
        self.myThread.join()
        self.myThread = None
        print("CommandTable interrupted")

    def waitExecutionFinish(self):
        if not(self.IsActiveExecution):
            return
        self.myThread.join()
        self.myThread = None        


if __name__ == "__main__":

    def CallFunction(x):
        print("CallFunction called")

    d = {"CallFunction" : CallFunction}

    c = CommandTable("CommandTable_example.ini", dCommand_to_Functor = d)
    c.startTableExecution()
    c.waitExecutionFinish()
"""
class E:
    def __init__(self, x):
        self.x = x
    def f(self):
        print(self.x)

e = E(3)
d = {"f":e.f}

d["f"]()
"""
