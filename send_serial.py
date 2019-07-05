# Description: Script to send data through USB serial port and listen for a response
#
# :authors: [Justin Pan, Zach Burnett]
#
# Last Updated: 06/09/2017

import time

import serial
from serial.tools import list_ports


def ports() -> str:
    """
    Iterate over available serial ports.

    :return: port name
    """

    for com_port in serial.tools.list_ports.comports():
        yield com_port.device
    else:
        return None


def port() -> str:
    """
    Get next port in ports list.

    :return: port name
    """

    try:
        return next(ports())
    except StopIteration:
        raise OSError('No open serial ports.')


if __name__ == '__main__':
    serial_port = port()

    data = ('P', 'A', 'D', 'T', 'A', 'T')

    # hasp_packet_structure = (0x01, 0x02, 0x00, None, 0x03, 0x0D, 0x0A)

    serial_connection = serial.Serial(
        port=serial_port,
        baudrate=1200,
        parity=serial.PARITY_NONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
    )

    for character in data:
        print(f'transmit {character}')

        # for byte in hasp_packet_structure:
        #     if byte is None:
        #         byte = ord(character)

        serial_connection.write(character.encode())
        time.sleep(0.25)
        received_data = serial_connection.readlines()
        print(f'receive {received_data}')
