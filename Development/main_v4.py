if __name__ == "__main__":
    import sys
    from PyQt5 import QtCore, QtGui, QtWidgets
    from MainWindow_CloseEvent import MainWindow_withCloseEvent
    from Tab_Device_Manager import TabDeviceManager

    app = QtWidgets.QApplication(sys.argv)
    MainWindow = MainWindow_withCloseEvent()
    ui = TabDeviceManager()
    ui.setupUi(MainWindow)
    MainWindow.CloseEventListeners.append(ui.onCloseEvent)
    MainWindow.show()
    sys.exit(app.exec_())
