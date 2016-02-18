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
#define MIN_CODE_LENGTH 1
#define MAX_CODE_LENGTH LCD_LINE_LENGTH
#define NB_DIGITS_CODE_LENGTH 2
int prefCodeLength = MIN_CODE_LENGTH;

// Game modes
const int ANTI_TERRO_A = (int) '1';
const int ANTI_TERRO_D = (int) '2';
const int CAPTURE = (int) '3';
const int DEFUSE = (int) '4';
int gameMode = ANTI_TERRO_A;

// Game duration
#define MIN_GAME_DURATION 5
#define DURATION_DIGITS 2
int gameDuration = MIN_GAME_DURATION;

// Key/Code choice
const int KEY_ONLY = (int) '0';
const int CODE_ONLY = (int) '1';
const int KEY_OR_CODE = (int) '2';
const int KEY_AND_CODE = (int) '3';
int keyOrCode = KEY_OR_CODE;

// Game code
String gameCode = "";

// Time for defusing
#define MIN_DEFUSE_TIME 1
int defuseTime = MIN_DEFUSE_TIME;

// Teams
#define MAX_NB_TEAMS 8
#define MIN_NB_TEAMS 2
#define NB_TEAMS_DIGITS 1
int nbOfTeams = MIN_NB_TEAMS;

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

bool armedKeyState;

bool isKeyDefused ()
{
  return (digitalRead(KEY_PIN) != armedKeyState);
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
  if (expectedLength == 0) expectedLength = MAX_CODE_LENGTH;
  
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

int readNumber (int nbDigits, int minValue, int maxValue)
{
  String numberString = "";
  int number;
  // Initial display
  lcd.setCursor(0,1);
  lcd.print(numberString);
  
  char keyPressed;

  int charCounter = 0;
    
  while(true)
  {
    keyPressed = myKeypad.getKey();
    
    if (keyPressed >= '0' && keyPressed <= '9' && charCounter < nbDigits)
    {
      Serial.println(keyPressed);
      numberString += keyPressed;
      charCounter++;
      
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(numberString);
    }
    if (keyPressed == 'C' && charCounter > 0)
    {
      charCounter--;
      numberString.remove(charCounter);      
      
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(numberString);
    }
    if (keyPressed == 'D' && charCounter > 0)
    {
      number = numberString.toInt();
      if (number >= minValue && number <= maxValue)
      {
        return number;
      }
      else
      {
        numberString = "";
        charCounter = 0;
        lcd.setCursor(0,1);
        lcd.print("Invalid value   ");
        delay(1000);
        lcd.setCursor(0,1);
        lcd.print("                ");
      }
    }
  }
}

int readDuration (int minDuration)
{
  String minutes = String(minDuration);
  int duration = minDuration;
  // Initial display
  lcd.setCursor(0,1);
  lcd.print(minutes);
  lcd.print(" minutes");
  
  char keyPressed;

  int charCounter = 0;
    
  while(true)
  {
    keyPressed = myKeypad.getKey();
    
    if (keyPressed >= '0' && keyPressed <= '9' && charCounter < DURATION_DIGITS)
    {
      Serial.println(keyPressed);
      if (charCounter == 0) minutes = keyPressed;
      if (charCounter == 1) minutes += keyPressed;
      charCounter++;
      
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(minutes);
      lcd.print(" minutes");
    }
    if (keyPressed == 'C')
    {
      minutes = String(minDuration);
      charCounter = 0;
      
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(minutes);
      lcd.print(" minutes");
    }
    if (keyPressed == 'D')
    {
      duration = minutes.toInt();
      if (duration >= minDuration)
      {
        return duration;
      }
      else
      {
        minutes = String(minDuration);
        charCounter = 0;
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print(minutes);
        lcd.print(" minutes");
      }
    }
  }
}

//------------------------------------------------------------------------------------------
// Displays
//------------------------------------------------------------------------------------------
// Config displays
#define ENTER_CODE_CONFIG_DISPLAY 0//
#define MANUAL_OR_BLUETOOTH__CONFIG_DISPLAY 1//
#define WAITING_BLUETOOTH_CONFIG_DISPLAY 2//
#define GAME_MODE_CONFIG_DISPLAY 3//
#define GAME_DURATION_CONFIG_DISPLAY 4//
#define KEY_CODE_CONFIG_DISPLAY 5//
#define CODE_LENGTH_CONFIG_DISPLAY 6//
#define CODE_DEFINITION_CONFIG_DISPLAY 7//
#define TIME_FOR_DEFUSING_CONFIG_DISPLAY 8//
#define NUMBER_OF_TEAMS_CONFIG_DISPLAY 9//
#define RANDOM_CODE_GENERATION_DISPLAY 10//

// Begin/end diplays
#define READY_TO_START_DISPLAY 100//
#define END_OF_GAME_DISPLAY 101

// Ingame displays
#define WAITING_FOR_CODE_INGAME_DISPLAY 200
#define WAITING_FOR_KEY_INGAME_DISPLAY 201
#define WAITING_FOR_EITHER_INGAME_DISPLAY 202
#define WAITING_FOR_TEAM_NUMBER_INGAME_DISPLAY 203
#define WAITING_FOR_CODE_ACTIVATION_INGAME_DISPLAY 204
#define WAITING_FOR_KEY_ACTIVATION_INGAME_DISPLAY 205
#define WAITING_FOR_EITHER_ACTIVATION_INGAME_DISPLAY 206

// Errors
#define CODE_LENGTH_ERROR_DISPLAY 300//
#define CODE_ERROR_DISPLAY 301//
#define BOX_OPENED_ERROR_DISPLAY 302

// Variables to keep track of what is being displayed
int currentDisplay = ENTER_CODE_CONFIG_DISPLAY;
int previousDisplay = currentDisplay;

//------------------------------------------------------------------------------------------
// Config Displays Methods and Functions
//------------------------------------------------------------------------------------------
/**
 * Enter Code Config Display
 */
int enterCodeConfigDisplay ()
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

/**
 * Manual Or Bluetooth Config DisplayWAITING_BLUETOOTH_CONFIG_DISPLAY
 */
int manualOrBluetoothConfigDisplay ()
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

/** 
 * Bluetooth Config Display
 */
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

/**
 * Game Mode Config Display
 */
void gameModeConfigDisplay ()
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
        return;
      case '2' :
        gameMode = ANTI_TERRO_D;
        return;
      case '3' :
        gameMode = CAPTURE;
        return;
      case '4' :
        gameMode = DEFUSE;
        return;
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

/** 
 * Game Duration Config Display
 */
void gameDurationConfigDisplay()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Game duration :");
  gameDuration = readDuration(MIN_GAME_DURATION);
}

/**
 * Key Code Config Display
 */
void keyCodeConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("(1) Key|(2) Code");
  lcd.setCursor(0,1);
  lcd.print("(3) OR |(4) AND");

  int keyPressed;

  while (true)
  {
    keyPressed = (int) myKeypad.getKey();

    // Depending on the key pressed, choose mode
    switch (keyPressed)
    {
      case KEY_ONLY :
        keyOrCode = KEY_ONLY;
        return;
      case CODE_ONLY :
        keyOrCode = CODE_ONLY;
        return;
      case KEY_OR_CODE :
        keyOrCode = KEY_OR_CODE;
        return;
      case KEY_AND_CODE :
        keyOrCode = KEY_AND_CODE;
        return;
    }
  }
}
/**
 * Code Length Config Display
 */
void codeLengthConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Code length :");
  prefCodeLength = readNumber(NB_DIGITS_CODE_LENGTH, MIN_CODE_LENGTH, MAX_CODE_LENGTH);
}

/**
 * Code Definition Config Display
 */
void codeDefinitionConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter the Code :");
  gameCode = readCode();
}

/**
 * Time For Defusing Config Display
 */
void timeForDefusingConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Defuse time :");
  defuseTime = readDuration(MIN_DEFUSE_TIME);
}

/**
 * Number Of Teams Config Display
 */
void numberOfTeamsConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("How many teams :");
  nbOfTeams = readNumber(NB_TEAMS_DIGITS, MIN_NB_TEAMS, MAX_NB_TEAMS);
}

/**
 * Random Code Generation Display
 */
void randomCodeGenerationDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Please unplug A1,");
  lcd.setCursor(0,1);
  lcd.print("then press D !");

  while(myKeypad.getKey() != 'D'){}

  // Initialize the random seed (must debranch A1 before startin the system!)
  randomSeed(analogRead(1));

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Please replug A1,");
  lcd.setCursor(0,1);
  lcd.print("then press D !");

  while(myKeypad.getKey() != 'D'){}
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Code is :");
  lcd.setCursor(0,1);
  
  gameCode = "";
  for (int i = 0; i < prefCodeLength; i++)
  {
    // Add a random number from 0 to 9 to the code string
    gameCode += (char) random(48, 58);
  }

  lcd.print(gameCode);

  while(myKeypad.getKey() != 'D'){}
}

//------------------------------------------------------------------------------------------
// Begin/End Displays Methods and Functions
//------------------------------------------------------------------------------------------
/** 
 * Ready To Start Display
 */
void readyToStartDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ready ! Press");
  lcd.setCursor(0,1);
  lcd.print("D to Start !");
  
  while(myKeypad.getKey() != 'D'){}
}

//------------------------------------------------------------------------------------------
// Ingame Displays Methods and Functions
//------------------------------------------------------------------------------------------
/** 
 * Waiting for code ingame display
 */
int waitingForCodeIngameDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter code :");

  if (readCode() == gameCode)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
 * Waiting for key ingame display
 */
int waitingForKeyIngameDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("To turn off,");
  lcd.setCursor(0,1);
  lcd.print("turn the key !");

  if (readCode() == gameCode)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
 * Waiting for either ingame display
 */

/**
 * Waiting for team number ingame display
 */

/**
 * Waiting for code activation ingame display
 */

/**
 * Waiting for key activation ingame display
 */

/**
 * Waiting for either activation ingame display
 */

//------------------------------------------------------------------------------------------
// Error Displays Methods and Functions
//------------------------------------------------------------------------------------------

/** 
 * Code Length Error Display
 */
void codeLengthErrorDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Must have :");
  lcd.setCursor(0,1);
  lcd.print(MIN_CODE_LENGTH);
  lcd.print(" < length < ");
  lcd.print(MAX_CODE_LENGTH);
  delay (1000);
}

/**
 * Code Error Display
 */
void codeErrorDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Wrong code !");
  delay (1000);
}

//------------------------------------------------------------------------------------------
// Config Methods
//------------------------------------------------------------------------------------------
void antiTerroAConfig ()
{
  
}

void antiTerroDConfig ()
{
  
}

void captureConfig ()
{
  
}

void defuseConfig ()
{
  
}

void specificConfig ()
{
  switch (gameMode)
  {
    case ANTI_TERRO_A :
      antiTerroAConfig();
      break;
    case ANTI_TERRO_D :
      antiTerroDConfig();
      break;
    case CAPTURE :
      captureConfig();
      break;
    case DEFUSE :
      defuseConfig();
      break;
  }
}

void commonConfig ()
{
  int displayReturnValue;
  
  while (true)
  {
      switch (currentDisplay)
      {
        // General cases
        case ENTER_CODE_CONFIG_DISPLAY :
          displayReturnValue = enterCodeConfigDisplay();
          if (displayReturnValue == 1) currentDisplay = MANUAL_OR_BLUETOOTH__CONFIG_DISPLAY;
          else
          {
            previousDisplay = currentDisplay;
            currentDisplay = CODE_ERROR_DISPLAY;
          }
          break;
          
        case MANUAL_OR_BLUETOOTH__CONFIG_DISPLAY :
          displayReturnValue = manualOrBluetoothConfigDisplay();
          if (displayReturnValue == 1) currentDisplay = GAME_MODE_CONFIG_DISPLAY;
          else if (displayReturnValue == 2) currentDisplay = WAITING_BLUETOOTH_CONFIG_DISPLAY;
          break;
    
        case WAITING_BLUETOOTH_CONFIG_DISPLAY :
          displayReturnValue = bluetoothConfigDisplay();
          if (displayReturnValue == 1) currentDisplay = READY_TO_START_DISPLAY;
          else currentDisplay = MANUAL_OR_BLUETOOTH__CONFIG_DISPLAY;
          break;
    
        case GAME_MODE_CONFIG_DISPLAY :
          gameModeConfigDisplay();
          currentDisplay = GAME_DURATION_CONFIG_DISPLAY;
          break;
        
        case GAME_DURATION_CONFIG_DISPLAY :
          gameDurationConfigDisplay();
          currentDisplay = KEY_CODE_CONFIG_DISPLAY;
          break;

        case KEY_CODE_CONFIG_DISPLAY :
          keyCodeConfigDisplay();
          if (keyOrCode > 0) currentDisplay = CODE_LENGTH_CONFIG_DISPLAY;
          else
          {
            specificConfig();
            currentDisplay = READY_TO_START_DISPLAY;
          }
          break;

        case CODE_LENGTH_CONFIG_DISPLAY :
          codeLengthConfigDisplay();
          specificConfig();
          currentDisplay = READY_TO_START_DISPLAY;
          break;

        // Begin game
        case READY_TO_START_DISPLAY :
          readyToStartDisplay();
          currentDisplay = ENTER_CODE_CONFIG_DISPLAY;
          return;
              
        // Error cases    
        case CODE_ERROR_DISPLAY :
          codeErrorDisplay();
          currentDisplay = previousDisplay;
          break;
    }
  }
}

void setup(){
   Serial.begin(115200);  //initial the Serial (for Bluetooth connection)
   
   pinMode(KEY_PIN, INPUT); // Set the key pin as input
   
   lcd.begin(LCD_LINE_LENGTH, LCD_NB_LINES); // Start the LCD
}

void loop(){
  commonConfig();
}
