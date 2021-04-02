from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QPlainTextEdit, QLineEdit, QWidget, QTabWidget, \
    QPushButton
from PyQt5 import QtCore
from PyQt5.QtCore import *

import sys
from datetime import datetime

from MCD_Communicator.src.modules import port, messages


class CommunicatorLog(QPlainTextEdit):
    def __init__(self):
        super().__init__()
        self.setFocusPolicy(Qt.NoFocus)

    @pyqtSlot(str)
    def on_new_tx_text(self, text):
        prefix = "[" + self.get_time() + "] "
        self.insertPlainText(prefix + text + '\n')

    def get_time(self) -> str:
        now = datetime.now()
        return now.strftime("%d/%m/%Y-%H:%M:%S.%f")

class CommunicatorInputLine(QLineEdit):
    text_signal = QtCore.pyqtSignal(str)

    def __init__(self) -> None:
        super().__init__()
        self._input_text = ""

        self.returnPressed.connect(self.on_return_pressed)
        self.textChanged.connect(self.on_text_changed)

    def on_return_pressed(self):
        self.text_signal.emit(self._input_text)
        self.clear()

    def on_text_changed(self, text):
        self._input_text = text



def configure_free_communicator() -> QWidget:
    free_communicator = QWidget()
    layout = QVBoxLayout()

    communicator_log = CommunicatorLog()
    communicator_input_line = CommunicatorInputLine()
    communicator_input_line.text_signal.connect(communicator_log.on_new_tx_text)

    layout.addWidget(communicator_log)
    layout.addWidget(communicator_input_line)

    free_communicator.setLayout(layout)
    return free_communicator


class Window(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setGeometry(0, 0, 600, 400)
        self.setWindowTitle("MCD Communicator")
        self._connection_button = self.create_connection_button()
        window_layout = QVBoxLayout()
        window_layout.addWidget(self._connection_button)

        main_widget = QTabWidget()
        self._free_communicator_tab = configure_free_communicator()
        main_widget.addTab(self._free_communicator_tab, "Free Communicator")
        self._message_communicator = QWidget()
        main_widget.addTab(self._message_communicator, "Message Communicator")
        window_layout.addWidget(main_widget)

        layout_widget = QWidget()
        layout_widget.setLayout(window_layout)
        self.setCentralWidget(layout_widget)
        self.setFocus()
        self.show()

    def create_connection_button(self):
        connection_button = QPushButton("Connect")
        connection_button.clicked.connect(self.on_button_clicked)
        return connection_button

    def on_button_clicked(self):
        transitions_map = {
            "Connect": "Disconnect",
            "Disconnect": "Connect",
        }
        self._connection_button.setText(transitions_map[self._connection_button.text()])


if __name__ == '__main__':
    req = messages.PlatformSetMotorSpeedReq(1, -1, 50)
    print(req.serialize())

    app = QApplication(sys.argv)

    window = Window()
    sys.exit(app.exec_())
