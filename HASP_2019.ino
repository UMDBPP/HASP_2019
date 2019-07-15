/* whether to print verbose debugging messages to serial */
#define VERBOSE false

/* seconds between status downlinks */
#define STATUS_UPDATE_INTERVAL_SECONDS 60

/* seconds before system disarms after being armed */
#define ARMING_TIMEOUT_SECONDS 30

/* pin numbers */
#define EOC_RELAY_PIN 10
#define CVA_RELAY_PIN_1 11
#define CVA_RELAY_PIN_2 12

/* incoming serial messages */
#define COMMAND_REQUEST_STATUS 'P'
#define COMMAND_ARM 'A'
#define COMMAND_DISARM 'D'
#define COMMAND_ACTIVATE_RELAYS 'T'

/* baud rate of HASP serial interface */
#define HASP_BAUD_RATE 1200

/* global variables */
unsigned long current_millis = 0;
unsigned long previous_status_update_millis = 0;
unsigned long arming_millis = 0;
bool RELAY_TRIGGERS_ARMED = false;
bool RELAYS_POWERED = false;

void setup() {
  Serial.begin(HASP_BAUD_RATE);
  debug_message("initializing FISH relay control...");

  pinMode(EOC_RELAY_PIN, OUTPUT);
  pinMode(CVA_RELAY_PIN_1, OUTPUT);
  pinMode(CVA_RELAY_PIN_2, OUTPUT);

  send_relay_status();
}

void loop() {
  current_millis = millis();

  /* read the serial buffer and see if anything has come in since the last iteration */
  if (Serial.available() > 0) {
    byte header[2];
    Serial.readBytes(header, 2);

    char incoming_data[2];
    Serial.readBytes(incoming_data, 2);

    byte footer[2];
    Serial.readBytes(footer, 2);

    char incoming_command = incoming_data[1];
    
    /* perform task given a char command code */
    switch (incoming_command) {
      case COMMAND_REQUEST_STATUS:
        send_relay_status();
        break;
      case COMMAND_ARM:
        if (RELAYS_POWERED) {
          debug_message("relays are already on");
        } else {
          debug_message("arming relay triggers for " + String(ARMING_TIMEOUT_SECONDS) + "s");
          RELAY_TRIGGERS_ARMED = true;
          arming_millis = current_millis;
        }
        send_relay_status();
        break;
      case COMMAND_DISARM:
        if (RELAYS_POWERED) {
          set_relay_power(false);
        }
        if (RELAY_TRIGGERS_ARMED) {
          debug_message("disarming relay triggers due to command");
          RELAY_TRIGGERS_ARMED = false;
        } else {
          debug_message("relay triggers are already disarmed");
        }
        send_relay_status();
        break;
      case COMMAND_ACTIVATE_RELAYS:
        if (RELAYS_POWERED) {
          debug_message("relays are already on");
        } else if (RELAY_TRIGGERS_ARMED) {
          set_relay_power(true);
        } else {
          debug_message("relay triggers are not armed");
        }
        send_relay_status();
        break;
    }
  } else {
    /* disarm relay triggers if the time since arming exceeds the timeout */
    if (RELAY_TRIGGERS_ARMED && !RELAYS_POWERED && (current_millis - arming_millis >= ARMING_TIMEOUT_SECONDS * 1000)) {
      debug_message("disarming relay triggers due to timeout");
      RELAY_TRIGGERS_ARMED = false;
      send_relay_status();
    }

    /* send status if the time since the last status update exceeds the interval */
    if (current_millis - previous_status_update_millis >= (unsigned long) STATUS_UPDATE_INTERVAL_SECONDS * 1000) {
      previous_status_update_millis = current_millis;
      send_relay_status();
    }
  }
}

/* control power to relays */
void set_relay_power(bool power) {
  if (power) {
    debug_message("switching relays on");
    digitalWrite(EOC_RELAY_PIN, HIGH);
    digitalWrite(CVA_RELAY_PIN_1, HIGH);
    digitalWrite(CVA_RELAY_PIN_2, HIGH);
    RELAYS_POWERED = true;
  } else {
    debug_message("switching relays off");
    digitalWrite(EOC_RELAY_PIN, LOW);
    digitalWrite(CVA_RELAY_PIN_1, LOW);
    digitalWrite(CVA_RELAY_PIN_2, LOW);
    RELAYS_POWERED = false;
  }
}

/* get status of relays */
String get_relay_status() {
  if (RELAYS_POWERED) {
    return "ACTIVE";
  } else if (RELAY_TRIGGERS_ARMED) {
    return "ARMED";
  } else {
    return "OFF";
  }
}

/* send relay status over serial */
void send_relay_status() {
  String relay_status = get_relay_status();
  debug_message("transmitting relay status: " + relay_status);
  Serial.println(String(current_millis) + ", DAS status: " + relay_status);
}

/* print to serial if debugging verbosity is enabled */
void debug_message(String message) {
  if (VERBOSE) {
    Serial.println(String(current_millis) + ", DEBUG MSG : " + message);
  }
}
