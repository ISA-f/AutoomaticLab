from Lcard_EmptyDevice import LcardE2010B_EmptyDevice
import threading
import time

class LcardSyncdController:
    def __init__(self, LcardDevice):
        self.myThread = None
        self.myEventListener = None
        self.IsActiveController = False
        self.ThreadSleepTime = None
        self.myLcardDevice = LcardDevice

    def startController(self, EventListener, ThreadSleepTime):
        self.ThreadSleepTime = ThreadSleepTime
        self.myEventListener = EventListener
        self.myThread = threading.Thread(target = self.checkWithTimeout)
        self.IsActiveController = True
        self.myThread.start()

    def checkWithTimeout(self):
        while self.IsActiveController:
            time.sleep(self.ThreadSleepTime)
            self.myEventListener(self.myLcardDevice.syncd())

    def finishController(self):
        self.IsActiveController = False
        if self.myThread:
            self.myThread.join()
        self.myThread = None

if __name__ == "__main__":
    def example(syncd):
        if syncd > 50000:
            print("syncd > 50000 !!!")
        return
    lcard = LcardE2010B_EmptyDevice("LcardE2010B.ini")
    lcard_controller = LcardSyncdController(lcard)
    
    lcard.connectToPhysicalDevice()
    lcard.loadConfiguration()
    lcard.startMeasurements()
    lcard_controller.startController(example, 0.5)
    
    time.sleep(10)
    
    lcard.finishMeasurements()
    lcard.disconnectFromPhysicalDevice()
