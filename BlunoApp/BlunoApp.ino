#include <LiquidCrystal.h>
#include <Keypad.h>

#include "Config.h"

//------------------------------------------------------------------------------
// Hardware initialization
//------------------------------------------------------------------------------
// LCD Screen
LiquidCrystal lcd(LCD_RS_PIN, LCD_ENABLE_PIN, LCD_D4_PIN,LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

// Keypad
Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, NB_ROWS, NB_COLS);

//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
int ingameCodeLength = MIN_CODE_LENGTH;

int gameMode = ANTI_TERRO_A;

int gameDuration = MIN_GAME_DURATION;

int keyOrCode = KEY_OR_CODE;

String gameCode = "";

int defuseDuration = MIN_DEFUSE_DURATION;

int nbOfTeams = MIN_NB_TEAMS;

int currentTeamNumber = 0;

bool gameGoingOn = false;

int currentDisplay = ENTER_CODE_CONFIG_DISPLAY; // Variable to determine what is to be displayed

String bluetoothCommand = "";

//------------------------------------------------------------------------------
// Photosensor
//------------------------------------------------------------------------------

bool isBoxOpen ()
{
  if(analogRead(PHOTOSENSOR_PIN) > PHOTOSENSOR_THRESHOLD) return true;
  return false;
}

//------------------------------------------------------------------------------
// User input
//------------------------------------------------------------------------------

bool readKey ()
{
  return digitalRead(KEY_PIN);
}

char readPad ()
{
  return myKeypad.getKey();
}

String readAndDisplayUserInput (int padInputType, bool doReadPad, bool doReadKey, int expectedLength = MAX_CODE_LENGTH)
{
  // Variables for pad read
  String currentString = "";
  char keyPressed;

  // Variables for key read
  bool currentKeyState = readKey();
  bool previousKeyState = currentKeyState;

  while(true)
  {
    if (gameGoingOn)
    {
      if (displayGameTimeLeft () == true) return ENDGAME_STRING;
    }
    // Code read and update
    keyPressed = readPad();

    if (keyPressed == 'D')
    {
      if (currentString.length() < expectedLength)
      {
        lcd.setCursor(0,1);
        lcd.print("Invalid value   ");
        delay(1000);
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print(currentString);
      }
      else break;
    }
    else if (keyPressed == 'C')
    {
      currentString.remove(currentString.length() - 1);
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(currentString);
    }
    else if (keyPressed != NO_KEY && currentString.length() < expectedLength)
    {
      currentString += keyPressed;
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print(currentString);
    }

    currentKeyState = readKey();
    if (currentKeyState != previousKeyState)
    {
      return KEY_DEACTIVATION;
    }
    else previousKeyState = currentKeyState;
  }
  return currentString;
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
    keyPressed = readPad();

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
    keyPressed = readPad();

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

//------------------------------------------------------------------------------
// Time Management Methods
//------------------------------------------------------------------------------
long int gameStartingTime;
long int gameEndingTime;
long int explosionStartingTime;
long int explosionEndingTime;
bool gameDurationPassed = false;
bool explosionDurationPassed = false;

long int timeIs()
{
  return millis()/1000;
}

void startGameTimer ()
{
  gameStartingTime = timeIs();
  gameEndingTime = gameStartingTime + gameDuration * 60;
  gameGoingOn = true;
}

void startExplosionTimer ()
{
  explosionStartingTime = timeIs();
  gameEndingTime = explosionStartingTime + gameDuration * 60;
}

bool isGameEndingTimeReached ()
{
  if (gameEndingTime <= timeIs())
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
  if (explosionEndingTime <= timeIs())
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
    long int secondsLeft = gameEndingTime - timeIs();
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
    long int secondsLeft = explosionEndingTime - timeIs();
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

//------------------------------------------------------------------------------
// Config Displays Methods and Functions
//------------------------------------------------------------------------------
/**
 * Enter Code Config Display
 */
void enterCodeConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Config code :");

  while (readAndDisplayUserInput(CODE, true, false, CONFIG_CODE.length()) != CONFIG_CODE)
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
      bluetoothCommand = Serial.readString();
      if (bluetoothCommand == "VALID_CONFIG") return 1;
      else if (bluetoothCommand == "CANCEL_CONFIG") return 0;
    }
  }
}

/**
 * Game Mode Config Display
 */
void gameModeConfigDisplay ()
{
  int scroll = 0;
  long int currentTime = timeIs();
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
    keyPressed = readPad();

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

    currentTime = timeIs();

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
    keyPressed = (int) readPad();

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
  ingameCodeLength = readNumber(NB_DIGITS_FOR_CODE_LENGTH, MIN_CODE_LENGTH, MAX_CODE_LENGTH);
}

/**
 * Code Definition Config Display
 */
void codeDefinitionConfigDisplay ()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("New code :");
  gameCode = readAndDisplayUserInput(CODE, true, false, ingameCodeLength);
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

  while(readPad() != 'D'){}

  // Initialize the random seed (must debranch A1 before startin the system!)
  randomSeed(analogRead(1));

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Please replug A1,");
  lcd.setCursor(0,1);
  lcd.print("then press D !");

  while(readPad() != 'D'){}

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Code is :");
  lcd.setCursor(0,1);

  gameCode = "";
  for (int i = 0; i < ingameCodeLength; i++)
  {
    // Add a random number from 0 to 9 to the code string
    gameCode += (char) random(48, 58);
  }

  lcd.print(gameCode);

  while(readPad() != 'D'){}
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

  bool currentKeyState = readKey();
  bool previousKeyState = currentKeyState;
  while (true)
  {
    currentKeyState = readKey();
    if (currentKeyState != previousKeyState) break;
    else previousKeyState = currentKeyState;
  }
}

//------------------------------------------------------------------------------
// Begin/End Displays Methods and Functions
//------------------------------------------------------------------------------
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

  while(readPad() != 'D'){}

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

  while(readPad() != 'D'){}
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

  while(readPad() != 'D'){}
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

  while(readPad() != 'D'){}
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

  while(readPad() != 'D'){}
}

//------------------------------------------------------------------------------
// Ingame Displays Methods and Functions
//------------------------------------------------------------------------------
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
    readCode = readAndDisplayUserInput(CODE, true, false, ingameCodeLength);
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

  bool currentKeyState = readKey();
  bool previousKeyState = currentKeyState;
  while (true)
  {
    currentKeyState = readKey();
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

  String returnCode = readAndDisplayUserInput(CODE, true, true, ingameCodeLength);

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
      returnCode = readAndDisplayUserInput(CODE, true, true, ingameCodeLength);
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
  gameCode = readAndDisplayUserInput(CODE, true, false, ingameCodeLength);
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
    currentKeyState = readKey();
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

  String returnCode = readAndDisplayUserInput(CODE, true, true, ingameCodeLength);

  if (returnCode == KEY_DEACTIVATION) return 1;
  else
  {
    gameCode == returnCode;
    return 2;
  }
}

//------------------------------------------------------------------------------
// Error Displays Methods and Functions
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
// Config Methods
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// Game Mode Methods
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
// End Mode Methods
//------------------------------------------------------------------------------
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
        currentDisplay = TERROS_WIN_END_DISPLAY;
      }
      else
      {
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

//------------------------------------------------------------------------------
// Arduino Setup and Loop methods (Entry point for the program)
//------------------------------------------------------------------------------
void setup(){
  // Start serial communication for the Bluetooth feature
  Serial.begin(115200);

  // Set the KEY_PIN as an input
  pinMode(KEY_PIN, INPUT);

  // Start the LCD Screen
  lcd.begin(LCD_LINE_LENGTH, LCD_NB_LINES);
}

void loop(){
  // Configuration phase
  commonConfig();

  // Ingame phase
  commonGame();

  // EndGame phase
  commonEnd();
}
