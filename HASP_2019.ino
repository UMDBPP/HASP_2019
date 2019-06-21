#include <SoftwareSerial.h>

/* pin numbers */
#define EOC_RELAY_PIN 2
#define CVA_RELAY_PIN 3
#define EOC_STATUS_PIN 4

/* incoming serial messages */
#define EOC_RELAY_ON_COMMAND 'O'
#define CVA_RELAY_ON_COMMAND 'T'
#define STATUS_COMMAND 'P'

/* outgoing serial messages */
#define EOC_STATUS_RECORDING_ON 'A'
#define EOC_STATUS_RECORDING_OFF 'W'

/* seconds between program iterations */
#define INTERVAL_SECONDS 5

/* filename of data log */
#define DATA_LOG_FILENAME "datalog.txt"

/* global variables to store times */
unsigned long int current_millis = 0;
unsigned long int previous_millis = 0;

/* make a software serial interface for debugging */
SoftwareSerial debug_serial(10, 11);

void setup() {
  Serial.begin(1200);
  debug_serial.begin(9600);
  debug_serial.println("Initializing...");
  pinMode(EOC_RELAY_PIN, OUTPUT);
  pinMode(CVA_RELAY_PIN, OUTPUT);
  pinMode(EOC_STATUS_PIN, INPUT);
}

void loop() {
  current_millis = millis();
  read_serial();

  if (current_millis - previous_millis >= INTERVAL_SECONDS * 1000) {
    char eoc_status = get_eoc_status();
    debug_serial.println("[" + String(current_millis) + "]: EOC status: " + String(eoc_status));
    Serial.println(eoc_status);
    previous_millis = current_millis;
  }
}

/* read the serial buffer and see if anything has come in since the last iteration */
void read_serial(void)
{
  char incoming_command;

  if (Serial.available() > 0) {
    incoming_command = Serial.read();

    switch (incoming_command) {
      case EOC_RELAY_ON_COMMAND:
        debug_serial.println("[" + String(current_millis) + "]: triggering EOC relay");
        digitalWrite(EOC_RELAY_PIN, HIGH);
        break;
      case CVA_RELAY_ON_COMMAND:
        debug_serial.println("[" + String(current_millis) + "]: triggering CVA relay");
        digitalWrite(CVA_RELAY_PIN, HIGH);
        break;
      case STATUS_COMMAND:
        debug_serial.println("[" + String(current_millis) + "]: transmitting EOC status: " + String(get_eoc_status()));
        Serial.println(get_eoc_status());
        break;
      default:
        debug_serial.println("[" + String(current_millis) + "]: received unknown command: " + String(incoming_command));
        break;
    }
  }
}

/* get status of EOC from its status LED: either recording or not recording */
char get_eoc_status() {
  if (digitalRead(EOC_STATUS_PIN) == HIGH) {
    return EOC_STATUS_RECORDING_ON;
  } else {
    return EOC_STATUS_RECORDING_OFF;
  }
}
