    type = "Undefined_Device"

    def __init__(self, config_filename: str):
        self._ConfigFilename = config_filename
        self._MeasurementsFile = None

    @property
    def ConfigFilename(self) -> str:
        return self._ConfigFilename

    @property
    def CurrentMeasurementsFile(self):
        return self._MeasurementsFile

    def ConnectToPhysicalDevice(self, args):
        # Все действия, необходимые при первом
        # налаживании общения с устройством
        return

    def DisconnectFromPhysicalDevice(self):
        # Все действия, необходимые при отключении
        # отключении нас от управления этим устройством
        # aka отдаем управление им системе
        return

    def LoadConfiguration(self):
        # Загружаем все параметры из .config файла
        return

    def StartMeasurements(self, measurements_file: str):
        self._MeasurementsFile = open(measurements_file)
        # Создание и инициализация потока
        # для записи в файл.
        # Передача в поток функции self.TakeMeasurements
        return

    def TakeMeasurements(self):
        # функция, запускаемая в потоке и проверяющая,
        # пора ли запрашивать новые данные от устройства

        # При необходимости запрашивает данные и записывает их в файл
        # self._MeasurementsFile. Периодически запускает fflush()

        # При обнаружении ошибок / условий для прекращения снятия данных
        # может самостоятельно запустить self.FinishMeasurements
        return

    def FinishMeasurements(self):
        # Корректное закрытие файла self._MeasurementsFile
        # Корректное уничтожение дочернего потока
        return

    def GetDeviceParameters(self):
        return
