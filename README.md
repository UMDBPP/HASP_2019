# HASP_2019

[![Build Status](https://travis-ci.org/UMDBPP/HASP_2019.svg?branch=master)](https://travis-ci.org/UMDBPP/HASP_2019)

### University of Maryland Balloon Payload Program

The [High Altitude Student Platform](https://laspace.lsu.edu/hasp/) is a project run in conjuction with NASA and LSU to send student research payloads into the upper atmosphere on a floating flight profile.

The University of Maryland's 2019 payload (FISH) is a sensor suite designed to characterize high-altitude turbulence with high temporal resolution.
It utilizes a [DTS Slice Micro](https://www.dtsweb.com/slice-micro-daq/) to record data from several onboard sensors, with power and timing controlled by an Arduino Uno.

#### Testing

Python unit tests for the Arduino controller are provided in the `tests.py` file. 
To run these tests, you will need to install both the [Arduino IDE](https://www.arduino.cc/en/Main/Software) and [Python](https://www.python.org/downloads/), along with the PySerial library (`pip3 install pyserial`). Then, follow these steps:

1. upload the provided Arduino sketch (`HASP_2019.ino`) to an Arduino Uno, and keep it connected to your computer
2. open a command line and `cd` to the location on your computer where you cloned this repository
3. run the following command: `python3 tests.py`
