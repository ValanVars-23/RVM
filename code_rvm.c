#include <Adafruit_LiquidCrystal.h>
#include <Keypad.h>

#define trigPin1 11   // Trigger pin for ultrasonic sensor 1 (5 cm)
#define echoPin1 12  // Echo pin for ultrasonic sensor 1
#define trigPin2 9  // Trigger pin for ultrasonic sensor 2 (30 cm)
#define echoPin2 10  // Echo pin for ultrasonic sensor 2

const int buttonPin = 13;  // Pin where your button is connected
int buttonState = 0;
int lastButtonState = 0;
int buttonPressCount = 0;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {7, 6, 5, 4}; // Row pinouts of the keypad
byte colPins[COLS] = {3, 2, 1, 0}; // Column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Adafruit_LiquidCrystal lcd_1(0); // Initialize the LCD

String numberInput; // Declare numberInput as a global variable

// Function to measure distance from an ultrasonic sensor
long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);      // Measure the time for the sound wave to return
  long distance = duration * 0.034 / 2;        // Calculate distance in cm
  return distance;
}

// Function to get a 10-digit number from the keypad
String get10DigitNumber() {
  String input = "";
  lcd_1.clear();
  lcd_1.print("Enter 10 Digits:");
  while (input.length() < 10) {
    char key = keypad.getKey();
    if (key) {
      input += key;
      lcd_1.setCursor(0, 1);
      lcd_1.print(input);
      delay(200); // Debounce delay
    }
  }
  return input;
}

void setup() {
  lcd_1.begin(16, 2);    // Initialize the LCD
  lcd_1.setBacklight(1); // Turn on the backlight

  // Initialize ultrasonic sensor pins
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  
  pinMode(buttonPin, INPUT);

  // Display initial message
  lcd_1.clear();
  lcd_1.print("SMART RECYCLING");
  lcd_1.setCursor(0, 1);
  lcd_1.print("STATION");
  delay(3000); // Show for 3 seconds
}

void loop() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH && lastButtonState == LOW) {
    buttonPressCount++;

    // STAGE ONE: Ask for user's phone number
    if (buttonPressCount == 1) {
      lcd_1.clear();
      lcd_1.print("Give your number:");
      delay(1000);
      numberInput = get10DigitNumber(); // Store input in global variable
      delay(3000); // Simulate waiting for user input
    }

    // STAGE TWO: Ask the user to place the bottle
    else if (buttonPressCount == 2) {
      lcd_1.clear();
      lcd_1.print("Place the bottle");
      delay(3000); // Wait for the user to place the bottle
    } 
   
    // STAGE THREE: Detect and categorize the bottle
    else if (buttonPressCount == 3) { 
      lcd_1.clear();
      lcd_1.print("Detecting...");

      // Measure distance from both ultrasonic sensors
      long distance1 = measureDistance(trigPin1, echoPin1);  // Distance from sensor 1
      long distance2 = measureDistance(trigPin2, echoPin2);  // Distance from sensor 2

      lcd_1.clear();
      if (distance1 <= 10) 
      {
        if (distance2 <= 10) 
        {
          lcd_1.print("Category 2 Bottle");
        } else {
          lcd_1.print("Category 1 Bottle");
        }
      } else {
        lcd_1.print("No Bottle Detected");
      }

      // Display the entered number on the second row
      lcd_1.setCursor(0, 1);
      lcd_1.print("PHONE:");
      lcd_1.setCursor(6, 1); // Show the actual number input
      lcd_1.print(numberInput);           
      Serial.println("Bottle categorization done.");
      delay(3000); // Wait before resetting
      lcd_1.clear();
      lcd_1.print("Thank you");
      delay(3000);
      lcd_1.clear();
      lcd_1.print("SMART RECYCLING");
      lcd_1.setCursor(0, 1);
      lcd_1.print("STATION");
      
      buttonPressCount = 0; // Reset button press count
    }
  }

  lastButtonState = buttonState;
}
