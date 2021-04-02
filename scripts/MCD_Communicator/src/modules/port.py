import serial

from PyQt5.QtCore import QObject

dev = '/dev/ttyACM0'


class Port(QObject):

    def __init__(self):
        super().__init__()
        self._port = serial.Serial(None, timeout=1, baudrate=115200)

    def __del__(self):
        self.close()

    def open(self):
        self._port.setPort(dev)
        self._port.open()

    def close(self):
        self._port.close()
        self._port.setPort(None)