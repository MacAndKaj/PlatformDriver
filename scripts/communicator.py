import serial
import numpy

dev = '/dev/ttyACM0'


class Message:
    FRAME_CTRL_DATA = 0xF0

    def message_id(self):
        raise NotImplementedError("Should be implemented in derived class!")

    def get_data(self):
        raise NotImplementedError("Should be implemented in derived class!")

    def serialize(self):
        data = bytes([self.FRAME_CTRL_DATA, self.message_id()])
        data.join(self.get_data())
        data.join(bytes([self.FRAME_CTRL_DATA]))
        return data


class PlatformSetMotorSpeedReq(Message):
    PLATFORM_SET_MOTOR_SPEED_REQ_ID = 0x01

    def __init__(self, motor: numpy.uint8, speedI: numpy.int8, speedF: numpy.uint8):
        self._motor = motor
        self._speedI = speedI
        self._speedF = speedF

    def message_id(self):
        return self.PLATFORM_SET_MOTOR_SPEED_REQ_ID

    def get_data(self):
        return bytes([self._motor, numpy.uint8(self._speedI), self._speedF])


if __name__ == '__main__':
    # port = serial.Serial(dev, timeout=1, baudrate=115200)
    # port.write()

    req = PlatformSetMotorSpeedReq(1, -1, 50)
