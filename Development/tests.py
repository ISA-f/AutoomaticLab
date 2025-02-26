# ------ GUI --------------
import MainWindow_CloseEvent
import Tab_Device_Manager, filament_and_anode_tab, Lcard_VAC_GUI

# ----- Devices -----------
import Lcard_EmptyDevice, Device_Korad

# ----- Device Interfaces -------
import LcardDataInterface, Lcard_IF_FullBuffers
import Lcard_syncdController



tests = [Lcard_EmptyDevice.test, Device_Korad.test, # Devices
         LcardDataInterface.test,                   # Device Interfaces
         Lcard_syncdController.test, Lcard_IF_FullBuffers.test,
         filament_and_anode_tab.test,               # GUI
         Lcard_VAC_GUI.test,
         Tab_Device_Manager.test
         ]



if __name__ == "__main__":
    for itest in tests:
        try:
            itest()
            print(">> success")
            print()
        except Exception as e:
            print(">>", e)
            a = input()
