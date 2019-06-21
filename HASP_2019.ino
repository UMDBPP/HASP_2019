/* pin numbers */
#define EOC_RELAY_PIN 2
#define CVA_RELAY_PIN 3
#define EOC_STATUS_PIN 4

/* incoming serial messages */
#define EOC_RELAY_ON_COMMAND 'O'
#define CVA_RELAY_ON_COMMAND 'T'
#define STATUS_COMMAND 'P'

/* seconds between program iterations */
#define INTERVAL_SECONDS 5

/* filename of data log */
#define DATA_LOG_FILENAME "datalog.txt"

/* global variables to store times */
unsigned long int current_millis = 0;
unsigned long int previous_millis = 0;

void setup() {
  Serial.begin(1200);
  Serial.println("Initializing...");
  pinMode(EOC_RELAY_PIN, OUTPUT);
  pinMode(CVA_RELAY_PIN, OUTPUT);
  pinMode(EOC_STATUS_PIN, INPUT);
}

void loop() {
  current_millis = millis();
  read_serial();

  if (current_millis - previous_millis >= INTERVAL_SECONDS * 1000) {
    Serial.print("[" + current_millis + "]: EOC status: " + get_eoc_status());
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
        digitalWrite(EOC_RELAY_PIN, HIGH);
        Serial.println("[" + current_millis + "]: triggering EOC relay");
        break;
      case CVA_RELAY_ON_COMMAND:
        digitalWrite(CVA_RELAY_PIN, HIGH);
        Serial.println("[" + current_millis + "]: triggering CVA relay");
        break;
      case STATUS_COMMAND:
        Serial.print("[" + current_millis + "]: EOC status: " + get_eoc_status());
        break;
      default:
        Serial.println("[" + current_millis + "]: received unknown command: " + incoming_command);
        break;
    }
  }
}

/* get status of EOC from its status LED: either recording or not recording */
String get_eoc_status() {
  String eoc_status;

  if (digitalRead(EOC_STATUS_PIN) == HIGH) {
    eoc_status = "RECORDING";
  } else {
    eoc_status = "NOT RECORDING";
  }

  return eoc_status;
}
