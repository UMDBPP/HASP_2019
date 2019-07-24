# Description: Script to send data through USB serial port and listen for a response
#
# :authors: [Justin Pan, Zach Burnett]
#
# Last Updated: 06/09/2017
import sys
import unittest

import serial
from serial.tools import list_ports

SERIAL_PORT = None
BAUD_RATE = 1200


class TestHASPSerial(unittest.TestCase):
    def test_status(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the status request command will return the status
            send_packet(serial_connection, b'P')
            assert 'DAS status: OFF' in str(serial_connection.readlines()[-1], encoding='ASCII')

    def test_arming(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the arming command arms a disarmed system
            send_packet(serial_connection, b'A')
            assert 'DAS status: ARMED' in str(serial_connection.readlines()[-1], encoding='ASCII')

            # validate that the disarming command disarms an armed system
            send_packet(serial_connection, b'D')
            assert 'DAS status: OFF' in str(serial_connection.readlines()[-1], encoding='ASCII')

    def test_disarmed_activation(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the activation command does not activate a disarmed system
            send_packet(serial_connection, b'T')
            assert 'DAS status: OFF' in str(serial_connection.readlines()[-1], encoding='ASCII')

    def test_activation(self):
        with serial.Serial(port=SERIAL_PORT, baudrate=BAUD_RATE, parity=serial.PARITY_NONE, bytesize=serial.EIGHTBITS,
                           timeout=1) as serial_connection:
            # validate that the activation command activates an armed system
            send_packet(serial_connection, b'AT')
            assert 'DAS status: ACTIVE' in str(serial_connection.readlines()[-1], encoding='ASCII')

            # validate that the disarming command deactivates an active system
            send_packet(serial_connection, b'D')
            assert 'DAS status: OFF' in str(serial_connection.readlines()[-1], encoding='ASCII')


def send_packet(serial_connection: serial.Serial, data: bytes):
    for packet_byte in (0x01, 0x30, None, None, 0x03, 0x0D, 0x0A):
        if packet_byte is None:
            if len(data) == 1:
                serial_connection.write(bytes(0x00))
                serial_connection.write(data)
            else:
                for data_byte in data:
                    serial_connection.write(data_byte)
        else:
            serial_connection.write(packet_byte)


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


if __name__ == '__main__':
    if SERIAL_PORT is None:
        SERIAL_PORT = sys.argv[1] if len(sys.argv) > 1 else next_open_port()

    unittest.main()
