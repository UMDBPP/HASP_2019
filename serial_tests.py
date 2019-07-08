# Description: Script to send data through USB serial port and listen for a response
#
# :authors: [Justin Pan, Zach Burnett]
#
# Last Updated: 06/09/2017

import unittest

import serial
from serial.tools import list_ports

BAUD_RATE = 1200


def open_ports() -> str:
    for com_port in serial.tools.list_ports.comports():
        yield com_port.device
    else:
        return None


def next_open_port() -> str:
    try:
        return next(open_ports())
    except StopIteration:
        raise OSError('No open serial ports.')


SERIAL_PORT = next_open_port()  # '/dev/cu.usbmodem14101'


class TestHASPSerial(unittest.TestCase):
    def test_status(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the status request command will return the status
            serial_connection.write(b'P')
            received_message = str(serial_connection.readlines()[-1], encoding='ASCII')

            assert 'DAS status: OFF' in received_message

    def test_arming(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the arming command arms a disarmed system
            serial_connection.write(b'A')
            serial_connection.write(b'P')
            received_message = str(serial_connection.readlines()[-1], encoding='ASCII')

            assert 'DAS status: ARMED' in received_message

            # validate that the disarming command disarms an armed system
            serial_connection.write(b'D')
            serial_connection.write(b'P')
            received_message = str(serial_connection.readlines()[-1], encoding='ASCII')

            assert 'DAS status: OFF' in received_message

    def test_disarmed_activation(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the activation command does not activate a disarmed system
            serial_connection.write(b'T')
            serial_connection.write(b'P')
            received_message = str(serial_connection.readlines()[-1], encoding='ASCII')

            assert 'DAS status: OFF' in received_message

    def test_activation(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the activation command activates an armed system
            serial_connection.write(b'A')
            serial_connection.write(b'T')
            serial_connection.write(b'P')
            received_message = str(serial_connection.readlines()[-1], encoding='ASCII')

            assert 'DAS status: ACTIVE' in received_message

            serial_connection.write(b'D')

    def test_deactivation(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the disarming command deactivates an active system
            serial_connection.write(b'A')
            serial_connection.write(b'T')
            serial_connection.write(b'D')
            serial_connection.write(b'P')
            received_message = str(serial_connection.readlines()[-1], encoding='ASCII')

            assert 'DAS status: OFF' in received_message


if __name__ == '__main__':
    unittest.main()
