#include <LiquidCrystal.h>
#include <Keypad.h>

//------------------------------------------------------------------------------------------
// Configuration variables
//------------------------------------------------------------------------------------------
// LCD Definition
#define LCD_LINE_LENGTH 16
#define LCD_NB_LINES 2

// Secret code to trigger the configuration mode
const String CONFIG_CODE = "1337";

// Acceptable code lengths
#define minCodeLength 1
#define maxCodeLength LCD_LINE_LENGTH
int prefCodeLength = minCodeLength;

// Game mode
#define ANTI_TERRO_A 1
#define ANTI_TERRO_D 2
#define CAPTURE 3
#define DEFUSE 4
int gameMode = ANTI_TERRO_A;

//------------------------------------------------------------------------------------------
// LCD Configuration and methods
//------------------------------------------------------------------------------------------

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel
 
// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
 
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
 
int readLCDButtons(){               // read the buttons
    adc_key_in = analogRead(0);       // read the value from the sensor 
 
    // my buttons when read are centered at these values: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    // We make this the 1st option for speed reasons since it will be the most likely result
 
    if (adc_key_in > 1000) return btnNONE; 
 
    // For V1.1 us this threshold
    if (adc_key_in < 50)   return btnRIGHT;  
    if (adc_key_in < 250)  return btnUP; 
    if (adc_key_in < 450)  return btnDOWN; 
    if (adc_key_in < 650)  return btnLEFT; 
    if (adc_key_in < 850)  return btnSELECT;
 
    return btnNONE;                // when all others fail, return this.
}

//------------------------------------------------------------------------------------------
// Bluetooth Configuration and methods
//------------------------------------------------------------------------------------------
String command = "";

//------------------------------------------------------------------------------------------
// Key Configuration and methods
//------------------------------------------------------------------------------------------
// set key pin:
#define KEY_PIN 15

int armedKeyState;

int readKeyState ()
{
  return digitalRead(KEY_PIN);
}

//------------------------------------------------------------------------------------------
// Pad Configuration and methods
//------------------------------------------------------------------------------------------
const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]=
{
{'1', '2', '3', 'A'},
{'4', '5', '6', 'B'},
{'7', '8', '9', 'C'},
{'*', '0', '#', 'D'}
};

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {13,12,11,3}; //Rows 0 to 3
byte colPins[numCols]= {16,17,18,19}; //Columns 0 to 3

//initializes an instance of the Keypad class
Keypad myKeypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols);

char readPad ()
{
  char keyPressed;
  while ((keyPressed = myKeypad.getKey()) == NO_KEY){} // As long as the read key is nothing, read again.
  return keyPressed; // Return the pressed char
}

String readCode (int expectedLength = 0)
{
  if (expectedLength == 0) expectedLength = maxCodeLength;
  
  String theCode = "";
  char keyPressed;
  int charCounter = 0;
  
  while((keyPressed = readPad()) != 'D' && charCounter < expectedLength)
  {
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    if (keyPressed == 'C')
    {
      charCounter--;
      theCode.remove(charCounter);
      lcd.print(theCode);
    }
    else
    {
      theCode += keyPressed;
      lcd.print(theCode);
      charCounter++;
    }
  }
  return theCode;
}

//------------------------------------------------------------------------------------------
// Displays
//------------------------------------------------------------------------------------------
#define ENTER_CONFIG_CODE_DISPLAY 0
#define MANUAL_OR_BLUETOOTH_CONFIG_DISPLAY 1
#define BLUETOOTH_CONFIG_DISPLAY 2
#define GAME_MODE_CONFIG_DISPLAY 3
#define GAME_DURATION_CONFIG_DISPLAY 4
#define TIME_FOR_DEFUSING_CONFIG_DISPLAY 5
#define KEY_CODE_CONFIG_DISPLAY 6
#define CODE_LENGTH_CONFIG_DISPLAY 7
#define CODE_DEFINITION_CONFIG_DISPLAY 8
#define READY_TO_START_DISPLAY 9
#define WAITING_FOR_CODE_DISPLAY 10
#define WAITING_FOR_KEY_DISPLAY 11
#define WAITING_FOR_BOTH_DISPLAY 12
#define BOX_OPENED_DISPLAY 13
#define END_OF_GAME_DISPLAY 14

// Errors
#define WRONG_CODE_LENGTH_DISPLAY 101
#define WRONG_CODE_DISPLAY 102
#define WRONG_DURATION_VALUE_DISPLAY 103

// Variable
int currentDisplay = ENTER_CONFIG_CODE_DISPLAY;
int previousDisplay = currentDisplay;

//----------------------------- Config Code Display ----------------------------------------
int enterConfigCodeDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Config code :");

  if (readCode() == CONFIG_CODE)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

//----------------------------- Config Manual/Bluetooth ------------------------------------
int manualOrBluetoothDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Manual (1)");
  lcd.setCursor(0,1);
  lcd.print("Bluetooth (2)");

  char keyPressed;
  while (true)
  {
    keyPressed = readPad();
    if (keyPressed == '1') return 1;
    else if (keyPressed == '2') return 2;
  }
}

//----------------------------- Bluetooth config Display -----------------------------------
int bluetoothConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Waiting for");
  lcd.setCursor(0,1);
  lcd.print("configuration...");
  
  while (true)
  {
    if (Serial.available())  {
      command = Serial.readString();
      if (command == "VALID_CONFIG") return 1;
      else if (command == "CANCEL_CONFIG") return 0;
    }
  }
}

//----------------------------- Game mode config Display -----------------------------------
int gameModeConfigDisplay ()
{
  int scroll = 0;
  long int currentTime = millis()/1000;
  long int previousTime = currentTime;
  char keyPressed;

  // Init display
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("(1) Anti/Terro A");
  lcd.setCursor(0,1);
  lcd.print("(2) Anti/Terro D");
  scroll++;

  while (true)
  {
    keyPressed = myKeypad.getKey();

    // Depending on the key pressed, choose mode
    switch (keyPressed)
    {
      case '1' :
        gameMode = ANTI_TERRO_A;
        return gameMode;
      case '2' :
        gameMode = ANTI_TERRO_D;
        return gameMode;
      case '3' :
        gameMode = CAPTURE;
        return gameMode;
      case '4' :
        gameMode = DEFUSE;
        return gameMode;
    }

    currentTime = millis()/1000;
    
    // Every second, scroll the display
    if (currentTime - previousTime >= 2)
    {
      previousTime = currentTime;
      switch (scroll)
      {
        case 0 :
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("(1) Anti/Terro A");
          lcd.setCursor(0,1);
          lcd.print("(2) Anti/Terro D");
          scroll++;
          break;
        case 1 :
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("(3) Capture");
          lcd.setCursor(0,1);
          lcd.print("(4) Defuse");
          scroll = 0;
          break;
      }
    }
  }
}

//----------------------------- Game duration config Display -------------------------------

//----------------------------- Time for defusing config Display ---------------------------

//----------------------------- Key/Code config Display ------------------------------------

//----------------------------- Code length config Display ---------------------------------

//----------------------------- Code definition config Display -----------------------------

//----------------------------- Ready to start Display -------------------------------------

//----------------------------- Waiting for code Display -----------------------------------

//----------------------------- Waiting for key Display ------------------------------------

//----------------------------- Waiting for both Display -----------------------------------

//----------------------------- Box opened ! Display ---------------------------------------

//----------------------------- End of game Display ----------------------------------------

//----------------------------- Wrong code length Display ----------------------------------
void wrongCodeLengthDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Must have :");
  lcd.setCursor(0,1);
  lcd.print(minCodeLength);
  lcd.print(" < length < ");
  lcd.print(maxCodeLength);
  delay (1000);
}

//----------------------------- Wrong code Display -----------------------------------------
void wrongCodeDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Wrong code !");
  delay (1000);
}

//----------------------------- Wrong duration value Display -------------------------------
void wrongDurationValueDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("0 < minutes < 60");
  lcd.setCursor(0,1);
  lcd.print("0 < seconds < 60");
  delay (1000);
}

void setup(){
   Serial.begin(115200);  //initial the Serial (for Bluetooth connection)
   
   pinMode(KEY_PIN, INPUT); // Set the key pin as input
   
   lcd.begin(LCD_LINE_LENGTH, LCD_NB_LINES);               // start the library
   lcd.setCursor(0,0);             // set the LCD cursor   position 
   lcd.print("Push the buttons");  // print a simple message on the LCD
}


int theBuffer;

void loop(){
  // Read the strings that are sent via Bluetooth
  //if (Serial.available())  {
  // command = Serial.readString();
  //}
  
  switch (currentDisplay)
  {
    // General cases
    case ENTER_CONFIG_CODE_DISPLAY :
      theBuffer = enterConfigCodeDisplay();
      if (theBuffer == 1) currentDisplay = MANUAL_OR_BLUETOOTH_CONFIG_DISPLAY;
      else
      {
        previousDisplay = currentDisplay;
        currentDisplay = WRONG_CODE_DISPLAY;
      }
      break;
    case MANUAL_OR_BLUETOOTH_CONFIG_DISPLAY :
      theBuffer = manualOrBluetoothDisplay();
      if (theBuffer == 1) currentDisplay = GAME_MODE_CONFIG_DISPLAY;
      else if (theBuffer == 2) currentDisplay = BLUETOOTH_CONFIG_DISPLAY;
      break;

    case BLUETOOTH_CONFIG_DISPLAY :
      theBuffer = bluetoothConfigDisplay();
      if (theBuffer == 1) currentDisplay = READY_TO_START_DISPLAY;
      else currentDisplay = MANUAL_OR_BLUETOOTH_CONFIG_DISPLAY;
      break;

    case GAME_MODE_CONFIG_DISPLAY :
      theBuffer = gameModeConfigDisplay();
      currentDisplay = ENTER_CONFIG_CODE_DISPLAY; // TODO Write transitions
      break;
    
    case GAME_DURATION_CONFIG_DISPLAY :
      break;
        
    case TIME_FOR_DEFUSING_CONFIG_DISPLAY :
      break;
    
    case KEY_CODE_CONFIG_DISPLAY :
      break;
    
    case CODE_LENGTH_CONFIG_DISPLAY :
      break;
    
    case CODE_DEFINITION_CONFIG_DISPLAY :
      break;
    
    case READY_TO_START_DISPLAY :
      break;
    
    case WAITING_FOR_CODE_DISPLAY :
      break;
    
    case WAITING_FOR_KEY_DISPLAY :
      break;
    
    case WAITING_FOR_BOTH_DISPLAY :
      break;
    
    case BOX_OPENED_DISPLAY :
      break;
    
    case END_OF_GAME_DISPLAY :
      break;
          
    // Error cases
    case WRONG_CODE_LENGTH_DISPLAY :
      wrongCodeLengthDisplay();
      currentDisplay = previousDisplay;
      break;

    case WRONG_CODE_DISPLAY :
      wrongCodeDisplay();
      currentDisplay = previousDisplay;
      break;
      
    case WRONG_DURATION_VALUE_DISPLAY :
      wrongDurationValueDisplay();
      currentDisplay = previousDisplay;
      break;
  }
}
