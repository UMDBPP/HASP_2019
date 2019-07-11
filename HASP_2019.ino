/* whether to print verbose debugging messages to serial */
#define VERBOSE false

/* seconds between status downlinks */
#define AUTODOWNLINK_INTERVAL_SECONDS 60

/* seconds before system disarms after being armed */
#define ARMING_TIMEOUT_SECONDS 10 // TODO this should be set to 30 seconds for the flight

/* pin numbers */
#define EOC_RELAY_PIN 10
#define CVA_RELAY_PIN_1 11
#define CVA_RELAY_PIN_2 12
#define EOC_STATUS_PIN 13

/* incoming serial messages */
#define COMMAND_ARM 'A'
#define COMMAND_DISARM 'D'
#define COMMAND_RELAYS_ON 'T'
#define COMMAND_REQUEST_STATUS 'P'

/* baud rate of HASP serial interface */
#define HASP_BAUD_RATE 1200

/* global variables */
unsigned long int current_millis = 0;
unsigned long int previous_millis = 0;
unsigned long int arming_millis = 0;
bool RELAY_TRIGGERS_ARMED = false;
bool RELAYS_POWERED = false;

void setup() {
  Serial.begin(HASP_BAUD_RATE);

  debug_println("[" + String(millis()) + "]: initializing FISH relay control...");

  pinMode(EOC_RELAY_PIN, OUTPUT);
  pinMode(CVA_RELAY_PIN_1, OUTPUT);
  pinMode(CVA_RELAY_PIN_2, OUTPUT);
  pinMode(EOC_STATUS_PIN, INPUT);

  set_relay_power(false);
}

void loop() {
  current_millis = millis();

  /* disarm relay triggers if timeout is exceeded */
  if (RELAY_TRIGGERS_ARMED && !RELAYS_POWERED && (current_millis - arming_millis >= ARMING_TIMEOUT_SECONDS * 1000)) {
    debug_println("[" + String(current_millis) + "]: disarming relay triggers due to timeout");
    
    set_relay_power(false);
  }

  /* read the serial buffer and see if anything has come in since the last iteration */
  if (Serial.available() > 0) {
    char incoming_command = Serial.read();
    execute_command(incoming_command);
  }

  if (current_millis - previous_millis >= AUTODOWNLINK_INTERVAL_SECONDS * 1000) {
    String relay_status = get_relay_status();
    debug_println("[" + String(current_millis) + "]: transmitting relay status: " + relay_status);

    Serial.println(String(current_millis) + ", DAS status: " + relay_status);
    previous_millis = current_millis;
  }
}

/* perform task given a char command code */
void execute_command(char incoming_command) {
  switch (incoming_command) {
    case COMMAND_ARM:
      if (RELAYS_POWERED) {
        debug_println("[" + String(current_millis) + "]: relays are already powered");
      } else {
        debug_println("[" + String(current_millis) + "]: arming relay triggers for " + String(ARMING_TIMEOUT_SECONDS) + "s");
      }

      RELAY_TRIGGERS_ARMED = true;
      arming_millis = current_millis;

      send_relay_status();
      break;
    case COMMAND_DISARM:
      if (RELAYS_POWERED) {
        set_relay_power(false);
      }

      if (RELAY_TRIGGERS_ARMED) {
        debug_println("[" + String(current_millis) + "]: disarming relay triggers due to command");
        RELAY_TRIGGERS_ARMED = false;
      } else {
        debug_println("[" + String(current_millis) + "]: unexpected input - relay triggers are not armed");
      }

      send_relay_status();
      break;
    case COMMAND_RELAYS_ON:
      if (RELAYS_POWERED) {
        debug_println("[" + String(current_millis) + "]: relays are already on");
      } else if (RELAY_TRIGGERS_ARMED) {
        set_relay_power(true);
      } else {
        debug_println("[" + String(current_millis) + "]: unexpected input - relay triggers are not armed");
      }

      send_relay_status();
      break;
    case COMMAND_REQUEST_STATUS:
      send_relay_status();
      break;
  }
}

/* send relay status over serial */
void send_relay_status() {
  String relay_status = get_relay_status();
  debug_println("[" + String(current_millis) + "]: transmitting relay status: " + relay_status);
  Serial.println(String(current_millis) + ", DAS status: " + relay_status);
}

/* control power to relays */
void set_relay_power(bool power) {
  if (power) {
    debug_println("[" + String(current_millis) + "]: switching relays on");
    digitalWrite(CVA_RELAY_PIN_1, HIGH);
    digitalWrite(CVA_RELAY_PIN_2, HIGH);
    digitalWrite(EOC_RELAY_PIN, HIGH);
    RELAYS_POWERED = true;
  } else {
    debug_println("[" + String(current_millis) + "]: switching relays off");
    digitalWrite(CVA_RELAY_PIN_1, LOW);
    digitalWrite(CVA_RELAY_PIN_2, LOW);
    digitalWrite(EOC_RELAY_PIN, LOW);
    RELAYS_POWERED = false;
  }
}

/* get status of relays */
String get_relay_status() {
  /* correct internal / external state discrepancy */
  if (digitalRead(EOC_STATUS_PIN) == HIGH && !RELAYS_POWERED) {
    RELAYS_POWERED = true;
  } else if (digitalRead(EOC_STATUS_PIN) == LOW && RELAYS_POWERED) {
    RELAYS_POWERED = false;
  }

  /* return string indicating relay power state */
  if (RELAYS_POWERED) {
    return "ACTIVE";
  } else if (RELAY_TRIGGERS_ARMED) {
    return "ARMED";
  } else {
    return "OFF";
  }
}

/* print to serial if debugging verbosity is enabled */
void debug_println(String message) {
  if (VERBOSE) {
    Serial.println(message);
  }
}
