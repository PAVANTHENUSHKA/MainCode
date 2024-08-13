#include <Servo.h>
#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"
#include <Stepper.h>


//function prototype
void motoron(int IN_volt1, int IN_volt2, int Enable, Servo name);
void motoroff(int IN_volt1, int IN_volt2, int Enable, Servo name);
void display(String text);
void sealer();
float reading();
float input_weight();

LiquidCrystal_I2C lcd(0x27, 16, 2);
//motor pin define
int ENA = 9;
int IN1 = 10;
int IN2 = 11;
int ENB = 49;
int IN3 = 51;
int IN4 = 53;
//load cell pins_22,23,24,25
HX711 cell;
HX711 cell2;
float val = 0;
float w = 0;
float val2 = 0;
float w2 = 0;
//keypad define
const byte ROWS = 4;  // four rows
const byte COLS = 3;  // four columns
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

String weight = "";
float weightf = 0;                    // redundent variables defined globally and returned later in a fuctions and ssighed to input weight
byte rowPins[ROWS] = { 2, 3, 4, 5 };  // connect to the row pinouts of the keypad
byte colPins[COLS] = { 6, 7, 8 };     // connect to the column pinouts of the keypad
int num = 0;
bool goodschosen = false;

//2servo in lid
Servo myservo1;
Servo myservo2;
int pos = 0;
int i = 0;

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
char customKey;  // changed(added)
//HX711 cell;

// Define the number of steps per revolution for your motor
int stepsPerRevolution = 2400;  // Change this value based on your motor specifications
const int relayPin = 30;
// Initialize the stepper motor object with the specified pins
Stepper myStepper(stepsPerRevolution, 17, 18, 19, 20);

// Flag variable to track if the action has been performed
bool actionPerformed = false;

float inputweight = 0;

int ultrasonic1 = 27;
int ultrasonic2 = 28;

int irSensorpin = 40;
int sensorvalue = 0;


void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  cell.begin(22, 23);  //load cell
  cell2.begin(24, 25);
  Serial.begin(9600);
  myservo1.attach(14);
  myservo2.attach(16);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  // Set the speed of the stepper motor in RPM (revolutions per minute)
  myStepper.setSpeed(15);  // Set stepper motor speed to 60 RPM
  pinMode(ultrasonic1, INPUT);
  pinMode(ultrasonic2, INPUT);
  pinMode(irSensorpin, INPUT);





  // Wait for the sensor to settle
  delay(1000);
}

void loop() {
  customKey = customKeypad.getKey();  // changed
  weight = "";
  if (customKey) {
    Serial.println(customKey);
    int inputultrasonic1 = digitalRead(ultrasonic1);
    int inputultrasonic2 = digitalRead(ultrasonic2);


    switch (int(customKey)) {
      case 49:
        Serial.println(inputultrasonic1);
        if (inputultrasonic1 == LOW) {  // low for adequate goods

          //Serial.println(inputultrasonic1);
          Serial.println(inputultrasonic1);

          display("White rice");
          delay(3000);
          display("Press enter");
          inputweight = input_weight();
          display(String(inputweight));
          Serial.println(inputweight);
          if (inputweight == 0) { break; }
          delay(3000);
          while (true) {
            if (digitalRead(irSensorpin) == LOW) {
              //lcd.clear();
              delay(1000);
              motoron(IN1, IN2, ENA, myservo1);
              while (inputweight > reading()) {}
              motoroff(IN1, IN2, ENA, myservo1);
              sealer();
              lcd.clear();
              break;
            } else {
              display("place a bag");
              delay(100);
            }
          }
        } else {
          display("White rice level too low");
          Serial.println("white rice too low");
          delay(3000);
          lcd.clear();
          break;
        }
        break;



      case 50:
      Serial.println(inputultrasonic2);
        if (inputultrasonic2 == LOW) {  // low for adequate goods

          //Serial.println(inputultrasonic1);
          Serial.println(inputultrasonic2);

          display("Red rice");
          delay(3000);
          display("Press enter");
          inputweight = input_weight();
          display(String(inputweight));
          Serial.println(inputweight);
          if (inputweight == 0) { break; }
          delay(3000);
          while (true) {
            if (digitalRead(irSensorpin) == LOW) {
              lcd.clear();
              delay(1000);
              motoron(IN3, IN4, ENB, myservo2);
              while (inputweight > reading()) {}
              motoroff(IN1, IN2, ENA, myservo1);
              sealer();
              lcd.clear();
              break;
            } else {
              display("place a bag");
              delay(100);
            }
          }
        } else {
          display("Red rice level too low");
          Serial.println("red rice too low");
          delay(3000);
          lcd.clear();
          break;
        }
        
        break;
        
      default:
        display("Invalid Input");
        delay(7000);
        lcd.clear();
        break;
    }
  }
}


//function definition
//motor on function
void motoron(int IN_volt1, int IN_volt2, int Enable, Servo name) {
  digitalWrite(IN_volt1, HIGH);
  digitalWrite(IN_volt2, LOW);
  analogWrite(Enable, 255);
  name.write(180);
}

//motor off function
void motoroff(int IN_volt1, int IN_volt2, int Enable, Servo name) {
  digitalWrite(IN_volt1, LOW);
  digitalWrite(IN_volt2, LOW);
  analogWrite(Enable, 0);
  name.write(95);
}

//display function
void display(String text) {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print(text);
}

//sealing function
/*void sealer() {
  actionPerformed = false;
  myStepper.step(stepsPerRevolution);  // Perform the action (rotate the stepper motor one full revolution clockwise)
  delay(1000);                         // Add a delay here if needed to control the timing between steps
  actionPerformed = true;              // Set the flag to indicate that the action has been performed
  delay(100);
  digitalWrite(relayPin, LOW);
  delay(1000);
  digitalWrite(relayPin, HIGH);
  delay(2000);
  //stepsPerRevolution = -stepsPerRevolution ;
  myStepper.step(-stepsPerRevolution);
}*/

//get the load cell reading function
float reading() {
  val = cell.read();
  val2 = cell2.read();
  w2 = (val2 + 133448) / 232231 * 100;  //calibration
  w = (val - 106125) / 208834 * 100;
  display(String(w + w2));
  Serial.println(w + w2);
  return (w + w2);
}

//get user input weight and display weight function
//char customKey = customKeypad.getKey(); // defiened twice here globally and locally in loop bad practice and probably the issue
float input_weight() {
  while (true) {
    customKey = customKeypad.getKey();
    if (customKey == 42) {
      Serial.println("cancelled");
      display("cancelled");
      weightf = 0;
      break;
    } else if (customKey == 35) {
      Serial.println("chosen");
      break;
    }
  }

  if (customKey == 35) {
    while (true) {
      customKey = customKeypad.getKey();
      if (customKey == 35) {
        break;
      } else if (customKey == 42) {
        weight.remove(weight.length() - 1);
      } else if (customKey) {
        weight += String(customKey);
      }
      weightf = weight.toFloat();
    }
    Serial.println(weight);
    display(String(weightf) + "g");
  }
  return weightf;
}
