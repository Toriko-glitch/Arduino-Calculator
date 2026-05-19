// Course: Introduction to IoT
// Description: PBL Arduino Calculator


// Libraries
#include <LiquidCrystal.h> // for the LCD
#include <Keypad.h> // for the keypad

// Define the LCD pin connections: RS, Enable, Data4, Data5, Data6, Data7
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = A0, d7 = A1;
// Create an LCD object
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Stores 4 columns and 4 rows, 
// "const" means that the value will never change 
const int ROWS = 4; // Four rows on the keypad
const int COLS = 4; // Four columns on the keypad

// The Keypad uses a 2D array to map out the keypad layout
// An array is a grid system of rows and columns that can store characters at each grid
// Note: arrays in C start at an index 0, so row 0, row 1, roow 2, etc..
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' }, // each set of curly braces is a row
  { '4', '5', '6', 'B' }, // each value stored is one column, here there are 4 columns
  { '7', '8', '9', 'C' }, // The stored values match exactly the keypad layout
  { '*', '0', '#', 'D' }
};

// Connecting the digital pins to the row pins and column pins
// THis is a 1D array, byte because the library expects byte the way it was written
byte rowPins[ROWS] = { 9, 8, 7, 6 };  // This sets the row 0 to D9, row 1 to D8, etc.
byte colPins[COLS] = { 5, 4, 3, 2 };  // This sets column 0 to D5, Column 1 to D4, etc.

// Create an object of keypad
// Note: makeKeymap(keys) cnverts the 2D array into a format the library understands
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
// note: how it works with the library -> when you press a button on the keypad
// it sets that row to LOW, all other rows are HIGH and then reads the column pins
// The pressed column will also be LOW, then the library will look up the stored char in keys[ROWS][COLS]

// global variables to be used in functions and calculations, initialized at 0
// float since there is division operation on the calculator
float Num1 = 0; // Num1 is the first digit pressed
float Num2 = 0; // Num2 is the second digit pressed
float result = 0; //the result based on the operation selected
char operation = ' '; // empty space initialized, meaning no operations 
bool waitforNum2 = false; // note: false means digits go to Num1 and true (after pressing an operator) means digits go Num2
// It will make the program understand if typing Num1 or Num2
String displayLine = ""; // The string that will be displayed on the lcd, LCD displays ASCII


void setup() {
  // put your setup code here, to run once:
  // Libraries automatically handles the pinMode INPUT and OUTPUT internally 
  lcd.begin(16, 2); // Tells lcd 16 columns and 2 rows for the display
  showWelcome(); // Display a welcome message, calls the showWelcome(); function 
  Serial.begin(9600); // for serial monitor
}

void loop() {
  // put your main code here, to run repeatedly:

  char key = keypad.getKey();  // Reads the keypad and stores in variable key

if (key) {
    Serial.print("Key pressed: "); 
    Serial.println(key); // shows on serial monitor

    // '*' key is for clearing the calculator
    if (key == '*') {
      resetCalculator(); // call function 

    } else if (key == '#') { // '#' is the equal operation
        computeResult(); // call function 

    } else if (key == 'A' || key == 'B' || key == 'C' || key == 'D') { // The Letter keys will be for different operations like + - * / 
        handleOperator(key); // call function
  
    } else if (key >= '0' && key <= '9') { 
        handleDigit(key); // call function
        // Since the keypad returns characters, gotta use this function to convert to integers
        // example number 5 is 53, and 0 is 48 in ASCII so '5' - '0' = 5 
    }
  }
}

// Function to clear
void resetCalculator() {
  //resets all variables to starting value
  Num1  = 0;
  Num2 = 0;
  operation = ' ';
  waitforNum2 = false;
  displayLine = "";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Cleared!");
  delay(600);
  lcd.clear();
  lcd.setCursor(0, 0); //reset cursor
  lcd.print("Enter number:"); 
}

// Function to get result of selected operation
void computeResult() {
  // float since there is division, initialize result to zero
  float result = 0;

  // the different equations per operation
  if (operation == '+') {
    result = Num1 + Num2;
  } else if (operation == '-') {
    result = Num1 - Num2;
  } else if (operation == '*') {
    result = Num1 * Num2;
  } else if (operation == '/') {
    // division (reset if division by 0)
    if (Num2 == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Error: Div by 0");
      delay(2000);
      resetCalculator();
      return;
    }
    result = Num1 / Num2;
  }

  // Show the full expression on line 1
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(displayLine + "=");

  // Show the result on line 2
  lcd.setCursor(0, 1);
  lcd.print("= ");
  lcd.print(result, 2); // 2 decimal places

  // Prepare for a new calculation using the result
  Num1 = result; // this allows chain calculations
  Num2 = 0;
  operation = ' ';
  waitforNum2 = false;
  displayLine = String(result, 2); // converts the float to in text
}

// Function to map letter keys to operation symbols
void handleOperator(char key) {

  if (key == 'A') {
    operation = '+';
  } else if (key == 'B') {
    operation = '-';
  } else if (key == 'C') {
    operation = '*';
  } else if (key == 'D') {
    operation = '/';
  }

  waitforNum2 = true;  // Next digits go to Num2
  displayLine += operation; // Append operator to the display string

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(displayLine);
  // need these three lines everytime, becasue lcd needs to refresh whenever a new input
  //it clears the screen, brings back the cursor, and prints the new updated string
}

// 
void handleDigit(char key) {
  int digit = key - '0';  // Convert char '0'-'9' to int 0-9

  if (waitforNum2) {
    // Building the second operand
    Num2 = Num2 * 10 + digit;
  } else {
    // Building the first operand
    Num1 = Num1 * 10 + digit;
    // example: key pressed = '2', digit = 2, Num1 = 0, 0 * 10 + 2 = 2
    // lets say you press a second key  '4', digit = 4, num1 = 2, 2 * 10 + 4 = 24
  }

  displayLine += key;  // Append digit character to the display string
  // the += operator appends to the end of displayLine to build the string shown on the lcd

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(displayLine);
  // need these three lines everytime, becasue lcd needs to refresh whenever a new input
  // it clears the screen, brings back the cursor, and prints the new updated string
}

void showWelcome() {
  lcd.clear(); // clears the screen
  lcd.setCursor(0, 0); // like placing a cursor where to type, so columns (0 to 15) rows (0 to 1)
  // column 0, row 0
  lcd.print("Calculator"); // display calculator
  lcd.setCursor(0, 1); // cursor at column 0, row 1 (second line)
  lcd.print("Loading...");
  delay(2000); // display for 2 sec before clearing
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter number:");
}


