#include <SoftwareSerial.h>

/* pin numbers */
#define EOC_RELAY_PIN 10
#define CVA_RELAY_PIN_1 11
#define CVA_RELAY_PIN_2 12
//#define EOC_STATUS_PIN 13

/* incoming serial messages */
#define COMMAND_ARM 'A'
#define COMMAND_DISARM 'D'
#define COMMAND_RELAYS_ON 'T'
#define COMMAND_REQUEST_STATUS 'P'

/* seconds before system disarms after being armed */
#define ARMING_TIMEOUT_SECONDS 30

/* seconds between program loop iterations */
#define INTERVAL_SECONDS 5

/* global variables */
unsigned long int current_millis = 0;
unsigned long int previous_millis = 0;
unsigned long int arming_millis = 0;
bool RELAY_TRIGGERS_ARMED = false;
bool RELAYS_POWERED = false;

/* make a software serial interface for debugging */
//SoftwareSerial debug_serial(10, 11);
HardwareSerial debug_serial = Serial;

void setup() {
  debug_serial.begin(9600);
  debug_serial.println("[" + String(millis()) + "]: initializing FISH relay control...");

  Serial.begin(1200);
  pinMode(EOC_RELAY_PIN, OUTPUT);
  pinMode(CVA_RELAY_PIN_1, OUTPUT);
  pinMode(CVA_RELAY_PIN_2, OUTPUT);
//  pinMode(EOC_STATUS_PIN, INPUT);
}

void loop() {
  current_millis = millis();

  /* disarm relay triggers if timeout is exceeded */
  if (RELAY_TRIGGERS_ARMED && current_millis - arming_millis >= ARMING_TIMEOUT_SECONDS * 1000) {
    debug_serial.println("[" + String(current_millis) + "]: disarming relay triggers due to timeout");
  }

  /* read the serial buffer and see if anything has come in since the last iteration */
  if (Serial.available() > 0) {
    char incoming_command = Serial.read();
    execute_command(incoming_command);
  }

  if (current_millis - previous_millis >= INTERVAL_SECONDS * 1000) {
    String relay_status = get_relay_status();
    debug_serial.println("[" + String(current_millis) + "]: transmitting relay status: " + relay_status);
    Serial.println(String(current_millis) + ", DAS status: " + relay_status);

    previous_millis = current_millis;
  }
}

/* perform task given a char command code */
void execute_command(char incoming_command) {
  String relay_status;
  
  switch (incoming_command) {
    case COMMAND_ARM:
      if (RELAYS_POWERED) {
        debug_serial.println("[" + String(current_millis) + "]: relays are already powered");
      } else {
        debug_serial.println("[" + String(current_millis) + "]: arming relay triggers for " + String(ARMING_TIMEOUT_SECONDS) + "s");
      }

      RELAY_TRIGGERS_ARMED = true;
      arming_millis = current_millis;
      break;
    case COMMAND_DISARM:
      if (RELAYS_POWERED) {
        debug_serial.println("[" + String(current_millis) + "]: switching relays off");
        digitalWrite(CVA_RELAY_PIN_1, LOW);
        digitalWrite(CVA_RELAY_PIN_2, LOW);
        digitalWrite(EOC_RELAY_PIN, LOW);
        RELAYS_POWERED = false;
      } else if (RELAY_TRIGGERS_ARMED) {
        debug_serial.println("[" + String(current_millis) + "]: disarming relay triggers due to command");
      } else {
        debug_serial.println("[" + String(current_millis) + "]: relay triggers are not armed");
      }

      RELAY_TRIGGERS_ARMED = false;
      break;
    case COMMAND_RELAYS_ON:
      if (RELAYS_POWERED) {
        debug_serial.println("[" + String(current_millis) + "]: relays are already on");
      } else if (RELAY_TRIGGERS_ARMED) {
        debug_serial.println("[" + String(current_millis) + "]: switching relays on");
        digitalWrite(CVA_RELAY_PIN_1, HIGH);
        digitalWrite(CVA_RELAY_PIN_2, HIGH);
        digitalWrite(EOC_RELAY_PIN, HIGH);
      } else {
        debug_serial.println("[" + String(current_millis) + "]: relay triggers are not armed");
      }

      break;
    case COMMAND_REQUEST_STATUS:
      relay_status = get_relay_status();
      debug_serial.println("[" + String(current_millis) + "]: transmitting relay status: " + relay_status);
      Serial.println(String(current_millis) + ", DAS status: " + relay_status);
      break;
    default:
        debug_serial.println("[" + String(current_millis) + "]: received unknown command: " + String(incoming_command));
        break;
      }
}

/* get status of relays */
String get_relay_status() {
  // if (digitalRead(EOC_STATUS_PIN) == HIGH) {
  if (RELAYS_POWERED) {
    return "ACTIVE";
  } else if (RELAY_TRIGGERS_ARMED) {
    return "ARMED";
  } else {
    return "OFF";
  }
}
