# Description: Script to send data through USB serial port and listen for a response
#
# :authors: [Justin Pan, Zach Burnett]
#
# Last Updated: 06/09/2017

import time
import unittest

import serial
from serial.tools import list_ports

SERIAL_PORT = '/dev/cu.usbmodem14101'
BAUD_RATE = 1200


class TestHASPSerial(unittest.TestCase):
    def test_connection(self):
        ports = serial.tools.list_ports.comports()
        self.assertGreater(len(ports), 0)

    def test_status_request(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the status request command will return the status
            serial_connection.write('P')
            time.sleep(0.25)
            received_data = serial_connection.read()
            self.assertIn('DAS status: OFF', received_data)

    def test_disarming_sequence(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the arming command arms a disarmed system
            serial_connection.write('A')
            time.sleep(0.25)
            serial_connection.write('P')
            time.sleep(0.25)
            received_data = serial_connection.read()
            self.assertIn('DAS status: ARMED', received_data)

            # validate that the disarming command disarms an armed system
            serial_connection.write('D')
            time.sleep(0.25)
            serial_connection.write('P')
            time.sleep(0.25)
            received_data = serial_connection.read()
            self.assertIn('DAS status: OFF', received_data)

    def test_activation_sequence(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the activation command does not activate a disarmed system
            serial_connection.write('T')
            time.sleep(0.25)
            serial_connection.write('P')
            time.sleep(0.25)
            received_data = serial_connection.read()
            self.assertIn('DAS status: OFF', received_data)

            # validate that the activation command activates an armed system
            serial_connection.write('T')
            time.sleep(0.25)
            serial_connection.write('P')
            time.sleep(0.25)
            received_data = serial_connection.read()
            self.assertIn('DAS status: ACTIVE', received_data)

            # validate that the disarming command deactivates an active system
            serial_connection.write('D')
            time.sleep(0.25)
            serial_connection.write('P')
            time.sleep(0.25)
            received_data = serial_connection.read()
            self.assertIn('DAS status: OFF', received_data)


def open_ports() -> str:
    """
    Iterate over available serial ports.

    :return: port name
    """

    for com_port in serial.tools.list_ports.comports():
        yield com_port.device
    else:
        return None


def next_open_port() -> str:
    """
    Get next port in ports list.

    :return: port name
    """

    try:
        return next(open_ports())
    except StopIteration:
        raise OSError('No open serial ports.')


if __name__ == '__main__':
    unittest.main()
