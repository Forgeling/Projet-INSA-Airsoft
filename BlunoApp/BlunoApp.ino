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
#define MIN_GAME_DURATION 1
#define DURATION_DIGITS 2
int gameDuration = MIN_GAME_DURATION;

// Key/Code choice
const int KEY_ONLY = (int) '1';
const int CODE_ONLY = (int) '2';
const int KEY_OR_CODE = (int) '3';
const int KEY_AND_CODE = (int) '4';
int keyOrCode = KEY_OR_CODE;

// Game code
String gameCode = "";

// Time for defusing
#define MIN_DEFUSE_DURATION 1
int defuseDuration = MIN_DEFUSE_DURATION;

// Teams
#define MAX_NB_TEAMS 8
#define MIN_NB_TEAMS 2
#define NB_TEAMS_DIGITS 1
int nbOfTeams = MIN_NB_TEAMS;
int currentTeamNumber = 0;

// Key deactivation message
const String KEY_DEACTIVATION = "KEY_DEACTIVATION";

// Endgame string
const String ENDGAME_STRING = "ENDGAME_STRING";
bool gameGoingOn = false;

//------------------------------------------------------------------------------------------
// Photosensor Configuration and methods
//------------------------------------------------------------------------------------------
// set key pin:
#define PHOTOSENSOR_PIN 1
#define PHOTOSENSOR_THRESHOLD 300

bool isBoxOpen ()
{
  if(analogRead(PHOTOSENSOR_PIN) > PHOTOSENSOR_THRESHOLD) return true;
  return false;
}

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
#define KEY_PIN 2

bool readKeyState ()
{
  return digitalRead(KEY_PIN);
}
//------------------------------------------------------------------------------------------
// Time Management Methods
//------------------------------------------------------------------------------------------
long int gameStartingTime;
long int gameEndingTime;
long int explosionStartingTime;
long int explosionEndingTime;
bool gameDurationPassed = false;
bool explosionDurationPassed = false;

void startGameTimer ()
{
  gameStartingTime = millis()/1000;
  gameEndingTime = gameStartingTime + gameDuration * 60;
  gameGoingOn = true;
}

void startExplosionTimer ()
{
  explosionStartingTime = millis()/1000;
  gameEndingTime = explosionStartingTime + gameDuration * 60;
}

bool isGameEndingTimeReached ()
{
  if (gameEndingTime <= millis()/1000)
  {
    gameDurationPassed = true;
    return gameDurationPassed;
  }
  else
  {
    gameDurationPassed = false;
    return false;
  }
}

bool isExplosionEndingTimeReached ()
{
  if (explosionEndingTime <= millis()/1000)
  {
    explosionDurationPassed = true;
    return explosionDurationPassed;
  }
  else
  {
    explosionDurationPassed = false;
    return false;
  }
}

bool displayGameTimeLeft ()
{
  if(isGameEndingTimeReached() == false)
  {
    lcd.setCursor(11, 0);
    lcd.print("      ");
    lcd.setCursor(11, 0);
    long int secondsLeft = gameEndingTime - millis()/1000;
    long int minutesLeft = secondsLeft / 60;
    secondsLeft = secondsLeft % 60;
    lcd.print(minutesLeft);
    lcd.print(":");
    lcd.print(secondsLeft);
    delay(20);
    return false;
  }
  else return true;
}

bool displayExplosionTimeLeft ()
{
  if(isExplosionEndingTimeReached() == false)
  {
    lcd.setCursor(11, 0);
    lcd.print("      ");
    lcd.setCursor(11, 0);
    long int secondsLeft = explosionEndingTime - millis()/1000;
    long int minutesLeft = secondsLeft / 60;
    secondsLeft = secondsLeft % 60;
    lcd.print(minutesLeft);
    lcd.print(":");
    lcd.print(secondsLeft);
    delay(20);
    return false;
  }
  else return true;
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

String readCodeAndKey (int expectedLength = MAX_CODE_LENGTH)
{
  // Variables for code read
  String theCode = "";
  char keyPressed;

  // Variables for key read
  bool currentKeyState = readKeyState();
  bool previousKeyState = currentKeyState;
  
  while(true)
  {
    if (gameGoingOn)
    {
      if (displayGameTimeLeft () == true) return ENDGAME_STRING;
    }
    // Code read and update
    keyPressed = myKeypad.getKey();

    if (keyPressed == 'D')
    {
      if (theCode.length() < expectedLength)
      {
        lcd.setCursor(0,1);
        lcd.print("Invalid value   ");
        delay(1000);
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print(theCode);
      }
      else break;
    }
    else if (keyPressed == 'C')
    {
      theCode.remove(theCode.length() - 1);
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(theCode);
    }
    else if (keyPressed != NO_KEY && theCode.length() < expectedLength)
    {
      theCode += keyPressed;
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(theCode);
    }

    currentKeyState = readKeyState();
    if (currentKeyState != previousKeyState)
    {
      return KEY_DEACTIVATION;
    }
    else previousKeyState = currentKeyState;
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
#define ENTER_CODE_CONFIG_DISPLAY 0
#define MANUAL_OR_BLUETOOTH_CONFIG_DISPLAY 1
#define WAITING_BLUETOOTH_CONFIG_DISPLAY 2
#define GAME_MODE_CONFIG_DISPLAY 3
#define GAME_DURATION_CONFIG_DISPLAY 4
#define KEY_CODE_CONFIG_DISPLAY 5
#define CODE_LENGTH_CONFIG_DISPLAY 6
#define CODE_DEFINITION_CONFIG_DISPLAY 7
#define TIME_FOR_DEFUSING_CONFIG_DISPLAY 8
#define NUMBER_OF_TEAMS_CONFIG_DISPLAY 9
#define RANDOM_CODE_GENERATION_DISPLAY 10
#define KEY_CONFIRMATION_CONFIG_DISPLAY 11

// Begin/end diplays
#define READY_TO_START_DISPLAY 100
#define NO_ONES_WIN_END_DISPLAY 101
#define TERROS_WIN_END_DISPLAY 102
#define ANTIS_WIN_END_DISPLAY 103
#define TEAM_NUMBER_WIN_END_DISPLAY 104

// Ingame displays
#define WAITING_FOR_CODE_INGAME_DISPLAY 200
#define WAITING_FOR_KEY_INGAME_DISPLAY 201
#define WAITING_FOR_EITHER_INGAME_DISPLAY 202
#define WAITING_FOR_TEAM_NUMBER_INGAME_DISPLAY 203
#define WAITING_FOR_CODE_ACTIVATION_INGAME_DISPLAY 204
#define WAITING_FOR_KEY_DEACTIVATION_INGAME_DISPLAY 205
#define WAITING_FOR_EITHER_ACTIVATION_INGAME_DISPLAY 206

// Errors
#define BOX_OPENED_ERROR_DISPLAY 300

// Variables to keep track of what is being displayed
int currentDisplay = ENTER_CODE_CONFIG_DISPLAY;

//------------------------------------------------------------------------------------------
// Config Displays Methods and Functions
//------------------------------------------------------------------------------------------
/**
 * Enter Code Config Display
 */
void enterCodeConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Config code :");

  while (readCodeAndKey(CONFIG_CODE.length()) != CONFIG_CODE)
  {
    lcd.setCursor(0,1);
    lcd.print("Invalid value   ");
    delay(1000);
    lcd.setCursor(0,1);
    lcd.print("                ");
  }
}

/**
 * Manual Or Bluetooth Config Display
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
    keyPressed = myKeypad.getKey();
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
  lcd.print("New code :");
  gameCode = readCodeAndKey(prefCodeLength);
}

/**
 * Time For Defusing Config Display
 */
void timeForDefusingConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Defuse time :");
  defuseDuration = readDuration(MIN_DEFUSE_DURATION);
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

/**
 * Key Confirmation Config Display
 */
void keyConfirmationConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Turn the key");
  lcd.setCursor(0,1);
  lcd.print("to start !");

  bool currentKeyState = readKeyState();
  bool previousKeyState = currentKeyState;
  while (true)
  {
    currentKeyState = readKeyState();
    if (currentKeyState != previousKeyState) break;
    else previousKeyState = currentKeyState;
  }
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

  startGameTimer();
}

/**
 * No One's Win End Display
 */
void noOnesWinEndDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("No one's win !");
  lcd.setCursor(0,1);
  lcd.print("Press D.");

  while(myKeypad.getKey() != 'D'){}
}

/**
 * Terros Win End Display
 */
void terrosWinEndDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Terros win !");
  lcd.setCursor(0,1);
  lcd.print("Press D.");

  while(myKeypad.getKey() != 'D'){}
}

/**
 * Antis Win End Display
 */
void antisWinEndDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Antis win !");
  lcd.setCursor(0,1);
  lcd.print("Press D.");

  while(myKeypad.getKey() != 'D'){}
}

/**
 * Team Number Win End Display
 */
void teamNumberWinEndDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Team ");
  lcd.print(currentTeamNumber);
  lcd.print("wins !");
  lcd.setCursor(0,1);
  lcd.print("Press D.");

  while(myKeypad.getKey() != 'D'){}
}

//------------------------------------------------------------------------------------------
// Ingame Displays Methods and Functions
//------------------------------------------------------------------------------------------
/**
 * Waiting for code ingame display
 */
void waitingForCodeIngameDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("The code :");

  String readCode = "";

  while (readCode != gameCode)
  {
    readCode = readCodeAndKey(prefCodeLength);
    if (readCode == ENDGAME_STRING) break;
    else if (readCode == gameCode) break;
    else
    {
      lcd.setCursor(0,1);
      lcd.print("Invalid value   ");
      delay(1000);
      lcd.setCursor(0,1);
      lcd.print("                ");
    }
  }
}

/**
 * Waiting for key ingame display
 */
void waitingForKeyIngameDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Turn");
  lcd.setCursor(0,1);
  lcd.print("the key !");

  bool currentKeyState = readKeyState();
  bool previousKeyState = currentKeyState;
  while (true)
  {
    currentKeyState = readKeyState();
    if (currentKeyState != previousKeyState) break;
    else previousKeyState = currentKeyState;
  }
}

/**
 * Waiting for either ingame display
 */
int waitingForEitherIngameDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Key/code :");

  String returnCode = readCodeAndKey(prefCodeLength);

  while (true)
  {
    if (returnCode == gameCode)
    {
      return 0;
    }
    else if (returnCode == KEY_DEACTIVATION)
    { 
      return 1;
    }
    else
    {
      lcd.setCursor(0,1);
      lcd.print("Invalid value   ");
      delay(1000);
      lcd.setCursor(0,1);
      lcd.print("                ");
      returnCode = readCodeAndKey(prefCodeLength);
    }
  }
}

/**
 * Waiting for team number ingame display
 */
void waitingForTeamNumberIngameDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Team nb :");
  currentTeamNumber = readNumber(NB_TEAMS_DIGITS, MIN_NB_TEAMS, nbOfTeams);
}

/**
 * Waiting for code activation ingame display
 */
void waitingForCodeActivationIngameDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("New Code :");
  gameCode = readCodeAndKey(prefCodeLength);
}

/**
 * Waiting for key activation ingame display
 */
void waitingForKeyActivationIngameDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Turn");
  lcd.setCursor(0,1);
  lcd.print("the key !");

  bool currentKeyState;
  bool previousKeyState;
  while (true)
  {
    currentKeyState = readKeyState();
    if (currentKeyState != previousKeyState) break;
    else previousKeyState = currentKeyState;
  }
}

/**
 * Waiting for either activation ingame display
 */
int waitingForEitherActivationIngameDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Key/code :");

  String returnCode = readCodeAndKey(prefCodeLength);

  if (returnCode == KEY_DEACTIVATION) return 1;
  else
  {
    gameCode == returnCode;
    return 2;
  }
}

//------------------------------------------------------------------------------------------
// Error Displays Methods and Functions
//------------------------------------------------------------------------------------------

/**
 * Box Opened Error Display
 */
void boxOpenedErrorDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("BOX OPENED !");
  lcd.setCursor(0,1);
  lcd.print("PLEASE RESET !");
}

//------------------------------------------------------------------------------------------
// Config Methods
//------------------------------------------------------------------------------------------
void antiTerroAConfig ()
{

}

void antiTerroDConfig ()
{
  if (keyOrCode == KEY_ONLY) currentDisplay = KEY_CONFIRMATION_CONFIG_DISPLAY;
  else currentDisplay = CODE_DEFINITION_CONFIG_DISPLAY;
  
  while (true)
  {
    switch (currentDisplay)
    {
      case CODE_DEFINITION_CONFIG_DISPLAY :
        codeDefinitionConfigDisplay();
        if (keyOrCode == CODE_ONLY) return;
        else currentDisplay = KEY_CONFIRMATION_CONFIG_DISPLAY;
        break;
        
      case KEY_CONFIRMATION_CONFIG_DISPLAY :
        keyConfirmationConfigDisplay();
        return;
    }
  }
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
  currentDisplay = ENTER_CODE_CONFIG_DISPLAY;
  int displayReturnValue;

  while (true)
  {
      switch (currentDisplay)
      {
        // General cases
        case ENTER_CODE_CONFIG_DISPLAY :
          enterCodeConfigDisplay();
          currentDisplay = MANUAL_OR_BLUETOOTH_CONFIG_DISPLAY;
          break;

        case MANUAL_OR_BLUETOOTH_CONFIG_DISPLAY :
          displayReturnValue = manualOrBluetoothConfigDisplay();
          if (displayReturnValue == 1) currentDisplay = GAME_MODE_CONFIG_DISPLAY;
          else if (displayReturnValue == 2) currentDisplay = WAITING_BLUETOOTH_CONFIG_DISPLAY;
          break;

        case WAITING_BLUETOOTH_CONFIG_DISPLAY :
          displayReturnValue = bluetoothConfigDisplay();
          if (displayReturnValue == 1) currentDisplay = READY_TO_START_DISPLAY;
          else currentDisplay = MANUAL_OR_BLUETOOTH_CONFIG_DISPLAY;
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
          if (keyOrCode != KEY_ONLY) currentDisplay = CODE_LENGTH_CONFIG_DISPLAY;
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
          return;
    }
  }
}

//------------------------------------------------------------------------------------------
// Game Mode Methods
//------------------------------------------------------------------------------------------

void antiTerroAGame ()
{

}

void antiTerroDGame ()
{
  switch (keyOrCode)
  {
    case KEY_ONLY :
      currentDisplay = WAITING_FOR_KEY_INGAME_DISPLAY;
      break;
    case CODE_ONLY :
      currentDisplay = WAITING_FOR_CODE_INGAME_DISPLAY;
      break;
    case KEY_OR_CODE :
      currentDisplay = WAITING_FOR_EITHER_INGAME_DISPLAY;
      break;
    case KEY_AND_CODE :
      currentDisplay = WAITING_FOR_EITHER_INGAME_DISPLAY;
      break;
  }

  int displayReturnValue;
  
  while (true)
  {
    switch (currentDisplay)
    {
      case WAITING_FOR_CODE_INGAME_DISPLAY :
        waitingForCodeIngameDisplay();
        return;
        
      case WAITING_FOR_KEY_INGAME_DISPLAY :
        waitingForKeyIngameDisplay();
        return;

      case WAITING_FOR_EITHER_INGAME_DISPLAY :
        displayReturnValue = waitingForEitherIngameDisplay();
        if (keyOrCode == KEY_AND_CODE)
        {
          if (displayReturnValue == 0) currentDisplay = WAITING_FOR_KEY_INGAME_DISPLAY;
          else if (displayReturnValue == 1) currentDisplay = WAITING_FOR_CODE_INGAME_DISPLAY;
        }
        else return;
    }
  }
}

void captureGame ()
{
  
}

void defuseGame ()
{
  
}

void commonGame ()
{
  switch (gameMode)
  {
    case ANTI_TERRO_A :
      antiTerroAGame();
      break;
    case ANTI_TERRO_D :
      antiTerroDGame();
      break;
    case CAPTURE :
      captureGame();
      break;
    case DEFUSE :
      defuseGame();
      break;
  }
}

//------------------------------------------------------------------------------------------
// End Mode Methods
//------------------------------------------------------------------------------------------
void commonEnd ()
{
  gameGoingOn = false;
  
  switch (gameMode)
  {
    case ANTI_TERRO_A :
      break;
    case ANTI_TERRO_D :
      if (gameDurationPassed)
      {
        Serial.println("TERROS_WIN_END_DISPLAY");
        currentDisplay = TERROS_WIN_END_DISPLAY;
      }
      else
      {
        Serial.println("ANTIS_WIN_END_DISPLAY")
        currentDisplay = ANTIS_WIN_END_DISPLAY;
      }
      break;
    case CAPTURE :
      break;
    case DEFUSE :
      break;
  }

  switch (currentDisplay)
  {
    case NO_ONES_WIN_END_DISPLAY :
      noOnesWinEndDisplay();
      break;

    case TERROS_WIN_END_DISPLAY :
      terrosWinEndDisplay();
      break;

    case ANTIS_WIN_END_DISPLAY :
      antisWinEndDisplay();
      break;

    case TEAM_NUMBER_WIN_END_DISPLAY :
      teamNumberWinEndDisplay();
      break;
  }
}

void setup(){
   Serial.begin(115200);  //initial the Serial (for Bluetooth connection)

   pinMode(KEY_PIN, INPUT); // Set the key pin as input

   lcd.begin(LCD_LINE_LENGTH, LCD_NB_LINES); // Start the LCD
}

void loop(){
  commonConfig();
  commonGame();
  commonEnd();
}

