import Abstract_Device_Interface
from Abstract_Device_Interface import DevicePropertyParameter
import Device_LcardE440
import properties
import pandas as pd

class plDescrParameterE440(DevicePropertyParameter):
    def __init__(self, device, par_name: str, data_type = str,
                 is_gettable: callable = (lambda dev: bool(dev.plDescr)),
                 get_value: callable = (lambda dev, name: getattr(dev.plDescr.t4, name))):
        super().__init__(
            device, par_name, data_type,
            is_gettable, get_value
            )
        return

class slotParameter(DevicePropertyParameter):
    def __init__(self, device, par_name: str, data_type = str,
                 is_gettable = (lambda dev: bool(dev.slPar)),
                 get_value = (lambda dev, name: getattr(dev.slPar, name))):
        super().__init__(
            device, par_name, data_type
            is_gettable, get_value
            )
        return

class adcParameterE440(DevicePropertyParameter):
    def __init__(self, device, par_name, data_type, 
                 is_gettable: callable = (lambda dev: bool(dev.adcPar)),
                 get_value: callable = (lambda dev, name: getattr(dev.adcPar.t3, name)),
                 is_settable: callable = (lambda dev, val: not(dev.MeasurementIsActive)),
                 set_value : callable = (lambda dev, name, val: setattr(dev.adcPar.t3, name)):
        super().__init__(
            device, par_name, data_type
            is_gettable, get_value,
            is_settable, set_value
            )
        return

class LcardInterface(Abstract_Device_Interface.Abstract_Device_Interface):
    def __init__(self, device):
        super().__init__(device)

        slotParameterKeys = ["Base", "BaseL", "Base1", "BaseL1", "Mem", "MemL", "Mem1", "MemL1", "Irq", "BoardType", "DSPType",
                             "Dma", "DmaDac", "DTA_REG", "IDMA_REG", "CMD_REG", "IRQ_RST", "DTA_ARRAY", "RDY_REG", "CFG_REG"]
        for key in slotParameterKeys:
            self.Parameters[key] = slotParameter(device, key)


        plDescrParameterKeys = ["SerNum", "BrdName", "Rev", "DspType", "IsDacPresent", "Quartz"]
        for key in plDescrParameterKeys:
            self.Parameters[key] = plDescrParameterE440(device, key)

        self.Parameters["ThreadSleepTime"] = DevicePropertyParameter(device, "ThreadSleepTime", float, is_settable = (lambda dev, val: val>0))
        self.Parameters["SavePeriodTime"] = DevicePropertyParameter(device, "SavePeriodTime", float, is_settable = (lambda dev, val: val>0))
        self.Parameters["DotsPerHalfBuffer"] = DevicePropertyParameter(device, "DotsPerHalfBuffer", int,
                                                          is_settable = (lambda dev, val: (val>0 and val%1==0 and dev.buffer_size > val)))
        self.Parameters["MeasurementIsActive"] = DevicePropertyParameter(device, "MeasurementIsActive", bool)
        self.Parameters["buffer_size"] = DevicePropertyParameter(device, "buffer_size", int)
        self.Parameters["myData"] = DevicePropertyParameter(device, "myData", pd.DataFrame)

        self.Parameters["AutoInit"] = adcParameterE440(device, "AutoInit", bool)  
        self.Parameters["FIFO"] = adcParameterE440(device, "FIFO", int,
                                                   is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (1 <= val <= 4096))))
        self.Parameters["IrqStep"] = adcParameterE440(device, "IrqStep", int,
                                                      is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (1 <= val <= 4096))))
        self.Parameters["Pages"] = adcParameterE440(device, "Pages", int,
                                                    is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (1 <= val <= 32))))       
        self.Parameters["dRate"] = adcParameterE440(device, "dRate", float,
                                                    is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (0 < val <= 400.0))))
        self.Parameters["dKadr"] = adcParameterE440(device, "dKadr", float,
                                    is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (0.0025 <= val < dev.adcPar.t3.dRate/4))))
        self.Parameters["AdChannel"] = adcParameterE440(device, "AdChannel", int,
                                                        is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (0 <= val <= 16))))
        self.Parameters["AdPorog"] = adcParameterE440(device, "AdPorog", int,
                                                      is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (0 <= val <= 16))))
        self.Parameters["IrqEna"] = adcParameterE440(device, "IrqEna", bool)
        self.Parameters["AdcEna"] = adcParameterE440(device, "AdcEna", bool)
        self.Parameters["NCh"] = adcParameterE440(device, "NCh", int,
                                                  1is_settable = (lambda dev, val: (not(dev.MeasurementIsActive) and (1 <= val <= 16))))
    return

        
    def LoadConfiguration(self, config_filename):
        f = open(config_filename)
        config = configparser.ConfigParser()
        config.read_file(f)
        
        if (config["Validation"]["BoardType"] != "E440"):
            raise NameError("Lcard E440: invalid BoardType ini file: ", config["Validation"]["BoardType"])

        for par_name in ["ThreadSleepTime", "DotsPerHalfBuffer", "SavePeriodTime"]:
            parameters[par_name] = config["CodeControls"][par_name]

        parameters["s_Type"] = L_ADC_PARAM

        ADCpar = config["ADC_Parameters"]
        parameters = {}
        par_names = ["dRate", "dKadr", "SynchroType", "SynchroSensitivity", "SynchroMode", "FIFO", "IrqStep",
                     "Pages", "AutoInit", "AdChannel", "AdPorog", "NCh", "IrqEna", "AdcEna"]
        for par_name in par_names:
            parameters[par_name] = ADCpar[par_name]
        """
        self.adcPar.t3.Chn[0] = e440.CH_0 | e440.V10000 | e440.dCH_TYPE[ADCpar["Ch0Mode"]]
        # adcPar.t3.Chn[1] = e140.CH_1 | e140.V2500         # e440.CH_1 | e440.V2500    e154.CH_1 | e154.V1600
        # adcPar.t3.Chn[2] = e140.CH_2 | e140.V0625         # e440.CH_2 | e440.V0625    e154.CH_2 | e154.V0500
        # adcPar.t3.Chn[3] = e140.CH_3 | e140.V0156         # e440.CH_3 | e440.V0156    e154.CH_3 | e154.V0160
        """
    return self.SetParameters(parameters)


    def SetParameters(par_dict : {}):
        for key in par_dict.keys():
            if key in self.DeviceParameters.keys():
                self.DeviceParameters[key] = par_dict[key]
    return self.DeviceParameters
    
