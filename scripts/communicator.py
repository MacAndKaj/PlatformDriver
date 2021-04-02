from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QPlainTextEdit, QLineEdit, QWidget, QTabWidget
from PyQt5.QtCore import *

import serial
import sys

dev = '/dev/ttyACM0'


class Message:
    FRAME_CTRL_DATA = 0xF0

    def message_id(self):
        raise NotImplementedError("Should be implemented in derived class!")

    def get_data(self):
        raise NotImplementedError("Should be implemented in derived class!")

    def serialize(self):
        data = bytearray([self.FRAME_CTRL_DATA, self.message_id()])
        data.join(self.get_data())
        data.join([self.FRAME_CTRL_DATA.to_bytes(1, 'big')])
        return data


class PlatformSetMotorSpeedReq(Message):
    PLATFORM_SET_MOTOR_SPEED_REQ_ID = 0x01

    def __init__(self, motor, speedI, speedF):
        self._motor = motor
        self._speedI = speedI
        self._speedF = speedF

    def message_id(self):
        return self.PLATFORM_SET_MOTOR_SPEED_REQ_ID

    def get_data(self):
        return [self._motor.to_bytes(1, 'big'), self._speedI.to_bytes(1, 'big', signed=True),
                self._speedF.to_bytes(1, 'big')]


class CommunicatorLog(QPlainTextEdit):
    def __init__(self):
        super().__init__()
        self.setFocusPolicy(Qt.NoFocus)


class CommunicatorInputLine(QLineEdit):
    def __init__(self) -> None:
        super().__init__()
        self.input_text = ""

        self.returnPressed.connect(self.on_return_pressed)
        self.textChanged.connect(self.on_text_changed)

    def on_return_pressed(self):
        print(self.input_text)
        self.clear()

    def on_text_changed(self, text):
        self.input_text = text


def create_message_line_edit() -> QWidget:
    def on_return_pressed():
        global input_text
        print(input_text)

    def on_text_changed(text):
        global input_text
        input_text = text

    messageLine = QLineEdit()
    return messageLine


class Window(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setGeometry(0, 0, 600, 400)
        self.setWindowTitle("MCD Communicator")
        central_widget = QTabWidget()

        self._free_communicator_tab = self.configure_free_communicator()
        central_widget.addTab(self._free_communicator_tab, "Free Communicator")

        self._message_communicator = QWidget()
        central_widget.addTab(self._message_communicator, "Message Communicator")

        self.setCentralWidget(central_widget)
        self.setFocus()
        self.show()

    def configure_free_communicator(self) -> QWidget:
        free_communicator = QWidget()
        layout = QVBoxLayout()

        communicator_log = CommunicatorLog()
        communicator_input_line = CommunicatorInputLine()

        layout.addWidget(communicator_log)
        layout.addWidget(communicator_input_line)

        free_communicator.setLayout(layout)
        return free_communicator


class Port(QObject):

    def __init__(self):
        self._port = serial.Serial(dev, timeout=1, baudrate=115200)


if __name__ == '__main__':
    # port = serial.Serial(dev, timeout=1, baudrate=115200)

    req = PlatformSetMotorSpeedReq(1, -1, 50)
    print(req.serialize())

    app = QApplication(sys.argv)

    window = Window()
    sys.exit(app.exec_())

    # port.write(req.serialize())
    # port.close()
