/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"

#define motorSerial Serial1 // tells compiler to replace all motorSerial as Serial1


// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Digging setups
// const int digPin = 9;
bool isDigging = false; // check if the digging motor is already on
// int stepsPerRevolution = 2048;
// Stepper digStepper (stepsPerRevolution, 4, 6, 5, 7);
// int rpm = 15;
const int relayPin = D6;

// const int ledPin = 7;  // or use a custom pin like 7

// heartbeat setup
unsigned long lastCommandTime = 0;
const unsigned long commandTimeout = 500;  // ms

unsigned long lastCommandDigTime = 0;
const unsigned long commandDigTimeout = 30000;  // ms

//Declare functions
int setLED(String command);
int autoMove(String command);
int manualMove(String command);
void stopMotors();
void shouldDig();
void diggingOn();
void diggingOff();


// setup() runs once, when the device is first turned on
void setup() {
  // Put initialization like pinMode and begin functions here
  Serial.begin(9600); // USB debugging
  motorSerial.begin(9600); //Use hardware Serial1 for motors
  // Particle.function("setLED", setLED);
  Particle.function("Command Movement", manualMove);

  stopMotors(); // ensure motor is stop

  // pinMode(ledPin, OUTPUT); // LED
  // digitalWrite(ledPin, LOW); // start off
  // digitalWrite(12, HIGH); // dig motor low

  // digStepper.setSpeed(rpm);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // high is OFF, the way I wired it..?
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // The core of your code will likely live here.
  if (Serial.available()) {
    char cmd = Serial.read();
    lastCommandTime = millis(); //reset heartbeat timer

    // digitalWrite(ledPin, HIGH);
    // delay(20);                  // short blink, non-blocking if you want to risk it
    // digitalWrite(ledPin, LOW);

    autoMove(String(cmd));
    // switch (cmd) {
    //   case 'f':
    //     // motor forward logic
    //     motorSerial.write(127); // M1, Forward
    //     motorSerial.write(255); // M2, Forward
    //     break;
    //   case 'b':
    //     // motor backward logic
    //     motorSerial.write(1); // M1, Reverse
    //     motorSerial.write(128); // M2, Reverse
    //     break;
    //   case 'l':
    //     // left turn logic
    //     motorSerial.write(1); // M1, Reverse
    //     motorSerial.write(255); // M2, Forward
    //     break;
    //   case 'r':
    //     // right turn logic
    //     motorSerial.write(127); // M1, Forward
    //     motorSerial.write(128); // M2, Reverse
    //     break;
    //   case 's':
    //     stopMotors();
    //     break;
    //   case 'd':
    //     Serial.println("should dig triggered");
    //     shouldDig();
    //     break;
    //   default:
    //     break;
    // }
  }

  //heartbeat not received, kill the motors
  if (millis() - lastCommandTime > commandTimeout) {
    stopMotors();
    // diggingOff();
  }
  if (millis() - lastCommandDigTime > commandDigTimeout) {
    diggingOff();
  }

  // Example: Publish event to cloud every 10 seconds. Uncomment the next 3 lines to try it!
  // Log.info("Sending Hello World to the cloud!");
  // Particle.publish("Hello world!");
  // delay( 10 * 1000 ); // milliseconds and blocking - see docs for more info!
}

int setLED(String command) {
  if (command == "on") {
    digitalWrite(D7, HIGH);
    return 1;
  } else if (command == "off") {
    digitalWrite(D7, LOW);
    return 0;
  }
  return -1;
}

int autoMove(String command) {
  char cmd = command.charAt(0);
  switch (cmd) {
    case 'f':
      // motor forward logic
      motorSerial.write(127); // M1, Forward
      motorSerial.write(255); // M2, Forward
      break;
    case 'b':
      // motor backward logic
      motorSerial.write(1); // M1, Reverse
      motorSerial.write(128); // M2, Reverse
      break;
    case 'l':
      // left turn logic
      motorSerial.write(1); // M1, Reverse
      motorSerial.write(255); // M2, Forward
      break;
    case 'r':
      // right turn logic
      motorSerial.write(127); // M1, Forward
      motorSerial.write(128); // M2, Reverse
      break;
    case 's':
      stopMotors();
      break;
    case 'd':
      Serial.println("should dig triggered");
      shouldDig();
      break;
    default:
      break;
  }
  return 0;
}

int manualMove(String command) {
  unsigned long commandStart = millis();
  unsigned long elapsedTime = 0;
  char cmd = command.charAt(0);

  // perform action for 2 seconds
  if (cmd == 'd'){
    shouldDig();
  }
  while(elapsedTime < 2000){
    elapsedTime = millis() - commandStart;
    switch (cmd) {
      case 'f':
        // motor forward logic
        motorSerial.write(127); // M1, Forward
        motorSerial.write(255); // M2, Forward
        break;
      case 'b':
        // motor backward logic
        motorSerial.write(1); // M1, Reverse
        motorSerial.write(128); // M2, Reverse
        break;
      case 'l':
        // left turn logic
        motorSerial.write(1); // M1, Reverse
        motorSerial.write(255); // M2, Forward
        break;
      case 'r':
        // right turn logic
        motorSerial.write(127); // M1, Forward
        motorSerial.write(128); // M2, Reverse
        break;
      case 's':
        stopMotors();
        break;
      // case 'd':
      //   Serial.println("should dig triggered");
      //   shouldDig();
      //   break;
      default:
        break;
    }
  }
  return 0;
}

void stopMotors() {
  motorSerial.write(64);  // M1 stop
  motorSerial.write(192);  // M2 stop
}

void shouldDig(){
  if(isDigging == false){
    lastCommandDigTime = millis();
    diggingOn();
    // isDigging = true;
    Serial.println("on");
  }
  else{
    diggingOff();
    // isDigging = false;
    Serial.println("off");
  }
}

void diggingOn(){
  // digitalWrite(digPin, HIGH);
  digitalWrite(relayPin, LOW);  // Turn on relay → motor spins
  isDigging = true;
  // delay(2000);
  // Serial.println("confirm dig on");
}

void diggingOff(){
  // digitalWrite(digPin, LOW);
  digitalWrite(relayPin, HIGH);   // Turn off relay → motor stops
  isDigging = false;
   // reset timer
  // Serial.println("confirm dig off");
  // delay(2000);
}