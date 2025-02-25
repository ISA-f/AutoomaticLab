import LcardDataInterface as LDIF
import Device_Korad as DKorad
import pandas as pd
myData = pd.DataFrame(columns = {**LDIF.LCARD_NAMES._member_map_,
                                 **DKorad.KORAD_NAMES._member_map_}.values())
myDataColumnDict = {**LDIF.LCARD_NAMES._value2member_map_,
                         **DKorad.KORAD_NAMES._value2member_map_,
                         "Ua" : "Ua", "Ia" : "Ia", "Imin" : "Imin", "sigmaI" : "sigmaI"}
print(myDataColumnDict)
print()
myKorad = DKorad.Korad('Korad.ini')
korad_data = myKorad.TakeMeasurements()
print(myData)
print()
print(korad_data.to_frame().T)
print("-----------------------")
myData = pd.concat([myData, korad_data.to_frame().T])
myData = pd.concat([myData, korad_data.to_frame().T])
myData = pd.concat([myData, korad_data.to_frame().T])
print(myData)
